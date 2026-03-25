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
    pub struct Box<T>(T);
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
    pub trait IovIter {}
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
    pub struct Registration<T: ?Sized>(core::marker::PhantomData<T>);
    pub trait RegistrationOps {
        type OpenData;
        type PrivateData;
    }
    impl<T: ?Sized> Registration<T> {
        pub fn new_pinned(name: &'static crate::prelude::c_str, minor: u32, dev: core::pin::Pin<crate::prelude::Box<T>>) -> crate::prelude::Result<Self> { Ok(Self(core::marker::PhantomData)) }
    }
}
pub mod mm {
    pub mod virt {
        pub struct VmArea;
    }
}
pub mod bindings {
    pub fn do_gettimeofday(_tv: *mut super::Timeval) {}
    pub type dma_chan = ();
    pub type dma_addr_t = u64;
}
pub struct Timeval { pub tv_sec: i64, pub tv_usec: i64 }
pub macro pr_info($($tt:tt)*) {}
pub macro c_str($($tt:tt)*) { unsafe { core::ffi::CStr::from_bytes_with_nul_unchecked(b"mock\0") } }
