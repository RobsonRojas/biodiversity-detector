use esp_idf_hal::i2s::{I2sConfig, I2sDriver, I2sMode, I2sStandard, I2sSlotMode, I2sDataBitWidth, I2sSlotBitWidth, I2sClockConfig};
use esp_idf_hal::peripherals::Peripherals;
use anyhow::Result;

pub struct AudioI2S<'d> {
    driver: I2sDriver<'d, esp_idf_hal::i2s::I2sRx>,
}

impl<'d> AudioI2S<'d> {
    pub fn new(peripherals: Peripherals) -> Result<Self> {
        let i2s = peripherals.i2s0;
        let bclk = peripherals.pins.gpio4; // Change pins as per Heltec V3/T-Beam
        let ws = peripherals.pins.gpio5;
        let din = peripherals.pins.gpio6;

        let config = I2sConfig::new_default()
            .mode(I2sMode::Master)
            .standard(I2sStandard::Philips)
            .slot_mode(I2sSlotMode::Mono)
            .data_bit_width(I2sDataBitWidth::Bits32)
            .slot_bit_width(I2sSlotBitWidth::Bits32)
            .sample_rate(16000);

        let driver = I2sDriver::new_rx(
            i2s,
            &config,
            bclk,
            ws,
            None::<esp_idf_hal::gpio::AnyIOPin>,
            din,
        )?;

        Ok(Self { driver })
    }

    pub fn read(&mut self, buffer: &mut [i32]) -> Result<usize> {
        // Cast i32 buffer to u8 slice for the driver
        let byte_buffer = unsafe {
            std::slice::from_raw_parts_mut(
                buffer.as_mut_ptr() as *mut u8,
                buffer.len() * std::mem::size_of::<i32>(),
            )
        };

        let bytes_read = self.driver.read_raw(byte_buffer, esp_idf_hal::delay::BLOCK)?;
        Ok(bytes_read / std::mem::size_of::<i32>())
    }

    pub fn encode_mu_law(samples: &[i32], output: &mut [u8]) -> usize {
        let mut count = 0;
        for &sample in samples.iter() {
            // Take 16-bit MSB from 32-bit sample
            let pcm16 = (sample >> 16) as i16;
            output[count] = Self::linear_to_mu_law(pcm16);
            count += 1;
            if count >= output.len() { break; }
        }
        count
    }

    fn linear_to_mu_law(pcm: i16) -> u8 {
        let mut pcm = pcm;
        let mut mask: u8 = 0;
        if pcm < 0 {
            pcm = -pcm;
            mask = 0x7F;
        } else {
            mask = 0xFF;
        }

        if pcm > 32635 { pcm = 32635; }
        pcm += 132;

        let mut exponent: u8 = 7;
        let mut exp_mask: i16 = 0x4000;
        while (pcm & exp_mask) == 0 && exponent > 0 {
            exponent -= 1;
            exp_mask >>= 1;
        }

        let mantissa = ((pcm >> (exponent + 3)) & 0x0F) as u8;
        let mu_law = (exponent << 4) | mantissa;
        
        !(mu_law ^ mask)
    }
}
