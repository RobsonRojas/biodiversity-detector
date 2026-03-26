use kernel::prelude::*;

/// BCM2837 Peripheral Base (BCM2835_PERI_BASE)
pub const BCM2837_PERI_BASE: usize = 0x3f000000;

/// PCM (I2S) Register Offsets
pub mod pcm {
    pub const BASE_OFFSET: usize = 0x203000;
    
    pub const CS_A: usize = 0x00;   // Control and Status
    pub const FIFO_A: usize = 0x04; // FIFO Data
    pub const MODE_A: usize = 0x08; // Operating Mode
    pub const RXC_A: usize = 0x0c;  // Receive Configuration
    pub const TXC_A: usize = 0x10;  // Transmit Configuration
    pub const DREQ_A: usize = 0x14; // DMA Request Control
    pub const INTEN_A: usize = 0x18; // Interrupt Enable
    pub const INTSTC_A: usize = 0x1c; // Interrupt Status
    pub const GRAY: usize = 0x20;   // Gray Mode Control
}

/// DMA Register Offsets
pub mod dma {
    pub const BASE_OFFSET: usize = 0x007000;
    pub const CH2_OFFSET: usize = 0x200;
    
    pub const CS: usize = 0x00;     // Control and Status
    pub const CONBLK_AD: usize = 0x04; // Control Block Address
    pub const DEBUG: usize = 0x20;
}

/// Helper for bit-field manipulation (simplified for demo)
pub struct Reg {
    addr: *mut u32,
}

impl Reg {
    pub unsafe fn new(base: *mut libc::c_void, offset: usize) -> Self {
        Self { addr: (base as usize + offset) as *mut u32 }
    }

    pub unsafe fn write(&self, val: u32) {
        core::ptr::write_volatile(self.addr, val);
    }

    pub unsafe fn read(&self) -> u32 {
        core::ptr::read_volatile(self.addr)
    }

    pub unsafe fn set_bits(&self, mask: u32) {
        let val = self.read();
        self.write(val | mask);
    }
}
