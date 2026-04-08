import socket
import sys
import threading
import time
import subprocess
import os

# Configuration
RENODE_PATH = "renode"
RESC_PATH = "renode/esp32s3-main.resc"
UART_PORT = 34568
UDP_LORA_PORT = 5000
UDP_IP = "127.0.0.1" # For local simulation bridging

class RenodeRunner:
    def __init__(self, node_id, neighbors):
        self.node_id = node_id
        self.neighbors = neighbors
        self.process = None
        self.uart_sock = None
        self.udp_sock = None

    def start(self):
        print(f"[RenodeRunner] Starting ESP32-S3 Node {self.node_id}...")
        
        # 1. Start Renode headlessly
        self.process = subprocess.Popen([
            "renode", 
            "--disable-gui", 
            "--hide-monitor",
            "--port", "34567",
            "-e", f"i @{RESC_PATH}; start"
        ])
        
        # 2. Setup UDP socket for Mesh Network
        self.udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.udp_sock.bind((UDP_IP, UDP_LORA_PORT))
        
        # 3. Connect to Renode UART Socket
        print("[RenodeRunner] Waiting for Renode UART socket...")
        time.sleep(10) # Wait for boot
        self.uart_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.uart_sock.connect(("localhost", UART_PORT))
        
        # 4. Start Bridge Threads
        threading.Thread(target=self._bridge_uart_to_udp, daemon=True).start()
        threading.Thread(target=self._bridge_udp_to_uart, daemon=True).start()
        
        print("[RenodeRunner] Bridge active.")

    def _bridge_uart_to_udp(self):
        """Read from Renode UART and send to neighbors via UDP."""
        while True:
            try:
                data = self.uart_sock.recv(1024)
                if not data: break
                for neighbor in self.neighbors:
                    self.udp_sock.sendto(data, (neighbor, UDP_LORA_PORT))
            except Exception as e:
                print(f"[Bridge] UART->UDP Error: {e}")
                break

    def _bridge_udp_to_uart(self):
        """Read from UDP Mesh and send to Renode UART."""
        while True:
            try:
                data, addr = self.udp_sock.recvfrom(1024)
                self.uart_sock.sendall(data)
            except Exception as e:
                print(f"[Bridge] UDP->UART Error: {e}")
                break

if __name__ == "__main__":
    # Example usage for Node 1
    runner = RenodeRunner("0x0001", ["172.20.0.3"])
    runner.start()
    try:
        while True: time.sleep(1)
    except KeyboardInterrupt:
        runner.process.terminate()
