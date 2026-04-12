import subprocess
import os
import socket
import time
import signal
import sys
import math
import json
import base64

# Configuration
QEMU_BIN = "qemu-system-xtensa"
DIST_DIR = "dist"
FLASH_BIN = "flash.bin"
UART_PORT = 45679
UDP_LORA_PORT = 5000
UDP_IP = "127.0.0.1"
TRIG_CHAINSAW = "/tmp/chainsaw_trigger"
TRIG_FROG = "/tmp/frog_trigger"

# Propagation Constants
REFERENCE_RSSI = -45.0
N_FACTOR = 3.5

# Simulation Environment
SIM_LAT = float(os.getenv("SIM_LAT", "-3.0096"))
SIM_LON = float(os.getenv("SIM_LON", "-59.9485"))
NODE_ID_INT = int(os.getenv("NODE_ID", "0x0001"), 16)

class QEMURunner:
    def __init__(self, node_id="0x0001"):
        self.node_id = node_id
        self.qemu_proc = None
        self.uart_sock = None
        self.udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def generate_flash_image(self):
        """Build a 4MB flash image from components."""
        print("[QEMURunner] Generating 4MB flash image...")
        flash = bytearray([0xFF] * (4 * 1024 * 1024))
        
        components = [
            ("bootloader.bin", 0x0),
            ("partition-table.bin", 0x8000),
            ("esp32.bin", 0x10000)
        ]
        
        for name, offset in components:
            path = os.path.join(DIST_DIR, name)
            if not os.path.exists(path):
                print(f"[QEMURunner] WARNING: Component {name} missing, skipping at 0x{offset:x}")
                continue
            with open(path, "rb") as f:
                data = f.read()
                flash[offset:offset+len(data)] = data
                print(f"[QEMURunner] Loaded {name} at 0x{offset:x} ({len(data)} bytes)")
        
        with open(FLASH_BIN, "wb") as f:
            f.write(flash)
        print(f"[QEMURunner] Flash image ready: {FLASH_BIN}")

    def verify_binaries(self):
        """Task 2.1: Verify firmware artifacts exist."""
        required = ["esp32.bin"]
        for f in required:
            path = os.path.join(DIST_DIR, f)
            if not os.path.exists(path):
                print(f"[QEMURunner] ERROR: Missing {path}")
                return False
        return True

    def start(self):
        print(f"[QEMURunner] Starting ESP32-S3 Node {self.node_id} via QEMU...")
        
        if not self.verify_binaries():
            return

        self.generate_flash_image()

        # Task 2.2 & 3.2: Configure QEMU arguments with socket bridge
        # Using if=pflash for ESP32-S3 as suggested by error logs
        qemu_cmd = [
            QEMU_BIN,
            "-nographic",
            "-machine", "esp32s3",
            "-drive", f"file={FLASH_BIN},if=mtd,format=raw",
            "-serial", f"tcp:localhost:{UART_PORT},server,nowait",
            "-monitor", "none"
        ]

        try:
            # Capturing output to see boot process
            self.qemu_proc = subprocess.Popen(qemu_cmd)
            print(f"[QEMURunner] QEMU started (PID: {self.qemu_proc.pid})")
            
            # Wait for socket to be ready
            print(f"[QEMURunner] Connecting to UART bridge on port {UART_PORT}...")
            # Longer wait to ensure QEMU has bound the port
            for _ in range(5):
                try:
                    self.uart_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    self.uart_sock.connect(("localhost", UART_PORT))
                    self.uart_sock.setblocking(False)
                    print("[QEMURunner] Connected to UART.")
                    break
                except ConnectionRefusedError:
                    time.sleep(1)
            else:
                print("[QEMURunner] FAILED connecting to UART bridge.")
                self.stop()
                return

            self.loop()
        except Exception as e:
            print(f"[QEMURunner] FAILED: {e}")
            self.stop()

    def loop(self):
        """Task 3.3: Bridge UART to UDP MeshRelay & Handle Triggers."""
        print("[QEMURunner] Simulation bridge active. Press Ctrl+C to stop.")
        
        last_beacon_time = 0
        BEACON_INTERVAL = 30 # Seconds
        
        try:
            while True:
                # 1. Periodic Gateway Beacon Propagation (Request 10)
                current_time = time.time()
                if current_time - last_beacon_time > BEACON_INTERVAL:
                    # In Gateway mode, we broadcast our coordinates as an authoritative beacon
                    # Format: BEACON:ID:LAT:LON:ACC
                    beacon = f"BEACON:0x{NODE_ID_INT:04X}:{SIM_LAT}:{SIM_LON}:1.0\n"
                    print(f"[QEMURunner] Periodic Gateway Beacon: {beacon.strip()}")
                    self.uart_sock.sendall(f"LORA_RX:{beacon}:-20\n".encode()) 
                    last_beacon_time = current_time

                # 2. Check for file-based triggers
                if os.path.exists(TRIG_CHAINSAW):
                    print("[QEMURunner] Triggering CHAINSAW via UART...")
                    self.uart_sock.sendall(b"CMD:TRIG:CHAINSAW\n")
                    os.remove(TRIG_CHAINSAW)
                
                if os.path.exists(TRIG_FROG):
                    print("[QEMURunner] Triggering FROG via UART...")
                    self.uart_sock.sendall(b"CMD:TRIG:FROG\n")
                    os.remove(TRIG_FROG)

                try:
                    data, addr = self.udp_sock.recvfrom(1024)
                    if data:
                        payload = data.decode(errors='ignore')
                        print(f"[QEMURunner] Mesh RX: {payload} from {addr}")
                        
                        # 3. Realistic RSSI Calculation based on distance
                        try:
                            # Try to extract sender coordinates if available in payload
                            # Expecting Format: BEACON:ID:LAT:LON:ACC
                            if payload.startswith("BEACON:"):
                                parts = payload.split(":")
                                sender_lat = float(parts[2])
                                sender_lon = float(parts[3])
                                
                                # Distance in meters (rough approx for small distances)
                                d = math.sqrt(((SIM_LAT - sender_lat) * 111000)**2 + 
                                             ((SIM_LON - sender_lon) * 111000 * math.cos(math.radians(SIM_LAT)))**2)
                                
                                # Log-distance path loss model
                                d = max(1.0, d) # Avoid log(0)
                                sim_rssi = REFERENCE_RSSI - 10 * N_FACTOR * math.log10(d)
                                print(f"[QEMURunner] Calculated RSSI for distance {d:.1f}m: {sim_rssi:.1f}dBm")
                            else:
                                sim_rssi = -75 # Default for non-beacon mesh traffic
                        except Exception as e:
                            print(f"[QEMURunner] RSSI calc failed: {e}")
                            sim_rssi = -80
                        
                        # Wrap in virtual protocol for guest
                        msg = f"LORA_RX:{payload}:{int(sim_rssi)}\n"
                        self.uart_sock.sendall(msg.encode())

                    data = self.uart_sock.recv(1024)
                    if data:
                        # Forward UART output to stdout for visibility
                        sys.stdout.write(data.decode(errors='ignore'))
                        sys.stdout.flush()
                        
                        # Check if guest is sending LORA_TX
                        guest_str = data.decode(errors='ignore')
                        if guest_str.startswith("LORA_TX:"):
                            payload_to_mesh = guest_str[8:].strip()
                            # Forward to MeshRelay (UDP)
                            self.udp_sock.sendto(payload_to_mesh.encode(), (UDP_IP, UDP_LORA_PORT))
                except BlockingIOError:
                    pass
                time.sleep(0.1)
        except KeyboardInterrupt:
            self.stop()

    def stop(self):
        print("\n[QEMURunner] Stopping simulation...")
        if self.qemu_proc:
            self.qemu_proc.terminate()
        if self.uart_sock:
            self.uart_sock.close()
        print("[QEMURunner] Bye.")

if __name__ == "__main__":
    runner = QEMURunner()
    runner.start()
