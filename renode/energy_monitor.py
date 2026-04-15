"""
Energy Monitor for Renode ESP32-S3 Simulation
Tracks simulated battery consumption, RSSI, and power states
"""

import time
from datetime import datetime
from collections import deque

class EnergyMonitor:
    """Monitors and simulates energy consumption in the ESP32-S3"""
    
    # Power states and consumption (mA)
    POWER_STATES = {
        'deep_sleep': 0.01,      # 10 µA
        'light_sleep': 0.1,       # 100 µA
        'idle': 1.0,              # 1 mA
        'cpu_active': 50.0,       # 50 mA
        'audio_capture': 10.0,    # 10 mA (PDM mic)
        'wifi_active': 100.0,     # 100 mA
        'lora_tx': 120.0,         # 120 mA
        'lora_rx': 30.0,          # 30 mA
    }
    
    def __init__(self, machine, battery_capacity_mah=5000, log_prefix="[Energy]"):
        self.machine = machine
        self.battery_capacity = battery_capacity_mah
        self.current_battery = battery_capacity_mah  # Start fully charged
        self.log_prefix = log_prefix
        
        # State tracking
        self.current_state = 'idle'
        self.state_start_time = time.time()
        
        # RSSI simulation (dBm, typically -50 to -120)
        self.current_rssi = -80
        
        # History for graphing
        self.history = deque(maxlen=3600)  # Keep 1 hour of history
        self.state_transitions = deque(maxlen=100)
        
        # Statistics
        self.total_energy_consumed = 0.0  # mAh
        self.stats = {state: 0.0 for state in self.POWER_STATES.keys()}
    
    def log(self, message, level="INFO"):
        """Log a message with timestamp"""
        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        print(f"{self.log_prefix} [{timestamp}] {level}: {message}")
    
    def set_state(self, new_state):
        """Transition to a new power state"""
        if new_state not in self.POWER_STATES:
            self.log(f"Unknown state: {new_state}", "WARN")
            return False
        
        # Calculate energy consumed in previous state
        if self.current_state != new_state:
            self._update_battery()
            
            elapsed_s = time.time() - self.state_start_time
            consumed = (self.POWER_STATES[self.current_state] * elapsed_s) / 3600  # Convert to mAh
            
            self.log(f"State transition: {self.current_state} → {new_state} "
                    f"(consumed {consumed:.3f} mAh)")
            
            self.state_transitions.append({
                'from': self.current_state,
                'to': new_state,
                'time': datetime.now(),
                'energy_mah': consumed
            })
            
            self.current_state = new_state
            self.state_start_time = time.time()
        
        return True
    
    def _update_battery(self):
        """Update battery level based on current state consumption"""
        elapsed_s = time.time() - self.state_start_time
        current_mah = self.POWER_STATES[self.current_state]
        
        # Calculate energy consumed (mA * hours)
        consumed = (current_mah * elapsed_s) / 3600
        
        self.current_battery -= consumed
        self.total_energy_consumed += consumed
        self.stats[self.current_state] += consumed
        
        # Clamp battery to 0
        if self.current_battery < 0:
            self.current_battery = 0
            self.log("⚠ Battery depleted!", "WARN")
    
    def get_battery_percent(self):
        """Get current battery level as percentage"""
        return max(0, (self.current_battery / self.battery_capacity) * 100)
    
    def set_rssi(self, rssi_dbm):
        """Set signal strength (dBm)"""
        if -120 <= rssi_dbm <= -30:
            self.current_rssi = rssi_dbm
            # Increase power consumption slightly with weaker signal (more TX retries)
            # This could affect nearby states like lora_tx
        else:
            self.log(f"Invalid RSSI: {rssi_dbm} dBm (must be -120 to -30)", "WARN")
    
    def get_rssi(self):
        """Get current simulated RSSI"""
        return self.current_rssi
    
    def simulate_motion(self, motion_level=0.5):
        """Simulate motion affecting RSSI (0.0 = static, 1.0 = high movement)"""
        # RSSI gets worse with movement (blockage, obstructions)
        rssi_degradation = int(motion_level * 30)  # Up to 30 dB worse
        new_rssi = -80 - rssi_degradation
        self.set_rssi(max(-120, new_rssi))
    
    def simulate_interference(self, interference_level=0.5):
        """Simulate RF interference affecting signal"""
        interference_degradation = int(interference_level * 20)
        new_rssi = -80 - interference_degradation
        self.set_rssi(max(-120, new_rssi))
    
    def print_status(self):
        """Print detailed energy monitor status"""
        self._update_battery()
        
        print(f"\n{self.log_prefix} === Energy Monitor Status ===")
        print(f"  Machine: {self.machine.GetName()}")
        print(f"  Battery: {self.get_battery_percent():.1f}% ({self.current_battery:.2f} / {self.battery_capacity} mAh)")
        print(f"  Current State: {self.current_state} ({self.POWER_STATES[self.current_state]} mA)")
        print(f"  RSSI: {self.current_rssi} dBm")
        print(f"  Total Consumed: {self.total_energy_consumed:.2f} mAh")
        print(f"  ")
        print(f"  Energy by State:")
        for state, consumed in sorted(self.stats.items(), key=lambda x: x[1], reverse=True):
            if consumed > 0:
                percent = (consumed / self.total_energy_consumed * 100) if self.total_energy_consumed > 0 else 0
                print(f"    {state:20s}: {consumed:8.3f} mAh ({percent:5.1f}%)")
        
        if self.state_transitions:
            print(f"  ")
            print(f"  Recent Transitions:")
            for trans in list(self.state_transitions)[-5:]:
                print(f"    {trans['time'].strftime('%H:%M:%S')} "
                      f"{trans['from']:15s} → {trans['to']:15s} ({trans['energy_mah']:.3f} mAh)")
        
        print()
    
    def write_report(self, filename):
        """Write energy report to file"""
        self._update_battery()
        
        try:
            with open(filename, 'w') as f:
                f.write(f"Energy Monitor Report\n")
                f.write(f"Machine: {self.machine.GetName()}\n")
                f.write(f"Timestamp: {datetime.now().isoformat()}\n")
                f.write(f"\n")
                
                f.write(f"Battery Status:\n")
                f.write(f"  Current: {self.get_battery_percent():.1f}%\n")
                f.write(f"  Remaining: {self.current_battery:.2f} mAh\n")
                f.write(f"  Capacity: {self.battery_capacity} mAh\n")
                f.write(f"  Total Consumed: {self.total_energy_consumed:.2f} mAh\n")
                f.write(f"\n")
                
                f.write(f"Power State Breakdown:\n")
                for state, consumed in sorted(self.stats.items(), key=lambda x: x[1], reverse=True):
                    if consumed > 0:
                        percent = (consumed / self.total_energy_consumed * 100) if self.total_energy_consumed > 0 else 0
                        f.write(f"  {state:20s}: {consumed:8.3f} mAh ({percent:5.1f}%)\n")
                
                f.write(f"\nState Transitions:\n")
                for trans in self.state_transitions:
                    f.write(f"  {trans['time'].isoformat()} {trans['from']:15s} → {trans['to']:15s} "
                           f"({trans['energy_mah']:.3f} mAh)\n")
            
            self.log(f"Report written to {filename}")
        except Exception as e:
            self.log(f"Error writing report: {e}", "ERROR")


# Global monitor instances
_monitors = {}

def get_monitor(machine):
    """Get or create energy monitor for a machine"""
    machine_name = machine.GetName()
    if machine_name not in _monitors:
        _monitors[machine_name] = EnergyMonitor(machine)
    return _monitors[machine_name]

def set_state(machine, state):
    """Set power state for a machine"""
    monitor = get_monitor(machine)
    return monitor.set_state(state)

def get_battery_percent(machine):
    """Get battery percentage for a machine"""
    monitor = get_monitor(machine)
    return monitor.get_battery_percent()

def set_rssi(machine, rssi_dbm):
    """Set RSSI for a machine"""
    monitor = get_monitor(machine)
    monitor.set_rssi(rssi_dbm)

def print_all_status():
    """Print status of all monitors"""
    for machine_name, monitor in _monitors.items():
        monitor.print_status()

def write_all_reports(output_dir="energy_reports"):
    """Write reports for all monitors"""
    import os
    os.makedirs(output_dir, exist_ok=True)
    
    for machine_name, monitor in _monitors.items():
        filename = os.path.join(output_dir, f"{machine_name}_energy.txt")
        monitor.write_report(filename)
