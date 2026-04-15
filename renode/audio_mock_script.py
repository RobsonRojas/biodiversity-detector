import os
import struct
import numpy as np
from datetime import datetime

class AudioMockInjector:
    """Simulates audio injection into the ESP32-S3 I2S interface"""
    
    # Memory regions
    I2S_FIFO_ADDR = 0x3f200000
    I2S_FIFO_SIZE = 0x10000  # 64KB
    AUDIO_MOCK_ADDR = 0x3F203000
    AUDIO_MOCK_SIZE = 0x1000  # 4KB
    
    # WAV constants
    SAMPLE_RATE = 16000  # 16 kHz (typical for PDM mics)
    CHANNELS = 1
    SAMPLE_WIDTH = 2  # 16-bit samples
    
    def __init__(self, machine, log_prefix="[Audio Ingest]"):
        self.machine = machine
        self.log_prefix = log_prefix
        self.injection_count = 0
        self.last_injection_time = None
    
    def log(self, message, level="INFO"):
        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        print(f"{self.log_prefix} [{timestamp}] {level}: {message}")
    
    def read_wav_file(self, wav_path):
        """Read WAV file and extract PCM data"""
        if not os.path.exists(wav_path):
            self.log(f"ERROR: File not found: {wav_path}", "ERROR")
            return None
        
        try:
            with open(wav_path, "rb") as f:
                wav_data = f.read()
            
            # Simple WAV parsing (skip header, extract PCM data)
            # WAV format: 44-byte header + PCM data
            if len(wav_data) < 44:
                self.log(f"ERROR: Invalid WAV file (too small)", "ERROR")
                return None
            
            # Extract sample rate from header (bytes 24-27)
            sample_rate = struct.unpack('<I', wav_data[24:28])[0]
            
            # Extract bit depth from header (bytes 34-35)
            bit_depth = struct.unpack('<H', wav_data[34:36])[0]
            
            # PCM data starts after header
            pcm_data = wav_data[44:]
            
            self.log(f"Loaded WAV: {os.path.basename(wav_path)} "
                    f"({sample_rate}Hz, {bit_depth}bit, {len(pcm_data)} bytes)")
            
            return {
                'raw': pcm_data,
                'sample_rate': sample_rate,
                'bit_depth': bit_depth,
                'duration_ms': (len(pcm_data) * 1000) // (sample_rate * bit_depth // 8)
            }
        except Exception as e:
            self.log(f"ERROR reading WAV: {e}", "ERROR")
            return None
    
    def inject_audio(self, wav_path, duration_ms=1000):
        """Inject audio data into simulated I2S memory"""
        self.log(f"Starting injection from {wav_path}...")
        
        wav_info = self.read_wav_file(wav_path)
        if not wav_info:
            return False
        
        try:
            # Limit injection size to available memory
            max_bytes = min(self.I2S_FIFO_SIZE, len(wav_info['raw']))
            injection_data = wav_info['raw'][:max_bytes]
            
            # Write to I2S FIFO
            self.log(f"Writing {len(injection_data)} bytes to I2S FIFO at {hex(self.I2S_FIFO_ADDR)}...")
            
            for i in range(0, len(injection_data), 4):
                chunk = injection_data[i:i+4]
                # Pad chunk if needed
                chunk = chunk + b'\x00' * (4 - len(chunk))
                value = struct.unpack('<I', chunk)[0]
                self.machine.SystemBus.WriteDoubleWord(self.I2S_FIFO_ADDR + i, value)
            
            # Also write metadata to audio_mock region
            self._write_metadata(len(injection_data), wav_info['sample_rate'])
            
            self.injection_count += 1
            self.last_injection_time = datetime.now()
            
            self.log(f"✓ Injection #{self.injection_count} complete "
                    f"({len(injection_data)} bytes, {wav_info['duration_ms']}ms audio)")
            
            return True
        
        except Exception as e:
            self.log(f"ERROR during injection: {e}", "ERROR")
            return False
    
    def _write_metadata(self, size, sample_rate):
        """Write audio metadata to mock region for firmware to read"""
        try:
            # Metadata format:
            # Offset 0: Audio size (4 bytes, little-endian)
            # Offset 4: Sample rate (4 bytes, little-endian)
            # Offset 8: Injection timestamp (4 bytes)
            
            self.machine.SystemBus.WriteDoubleWord(self.AUDIO_MOCK_ADDR, size)
            self.machine.SystemBus.WriteDoubleWord(self.AUDIO_MOCK_ADDR + 4, sample_rate)
            
            import time
            ts = int(time.time()) & 0xFFFFFFFF
            self.machine.SystemBus.WriteDoubleWord(self.AUDIO_MOCK_ADDR + 8, ts)
            
            self.log(f"Metadata written: size={size}, sample_rate={sample_rate}")
        except Exception as e:
            self.log(f"WARNING: Could not write metadata: {e}", "WARN")
    
    def generate_synthetic_audio(self, frequency=440, duration_ms=1000, amplitude=0.5):
        """Generate synthetic sine wave audio for testing"""
        self.log(f"Generating synthetic audio: {frequency}Hz, {duration_ms}ms, amplitude={amplitude}...")
        
        num_samples = (self.SAMPLE_RATE * duration_ms) // 1000
        t = np.linspace(0, duration_ms/1000.0, num_samples, False)
        
        # Generate sine wave
        signal = np.sin(2 * np.pi * frequency * t) * amplitude
        
        # Convert to 16-bit PCM
        signal_int = np.int16(signal * 32767)
        pcm_data = signal_int.tobytes()
        
        self.log(f"Generated {len(pcm_data)} bytes of synthetic audio")
        return pcm_data
    
    def inject_synthetic_audio(self, frequency=440, duration_ms=1000):
        """Inject synthetically generated audio"""
        self.log(f"Injecting synthetic {frequency}Hz tone ({duration_ms}ms)...")
        
        try:
            pcm_data = self.generate_synthetic_audio(frequency, duration_ms)
            
            # Limit to available memory
            max_bytes = min(self.I2S_FIFO_SIZE, len(pcm_data))
            injection_data = pcm_data[:max_bytes]
            
            # Write to I2S FIFO
            for i in range(0, len(injection_data), 4):
                chunk = injection_data[i:i+4]
                chunk = chunk + b'\x00' * (4 - len(chunk))
                value = struct.unpack('<I', chunk)[0]
                self.machine.SystemBus.WriteDoubleWord(self.I2S_FIFO_ADDR + i, value)
            
            self._write_metadata(len(injection_data), self.SAMPLE_RATE)
            
            self.injection_count += 1
            self.log(f"✓ Synthetic injection #{self.injection_count} complete")
            return True
        
        except Exception as e:
            self.log(f"ERROR generating synthetic audio: {e}", "ERROR")
            return False
    
    def print_status(self):
        """Print injector status"""
        print(f"\n{self.log_prefix} === Audio Injector Status ===")
        print(f"  Machine: {self.machine.GetName()}")
        print(f"  Total injections: {self.injection_count}")
        if self.last_injection_time:
            print(f"  Last injection: {self.last_injection_time.strftime('%H:%M:%S')}")
        print(f"  I2S FIFO: {hex(self.I2S_FIFO_ADDR)} ({self.I2S_FIFO_SIZE} bytes)")
        print(f"  Audio Mock: {hex(self.AUDIO_MOCK_ADDR)} ({self.AUDIO_MOCK_SIZE} bytes)")
        print()


# Global injector instances (one per machine)
_injectors = {}

def get_injector(machine):
    """Get or create an injector for a specific machine"""
    machine_name = machine.GetName()
    if machine_name not in _injectors:
        _injectors[machine_name] = AudioMockInjector(machine)
    return _injectors[machine_name]

def inject_audio(machine, wav_path):
    """Legacy interface for backward compatibility"""
    injector = get_injector(machine)
    return injector.inject_audio(wav_path)

def inject_synthetic(machine, frequency=440, duration_ms=1000):
    """Inject synthetic audio"""
    injector = get_injector(machine)
    return injector.inject_synthetic_audio(frequency, duration_ms)

def print_all_status():
    """Print status of all injectors"""
    for machine_name, injector in _injectors.items():
        injector.print_status()
