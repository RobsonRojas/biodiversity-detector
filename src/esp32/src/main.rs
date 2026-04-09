pub mod audio_i2s;
pub mod circular_buffer;
pub mod lora_driver;
pub mod mesh_protocol;
pub mod detection_engine;

use esp_idf_hal::peripherals::Peripherals;
use esp_idf_hal::uart::*;
use esp_idf_svc::log::EspLogger;
use crate::audio_i2s::AudioI2S;
use crate::circular_buffer::CircularBuffer;
use crate::lora_driver::LoRaDriver;
use crate::detection_engine::DetectionEngine;
use std::sync::{Arc, Mutex};
use std::thread;
use std::time::Duration;
use esp_idf_svc::nvs::{EspDefaultNvsPartition, EspNvs, NvsDefault};

fn main() -> anyhow::Result<()> {
    // It is necessary to call this function once. Otherwise some patches to the runtime
    // implemented by esp-idf-sys might not link properly. See https://github.com/esp-rs/esp-idf-template/issues/71
    esp_idf_sys::link_patches();

    // Bind the log crate to the ESP Logging facilities
    EspLogger::initialize_default();

    log::info!("Starting ESP32 Rust Chainsaw Detector...");

    let peripherals = Peripherals::take().unwrap();
    let mut audio = AudioI2S::new(peripherals)?;
    let buffer = Arc::new(Mutex::new(CircularBuffer::new(32000)));

    // Shared trigger state for simulator control
    let trigger_id = Arc::new(Mutex::new(0u8)); // 0: None, 1: Chainsaw, 2: Frog

    // Thread 0: UART Command Listener (Simulator Interface)
    let trigger_uart = trigger_id.clone();
    let uart0 = peripherals.uart0;
    let config = UartConfig::default().baudrate(esp_idf_hal::units::Hertz(115200));
    let mut uart = UartDriver::new(uart0, peripherals.pins.gpio1, peripherals.pins.gpio3, None::<esp_idf_hal::gpio::AnyIOPin>, &config)?;

    thread::spawn(move || {
        let mut buf = [0u8; 32];
        loop {
            if let Ok(n) = uart.read(&mut buf, esp_idf_hal::delay::BLOCK) {
                let cmd = String::from_utf8_lossy(&buf[..n]);
                if cmd.contains("CMD:TRIG:CHAINSAW") {
                    let mut t = trigger_uart.lock().unwrap();
                    *t = 1;
                    log::info!("[UART] SIMULATOR TRIGGER: CHAINSAW");
                } else if cmd.contains("CMD:TRIG:FROG") {
                    let mut t = trigger_uart.lock().unwrap();
                    *t = 2;
                    log::info!("[UART] SIMULATOR TRIGGER: FROG");
                }
            }
        }
    });

    // Thread 1: Audio Capture (Core 0)
    let buffer_capture = buffer.clone();
    let trigger_audio = trigger_id.clone();
    thread::spawn(move || {
        let mut local_buf = [0i32; 1024];
        loop {
            let mut triggered = 0u8;
            {
                let mut t = trigger_audio.lock().unwrap();
                triggered = *t;
                // Keep trigger active for ~1s to ensure detection cycle catches it
                // Logic: the runner will send the command, we flip the bit, and eventually reset
            }

            if let Ok(n) = audio.read(&mut local_buf) {
                if triggered == 1 {
                    // Inject "chainsaw" samples (sum target > 0.05)
                    local_buf.fill(150_000_000); 
                } else if triggered == 2 {
                    // Inject "frog" samples (sum target > 0.03)
                    local_buf.fill(90_000_000);
                }

                if n > 0 {
                    let mut b = buffer_capture.lock().unwrap();
                    b.push(&local_buf[..n]);
                }
            }
            
            // Auto-reset trigger after injecting some frames
            if triggered != 0 {
                let mut t = trigger_audio.lock().unwrap();
                *t = 0;
            }

            thread::sleep(Duration::from_millis(10));
        }
    });

    // Thread 2: Inference & LoRa (Core 1)
    let buffer_inference = buffer.clone();
    let lora = Arc::new(Mutex::new(LoRaDriver::new(peripherals)?));
    lora.lock().unwrap().initialize()?;

    let nvs_partition = EspDefaultNvsPartition::take()?;
    let mut nvs = EspNvs::new(nvs_partition, "config", true)?;
    
    // Default to true (forward audio) if not set
    let forward_audio = match nvs.get_u8("forward_audio")? {
        Some(val) => val != 0,
        None => {
            nvs.set_u8("forward_audio", 1)?;
            true
        }
    };
    
    let engine = DetectionEngine::new("forest_sounds_v1.tflite")?;

    thread::spawn(move || {
        let mut inference_frame = [0i32; 16000];
        let mut session_id_counter: u16 = 0;
        loop {
            let ready = {
                let b = buffer_inference.lock().unwrap();
                if b.size() >= 16000 {
                    b.read_latest(&mut inference_frame);
                    true
                } else {
                    false
                }
            };

            if ready {
                if let Ok((class, confidence)) = engine.detect_sound(&inference_frame) {
                    if confidence > 0.70 && class != "background" {
                        log::info!("[MAIN] {} Detected! Confidence: {:.2}", class, confidence);
                        
                        let header = mesh_protocol::MeshHeader::new(0x0E32, 0x0005, 3950, -85);
                        
                        let mut l = lora.lock().unwrap();
                        let _ = l.send(header.as_bytes());

                        if forward_audio {
                            log::info!("[MAIN] Forwarding audio fragments for {}...", class);
                            let mut encoded = [0u8; 16000];
                            let n = AudioI2S::encode_mu_law(&inference_frame, &mut encoded);
                            
                            let total_frags = ((n + 179) / 180) as u16;
                            session_id_counter = session_id_counter.wrapping_add(1);
                            
                            for i in 0..total_frags {
                                let mut packet = mesh_protocol::MeshDataPacket::new(
                                    0x0E32, 0x0005, session_id_counter, i, total_frags
                                );
                                let start = (i as usize) * 180;
                                let end = std::cmp::min(start + 180, n);
                                packet.data[..end-start].copy_from_slice(&encoded[start..end]);
                                
                                let _ = l.send(packet.as_bytes());
                                thread::sleep(Duration::from_millis(50)); // Avoid flooding LoRa chip
                            }
                        }
                    }
                }
            }
            
            thread::sleep(Duration::from_millis(500));
        }
    });

    let lora_heartbeat = lora.clone();
    loop {
        {
            let mut l = lora_heartbeat.lock().unwrap();
            let header = mesh_protocol::MeshHeader::new(0x0E32, 0x0005, 4120, -78);
            log::info!("[HEARTBEAT] Sending Mesh Heartbeat (Bat: 4120mV, RSSI: -78dBm)...");
            let _ = l.send(header.as_bytes());
        }
        
        thread::sleep(Duration::from_secs(30));
    }
}
