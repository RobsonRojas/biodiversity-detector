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
}
