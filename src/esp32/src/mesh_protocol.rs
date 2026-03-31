use core::mem;

#[repr(C, packed)]
#[derive(Debug, Clone, Copy)]
pub struct MeshHeader {
    pub target_id: u16,
    pub sender_id: u16,
    pub prev_hop_id: u16,
    pub battery_mv: u16,
    pub last_rssi: i8,
    pub hop_limit: u8,
    pub payload_len: u8,
    pub signature: u16,
}

impl MeshHeader {
    pub fn new(sender: u16, target: u16, bat: u16, rssi: i8) -> Self {
        Self {
            target_id: target,
            sender_id: sender,
            prev_hop_id: sender,
            battery_mv: bat,
            last_rssi: rssi,
            hop_limit: 10,
            payload_len: 0,
            signature: 0xABCD,
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

#[repr(C, packed)]
#[derive(Debug, Clone, Copy)]
pub struct MeshDataPacket {
    pub header: MeshHeader,
    pub session_id: u16,
    pub frag_index: u16,
    pub total_frags: u16,
    pub data: [u8; 180],
}

impl MeshDataPacket {
    pub fn new(sender: u16, target: u16, session: u16, index: u16, total: u16) -> Self {
        let mut header = MeshHeader::new(sender, target, 0, 0);
        header.payload_len = 180 + 6; // data + session/frag/total
        Self {
            header,
            session_id: session,
            frag_index: index,
            total_frags: total,
            data: [0u8; 180],
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
}

impl Default for MeshHeader {
    fn default() -> Self {
        unsafe { mem::zeroed() }
    }
}
