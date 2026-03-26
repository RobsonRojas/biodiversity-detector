use core::mem;

#[repr(C, packed)]
#[derive(Debug, Clone, Copy)]
pub struct MeshHeader {
    pub sender_id: u16,
    pub target_id: u16,
    pub battery_mv: u16,
    pub last_rssi: i8,
    pub hops_rem: u8,
    pub prev_hop_id: u8,
    pub data_len: u8,
}

impl MeshHeader {
    pub fn new(sender: u16, target: u16, bat: u16, rssi: i8) -> Self {
        Self {
            sender_id: sender,
            target_id: target,
            battery_mv: bat,
            last_rssi: rssi,
            hops_rem: 10,
            prev_hop_id: sender as u8, // Simplification for RPi compat
            data_len: 0,
        }
    }

    pub fn as_bytes(&self) -> &[u8] {
        unsafe {
            core::slice::from_raw_parts(
                (self as *const Self) as *const u8,
                mem::size_of::<Self>(),
            )
        }
    }

    pub fn from_bytes(bytes: &[u8]) -> Option<Self> {
        if bytes.len() < mem::size_of::<Self>() {
            return None;
        }
        let mut header = Self::default();
        unsafe {
            core::ptr::copy_nonoverlapping(
                bytes.as_ptr(),
                (&mut header as *mut Self) as *mut u8,
                mem::size_of::<Self>(),
            );
        }
        Some(header)
    }
}

impl Default for MeshHeader {
    fn default() -> Self {
        unsafe { mem::zeroed() }
    }
}
