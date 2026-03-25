//! Motoserra Guardian Audio Kernel Module
//!
//! High-performance, memory-safe audio capture using Rust-for-Linux.

#![no_std]
// Removed unstable features as RUSTC provides standard interfaces in linux 6.6

use kernel::prelude::*;
use kernel::interrupt;
use core::sync::atomic::{AtomicUsize, Ordering};
use core::pin::Pin;
use kernel::{chrdev, file, mm, iov_iter, c_str, pr_info, bindings};
use kernel::error::code::*;

const DMA_BUFFER_SIZE: usize = 64 * 1024;

module! {
    type: GuardianAudioModule,
    name: "guardian_audio",
    author: "Guardian Project",
    description: "Real-time acoustic sensor driver for Motoserra detection",
    license: "GPL",
}

struct GuardianAudioModule {
    _dev: Pin<Box<chrdev::Registration<Self>>>,
    // Task 1.1: DMA channel and buffer infrastructure
    dma_chan: Option<*mut kernel::bindings::dma_chan>,
    dma_buffer: Option<*mut core::ffi::c_void>,
    dma_addr: kernel::bindings::dma_addr_t,
    // Task 1.1: Mapped I2S registers
    i2s_regs: Option<*mut core::ffi::c_void>,
    // Task 2.3: Offset synchronization
    last_offset: AtomicUsize,
}

impl chrdev::RegistrationOps for GuardianAudioModule {
    type Private = ();

    fn open(_dev: &Self, _file: &file::File) -> Result<Self::Private> {
        pr_info!("Guardian Audio: Device opened. Starting DMA ingest.\n");
        _dev.start_dma();
        Ok(())
    }

    fn read(
        _dev: &Self,
        _file: &file::File,
        _priv: &Self::Private,
        buffer: &mut dyn iov_iter::IovIter,
        _offset: u64,
    ) -> Result<usize> {
        // Task 3.1: Read from DMA buffer
        if let Some(dma_buf) = _dev.dma_buffer {
            let offset = _dev.last_offset.load(Ordering::Acquire);
            let to_read = core::cmp::min(buffer.count(), DMA_BUFFER_SIZE / 2);
            
            // Safety: dma_buf is allocated via dma_alloc_coherent
            let src = (dma_buf as usize + offset) as *const u8;
            let slice = unsafe { core::slice::from_raw_parts(src, to_read) };
            
            buffer.copy_to_user(slice)?;
            Ok(to_read)
        } else {
            Err(ENODEV)
        }
    }

    fn release(_dev: &Self, _file: &file::File, _priv: Self::Private) -> Result<()> {
        pr_info!("Guardian Audio: Device closed. Stopping DMA.\n");
        _dev.stop_dma();
        Ok(())
    }

    fn mmap(
        _dev: &Self,
        _file: &file::File,
        _priv: &Self::Private,
        vma: &mut mm::virt::VmArea,
    ) -> Result<()> {
        pr_info!("Guardian Audio: mmap requested\n");
        // Task 3.2: Map the kernel ring buffer to user-space
        if _dev.dma_addr != 0 {
            let pfn = _dev.dma_addr >> kernel::bindings::PAGE_SHIFT;
            if kernel::bindings::remap_pfn_range(
                vma.as_ptr(),
                vma.start(),
                pfn,
                (vma.end() - vma.start()) as u64,
                vma.page_prot()
            ) != 0 {
                return Err(EAGAIN);
            }
            Ok(())
        } else {
            Err(ENODEV)
        }
    }
}

// Task 3.2: I2S Controller Register Definitions (BCM2837 - RPi 3)
// BCM2837 peripheral base: 0x3F000000 + I2S offset: 0x203000
const I2S_BASE: usize = 0x3F203000;
const I2S_LEN: usize = 0x24; // Range of registers

// Register Offsets
const CS_A: usize = 0x00;
const FIFO_A: usize = 0x04;
const MODE_A: usize = 0x08;
const RXC_A: usize = 0x0C;
const TXC_A: usize = 0x10;
const DREQ_A: usize = 0x14;
const INTEN_A: usize = 0x18;
const INTSTC_A: usize = 0x1C;
const GRAY_A: usize = 0x20;

// CS_A bits
const CS_A_EN: u32 = 1 << 0;
const CS_A_RXON: u32 = 1 << 1;
const CS_A_TXON: u32 = 1 << 2;
const CS_A_RXCLR: u32 = 1 << 3;
const CS_A_TXCLR: u32 = 1 << 4;
const CS_A_RXTHR: u32 = 3 << 5; // 0: 1/4, 1: 2/4, 2: 3/4, 3: Full-1
const CS_A_DMAEN: u32 = 1 << 9;
const CS_A_RXERR: u32 = 1 << 15;
const CS_A_RXSYNC: u32 = 1 << 17;

// MODE_A bits
const MODE_A_CLK_DIS: u32 = 1 << 28;
const MODE_A_PDME: u32 = 1 << 26;
const MODE_A_CLKM: u32 = 1 << 23;
const MODE_A_FSM: u32 = 1 << 21;

impl GuardianAudioModule {
    fn write_reg(&self, offset: usize, value: u32) {
        if let Some(regs) = self.i2s_regs {
            unsafe {
                core::ptr::write_volatile((regs as usize + offset) as *mut u32, value);
            }
        }
    }

    fn read_reg(&self, offset: usize) -> u32 {
        if let Some(regs) = self.i2s_regs {
            unsafe {
                core::ptr::read_volatile((regs as usize + offset) as *const u32)
            }
        } else {
            0
        }
    }

    fn configure_i2s(&self) {
        pr_info!("Guardian Audio: Configuring I2S for 16kHz/32-bit MONO with DMA DREQ\n");
        
        // 1. Clear FIFOs and reset CS_A
        self.write_reg(CS_A, CS_A_RXCLR | CS_A_TXCLR);
        
        // 2. Configure MODE_A: 16kHz/32-bit settings
        // For 16kHz with RPi clocking, we need specific FLEN/FSLEN
        // Assuming BCM2837 PCM clock is set externally or using default 19.2MHz
        // Here we just set the bits as per specification
        let mut mode = self.read_reg(MODE_A);
        mode |= MODE_A_CLKM | MODE_A_FSM; // Master mode, Frame Sync Master
        mode &= !MODE_A_CLK_DIS;          // Enable clock
        self.write_reg(MODE_A, mode);

        // 3. Configure RXC_A: 32-bit MONO
        let rxc = 0x1F << 20 | 0x01 << 16; // 32-bit width, 1st channel
        self.write_reg(RXC_A, rxc);

        // 4. Enable DMA DREQ signaling
        let mut cs = self.read_reg(CS_A);
        cs |= CS_A_DMAEN | CS_A_EN | CS_A_RXON;
        self.write_reg(CS_A, cs);

        pr_info!("Guardian Audio: I2S DREQ enabled (CS_A: 0x{:X}, MODE_A: 0x{:X})\n", 
                 self.read_reg(CS_A), self.read_reg(MODE_A));
    }

    // Task 3.2: Start DMA transfer
    fn start_dma(&self) {
        if let (Some(chan), Some(buf)) = (self.dma_chan, self.dma_buffer) {
            pr_info!("Guardian Audio: Starting DMA transfer loop...\n");
            
            unsafe {
                // Task 2.1: Configure DMA slave
                let mut config: kernel::bindings::dma_slave_config = core::mem::zeroed();
                config.direction = kernel::bindings::dma_transfer_direction_DMA_DEV_TO_MEM;
                config.src_addr = (I2S_BASE + FIFO_A) as u64;
                config.src_addr_width = kernel::bindings::dma_slave_buswidth_DMA_SLAVE_BUSWIDTH_4_BYTES;
                config.src_maxburst = 1;

                if kernel::bindings::dmaengine_slave_config(chan, &mut config) != 0 {
                    pr_info!("Guardian Audio: Failed to configure DMA slave\n");
                    return;
                }

                // Task 2.3: Prepare cyclic transfer
                let flags = kernel::bindings::DMA_PREP_INTERRUPT | kernel::bindings::DMA_CTRL_ACK;
                let desc = kernel::bindings::dmaengine_prep_dma_cyclic(
                    chan,
                    self.dma_addr,
                    DMA_BUFFER_SIZE as u64,
                    (DMA_BUFFER_SIZE / 2) as u64,
                    kernel::bindings::dma_transfer_direction_DMA_DEV_TO_MEM,
                    flags as u64
                );

                if desc.is_null() {
                    pr_info!("Guardian Audio: Failed to prepare cyclic DMA\n");
                    return;
                }

                // Set callback
                (*desc).callback = Some(Self::dma_callback_wrapper);
                (*desc).callback_param = self as *const _ as *mut core::ffi::c_void;

                // Issue pending
                let cookie = kernel::bindings::dmaengine_submit(desc);
                if kernel::bindings::dma_submit_error(cookie) != 0 {
                    pr_info!("Guardian Audio: Failed to submit DMA descriptor\n");
                    return;
                }

                kernel::bindings::dma_async_issue_pending(chan);
            }
        }
    }

    // Task 3.2: Stop DMA transfer
    fn stop_dma(&self) {
        if let Some(chan) = self.dma_chan {
            pr_info!("Guardian Audio: Stopping DMA transfer...\n");
            unsafe {
                kernel::bindings::dmaengine_terminate_async(chan);
            }
        }
    }

    // Task 2.2: DMA Callback wrapper
    unsafe extern "C" fn dma_callback_wrapper(param: *mut core::ffi::c_void) {
        let this = unsafe { &*(param as *const Self) };
        this.dma_callback();
    }

    fn interrupt_handler(&self) -> interrupt::ReturnValue {
        // Task 3.3: Move data from I2S FIFO to ring buffer
        // pr_info!("Guardian Audio: Interrupt received\n");
        interrupt::ReturnValue::Handled
    }

    // Task 2.2: DMA Transfer Complete Callback
    fn dma_callback(&self) {
        let current_offset = self.last_offset.load(Ordering::Relaxed);
        let next_offset = (current_offset + (DMA_BUFFER_SIZE / 2)) % DMA_BUFFER_SIZE;
        
        pr_info!("Guardian Audio: DMA Half-Buffer Complete. Syncing offset {} -> {}\n", 
                 current_offset, next_offset);
        
        self.last_offset.store(next_offset, Ordering::Relaxed);
        // Task 2.3: Here we would signal the user-space ring buffer
    }
}

impl kernel::Module for GuardianAudioModule {
    fn init(_name: &'static c_str, _module: &'static kernel::prelude::ThisModule) -> Result<Self> {
        pr_info!("Guardian Audio Module: Initializing (RPi 3 BCM2837 aarch64 with DMA support)\n");
        
        // Task 1.1: Map I2S registers
        pr_info!("Guardian Audio: Mapping I2S registers at 0x{:X}\n", I2S_BASE);
        let i2s_regs = kernel::bindings::ioremap(I2S_BASE as u64, I2S_LEN as u64);
        if i2s_regs.is_null() {
            return Err(ENOMEM);
        }

        let dev = chrdev::Registration::new_pinned::<Self>(
            c_str!("forest_audio"),
            0, // Minor number
            _module,
        )?;

        // Task 1.1 & 1.2: Request DMA channel and allocate coherent buffer
        // Note: In a real implementation, we would use dma_request_chan() 
        // and dma_alloc_coherent(). Here we show the structural integration.
        pr_info!("Guardian Audio: Requesting DMA channel for I2S RX...\n");
        pr_info!("Guardian Audio: Allocating 64KB coherent DMA buffer...\n");

        // Task 1.3: Request DMA channel
        pr_info!("Guardian Audio: Requesting DMA channel for I2S RX...\n");
        let dma_chan = unsafe { kernel::bindings::dma_request_chan(core::ptr::null_mut(), c_str!("rx").as_ptr()) };
        if dma_chan.is_null() {
            pr_info!("Guardian Audio: Failed to request DMA channel (falling back to PIO not supported)\n");
            // Clean up i2s_regs before returning
            unsafe { kernel::bindings::iounmap(i2s_regs) };
            return Err(ENODEV);
        }

        // Task 1.4: Allocate coherent DMA buffer
        pr_info!("Guardian Audio: Allocating 64KB coherent DMA buffer...\n");
        let mut dma_addr: kernel::bindings::dma_addr_t = 0;
        let dma_buffer = kernel::bindings::dma_alloc_coherent(
            core::ptr::null_mut(), // Should be device->dev
            DMA_BUFFER_SIZE as u64,
            &mut dma_addr,
            0 // GFP_KERNEL is usually implicitly handled or passed as bitmask
        );

        if dma_buffer.is_null() {
            kernel::bindings::dma_release_channel(dma_chan);
            kernel::bindings::iounmap(i2s_regs);
            return Err(ENOMEM);
        }

        let module = GuardianAudioModule { 
            _dev: dev,
            dma_chan: Some(dma_chan),
            dma_buffer: Some(dma_buffer),
            dma_addr,
            i2s_regs: Some(i2s_regs),
            last_offset: AtomicUsize::new(0),
        };

        // Initialize hardware
        module.configure_i2s();

        Ok(module)
    }
}

impl Drop for GuardianAudioModule {
    fn drop(&mut self) {
        pr_info!("Guardian Audio Module: Exiting and releasing DMA resources\n");
        // Task 3.3 & 1.3: Clean DMA channel release
        if let Some(chan) = self.dma_chan {
            pr_info!("Guardian Audio: Releasing DMA channel...\n");
            kernel::bindings::dma_release_channel(chan);
        }
        if let Some(buf) = self.dma_buffer {
            pr_info!("Guardian Audio: Freeing coherent DMA buffer...\n");
            kernel::bindings::dma_free_coherent(
                core::ptr::null_mut(),
                DMA_BUFFER_SIZE as u64,
                buf,
                self.dma_addr
            );
        }
        // Task 3.3: Unmap I2S registers
        if let Some(regs) = self.i2s_regs {
            pr_info!("Guardian Audio: Unmapping I2S registers...\n");
            kernel::bindings::iounmap(regs);
        }
    }
}
