import subprocess
import os
import socket
import time
import signal
import sys

# Configuration
QEMU_BIN = "qemu-system-xtensa"
DIST_DIR = "dist"
UART_PORT = 45679
UDP_LORA_PORT = 5000
UDP_IP = "127.0.0.1"

class QEMURunner:
    def __init__(self, node_id="0x0001"):
        self.node_id = node_id
        self.qemu_proc = None
        self.uart_sock = None
        self.udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def verify_binaries(self):
        """Task 2.1: Verify firmware artifacts exist."""
        required = ["esp32.bin"] # Minimally required for dummy test
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

        # Task 2.2 & 3.2: Configure QEMU arguments with socket bridge
        # -serial telnet:localhost:PORT,server,nowait exposes UART over TCP
        qemu_cmd = [
            QEMU_BIN,
            "-nographic",
            "-machine", "esp32s3",
            "-drive", f"file={DIST_DIR}/esp32.bin,if=mtd,format=raw",
            "-serial", f"tcp:localhost:{UART_PORT},server,nowait",
            "-monitor", "none"
        ]

        try:
            self.qemu_proc = subprocess.Popen(qemu_cmd, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
            print(f"[QEMURunner] QEMU started (PID: {self.qemu_proc.pid})")
            
            # Wait for socket to be ready
            print(f"[QEMURunner] Connecting to UART bridge on port {UART_PORT}...")
            time.sleep(2)
            
            self.uart_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.uart_sock.connect(("localhost", UART_PORT))
            self.uart_sock.setblocking(False)
            print("[QEMURunner] Connected to UART.")

            self.loop()
        except Exception as e:
            print(f"[QEMURunner] FAILED: {e}")
            self.stop()

    def loop(self):
        """Task 3.3: Bridge UART to UDP MeshRelay."""
        print("[QEMURunner] Simulation bridge active. Press Ctrl+C to stop.")
        try:
            while True:
                try:
                    data = self.uart_sock.recv(1024)
                    if data:
                        # Forward UART output to stdout for visibility
                        sys.stdout.write(data.decode(errors='ignore'))
                        sys.stdout.flush()
                        # Forward to MeshRelay (UDP)
                        self.udp_sock.sendto(data, (UDP_IP, UDP_LORA_PORT))
                except BlockingIOError:
                    pass
                time.sleep(0.01)
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
