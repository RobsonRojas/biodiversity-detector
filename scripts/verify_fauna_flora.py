import socket
import struct
import time

# Mesh Protocol Constants
SIGNATURE = 0xABCD
GATEWAY_ID = 0x0005
SENDER_ID = 0x0E32
FRAG_SIZE = 180

def create_mesh_header(target_id, sender_id, payload_len):
    # struct MeshHeader {
    #     uint16_t target_id;
    #     uint16_t sender_id;
    #     uint16_t prev_hop_id;
    #     uint16_t battery_mv;
    #     int8_t last_rssi;
    #     uint8_t hop_limit;
    #     uint8_t payload_len;
    #     uint16_t signature;
    # };
    return struct.pack('<HHHHbBBH', target_id, sender_id, sender_id, 4100, -80, 10, payload_len, SIGNATURE)

def send_alert():
    msg = "ALERT: birds detected"
    header = create_mesh_header(GATEWAY_ID, SENDER_ID, len(msg))
    packet = header + msg.encode()
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(packet, ("127.0.0.1", 5000))
    print(f"Sent Alert: {msg}")

def send_audio_fragments():
    session_id = 123
    total_samples = 16000
    total_frags = (total_samples + FRAG_SIZE - 1) // FRAG_SIZE
    
    mock_audio = bytes([i % 256 for i in range(total_samples)])
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    print(f"Streaming {total_frags} fragments for Session {session_id}...")
    
    for i in range(total_frags):
        start = i * FRAG_SIZE
        end = min(start + FRAG_SIZE, total_samples)
        fragment_data = mock_audio[start:end]
        
        # Adjust fragment_data to be exactly 180 bytes if needed (for fixed-size packet heuristic)
        if len(fragment_data) < FRAG_SIZE:
            fragment_data += b'\x00' * (FRAG_SIZE - len(fragment_data))
            
        # Payload: session_id(2), frag_index(2), total_frags(2), data(180)
        payload = struct.pack('>HHH', session_id, i, total_frags) + fragment_data
        
        header = create_mesh_header(GATEWAY_ID, SENDER_ID, len(payload))
        packet = header + payload
        
        sock.sendto(packet, ("127.0.0.1", 5000))
        if i % 10 == 0:
            print(f"Sent fragment {i}/{total_frags}")
        time.sleep(0.01) # Small delay

if __name__ == "__main__":
    send_alert()
    time.sleep(1)
    send_audio_fragments()
    print("Verification sequence complete.")
