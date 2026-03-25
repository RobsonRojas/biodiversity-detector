#![no_std]
#![feature(decl_macro)]
pub use core::pin::Pin;

pub mod prelude {
    pub use core::pin::Pin;
    pub use core::prelude::rust_2021::*;
    pub type c_str = core::ffi::CStr;
    pub macro module($($tt:tt)*) {}
    pub type Result<T = (), E = Error> = core::result::Result<T, E>;
    pub struct Error;
    pub struct Box<T: ?Sized>(core::marker::PhantomData<T>);
    pub struct ThisModule;
}
pub trait Module {
    fn init(name: &'static crate::prelude::c_str, module: &'static crate::prelude::ThisModule) -> crate::prelude::Result<Self> where Self: Sized;
}
pub mod interrupt {
    pub struct Registration;
    pub enum ReturnValue { Handled, None }
}
pub mod device {
    pub struct Device;
}
pub mod file {
    pub struct File;
}
pub mod io_buffer {
    pub struct IoBufferWriter;
}
pub mod iov_iter {
    pub trait IovIter {
        fn count(&self) -> usize;
        fn copy_to_user(&mut self, src: &[u8]) -> crate::prelude::Result<()>;
    }
}
pub mod miscdevice {
    pub struct MiscDeviceRegistration<T>(core::marker::PhantomData<T>);
    impl<T> MiscDeviceRegistration<T> {
        pub fn register(_: T) -> crate::prelude::Result<Self, crate::prelude::Error> { Ok(Self(core::marker::PhantomData)) }
    }
    pub struct MiscDeviceOptions;
}
pub mod file_operations {
    pub macro declare_file_operations($($tt:tt)*) {}
}
pub mod sync {
    pub mod smp { pub fn wmb() {} }
}
pub mod chrdev {
    use core::pin::Pin;
    pub struct Registration<T: ?Sized>(core::marker::PhantomData<T>);
    pub trait RegistrationOps {
        type Private;
        fn open(dev: &Self, file: &crate::file::File) -> crate::prelude::Result<Self::Private>;
        fn read(dev: &Self, file: &crate::file::File, priv_data: &Self::Private, buffer: &mut dyn crate::iov_iter::IovIter, offset: u64) -> crate::prelude::Result<usize>;
        fn release(dev: &Self, file: &crate::file::File, priv_data: Self::Private) -> crate::prelude::Result<()>;
        fn mmap(dev: &Self, file: &crate::file::File, priv_data: &Self::Private, vma: &mut crate::mm::virt::VmArea) -> crate::prelude::Result<()>;
    }
    impl<T: ?Sized> Registration<T> {
        pub fn new_pinned<U: RegistrationOps>(name: &'static crate::prelude::c_str, minor: u32, module: &'static crate::prelude::ThisModule) -> crate::prelude::Result<Pin<crate::prelude::Box<Self>>> { 
            Err(crate::prelude::Error)
        }
    }
}
pub mod mm {
    pub mod virt {
        pub struct VmArea;
        impl VmArea {
            pub fn as_ptr(&self) -> *mut core::ffi::c_void { core::ptr::null_mut() }
            pub fn start(&self) -> usize { 0 }
            pub fn end(&self) -> usize { 0 }
            pub fn page_prot(&self) -> u64 { 0 }
        }
    }
}
pub mod bindings {
    pub fn do_gettimeofday(_tv: *mut super::Timeval) {}
    pub type dma_chan = ();
    pub type dma_addr_t = u64;
    pub struct dma_slave_config {
        pub direction: u32,
        pub src_addr: u64,
        pub src_addr_width: u32,
        pub src_maxburst: u32,
        pub device_fc: bool,
    }
    pub const dma_transfer_direction_DMA_DEV_TO_MEM: u32 = 0;
    pub const dma_slave_buswidth_DMA_SLAVE_BUSWIDTH_4_BYTES: u32 = 4;
    pub const PAGE_SHIFT: u32 = 12;
    pub fn ioremap(_base: u64, _size: u64) -> *mut core::ffi::c_void { core::ptr::null_mut() }
    pub fn iounmap(_addr: *mut core::ffi::c_void) {}
    pub fn dma_request_chan(_dev: *mut core::ffi::c_void, _name: *const core::ffi::c_char) -> *mut dma_chan { core::ptr::null_mut() }
    pub fn dma_release_channel(_chan: *mut dma_chan) {}
    pub fn dma_alloc_coherent(_dev: *mut core::ffi::c_void, _size: u64, _addr: *mut dma_addr_t, _flags: u32) -> *mut core::ffi::c_void { core::ptr::null_mut() }
    pub fn dma_free_coherent(_dev: *mut core::ffi::c_void, _size: u64, _addr: *mut core::ffi::c_void, _phys: dma_addr_t) {}
    pub fn dmaengine_slave_config(_chan: *mut dma_chan, _config: *mut dma_slave_config) -> i32 { 0 }
    pub const DMA_PREP_INTERRUPT: u32 = 1;
    pub const DMA_CTRL_ACK: u32 = 2;
    pub struct dma_async_tx_descriptor {
        pub callback: Option<unsafe extern "C" fn(*mut core::ffi::c_void)>,
        pub callback_param: *mut core::ffi::c_void,
    }
    pub fn dmaengine_prep_dma_cyclic(_chan: *mut dma_chan, _buf: dma_addr_t, _len: u64, _period: u64, _dir: u32, _flags: u64) -> *mut dma_async_tx_descriptor { core::ptr::null_mut() }
    pub fn dmaengine_submit(_desc: *mut dma_async_tx_descriptor) -> i32 { 0 }
    pub fn dma_submit_error(_cookie: i32) -> i32 { 0 }
    pub fn dma_async_issue_pending(_chan: *mut dma_chan) {}
    pub fn dmaengine_terminate_async(_chan: *mut dma_chan) {}
    pub fn remap_pfn_range(_vma: *mut core::ffi::c_void, _addr: usize, _pfn: u64, _len: u64, _prot: u64) -> i32 { 0 }
}
pub mod error {
    pub mod code {
        pub const ENOMEM: super::super::prelude::Error = super::super::prelude::Error;
        pub const ENODEV: super::super::prelude::Error = super::super::prelude::Error;
        pub const EAGAIN: super::super::prelude::Error = super::super::prelude::Error;
    }
}
pub struct Timeval { pub tv_sec: i64, pub tv_usec: i64 }
pub macro pr_info($($tt:tt)*) {}
pub macro c_str($($tt:tt)*) { unsafe { core::ffi::CStr::from_bytes_with_nul_unchecked(b"mock\0") } }
