pub mod audio_i2s;
pub mod circular_buffer;
pub mod lora_driver;
pub mod mesh_protocol;
pub mod detection_engine;

use esp_idf_hal::peripherals::Peripherals;
use esp_idf_svc::log::EspLogger;
use crate::audio_i2s::AudioI2S;
use crate::circular_buffer::CircularBuffer;
use crate::lora_driver::LoRaDriver;
use crate::detection_engine::DetectionEngine;
use std::sync::{Arc, Mutex};
use std::thread;
use std::time::Duration;

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

    // Thread 1: Audio Capture (Core 0)
    let buffer_capture = buffer.clone();
    thread::spawn(move || {
        let mut local_buf = [0i32; 1024];
        loop {
            if let Ok(n) = audio.read(&mut local_buf) {
                if n > 0 {
                    let mut b = buffer_capture.lock().unwrap();
                    b.push(&local_buf[..n]);
                }
            }
            thread::sleep(Duration::from_millis(10));
        }
    });

    // Thread 2: Inference & LoRa (Core 1)
    let buffer_inference = buffer.clone();
    let lora = Arc::new(Mutex::new(LoRaDriver::new(peripherals)?));
    lora.lock().unwrap().initialize()?;

    let engine = DetectionEngine::new("motoserra_detect_v1.tflite")?;

    thread::spawn(move || {
        let mut inference_frame = [0i32; 16000];
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
                if let Ok(confidence) = engine.detect_motoserra(&inference_frame) {
                    if confidence > 0.85 {
                        log::info!("[MAIN] Chainsaw Detected! Confidence: {:.2}", confidence);
                        
                        let mut header = mesh_protocol::MeshHeader::new(0x0E32, 0x0005, 3950, -85);
                        header.data_len = 0; // Alert is signaling by presence in this context
                        
                        let mut l = lora.lock().unwrap();
                        let _ = l.send(header.as_bytes());
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
