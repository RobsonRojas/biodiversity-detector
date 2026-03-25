import numpy as np
import sys
import struct

def generate_motoserra_data(duration_sec=10, sample_rate=16000):
    """
    Generates a 32-bit PCM mono stream simulating a motoserra.
    A motoserra has a strong base frequency (engine) and high-frequency harmonics (chain).
    """
    t = np.linspace(0, duration_sec, int(sample_rate * duration_sec), endpoint=False)
    
    # Base engine frequency (100 Hz)
    engine = 0.4 * np.sin(2 * np.pi * 100 * t)
    
    # Harmonics and chain noise (400 Hz and high-freq noise)
    harmonics = 0.3 * np.sin(2 * np.pi * 400 * t)
    noise = 0.1 * np.random.normal(0, 1, len(t))
    
    # Combine signals
    signal = engine + harmonics + noise
    
    # Scale to 32-bit signed integer range
    signal = np.clip(signal, -1.0, 1.0)
    pcm_data = (signal * (2**31 - 1)).astype(np.int32)
    
    return pcm_data.tobytes()

def generate_background_data(duration_sec=10, sample_rate=16000):
    """
    Generates background noise (white noise).
    """
    num_samples = int(sample_rate * duration_sec)
    signal = 0.05 * np.random.normal(0, 1, num_samples)
    
    signal = np.clip(signal, -1.0, 1.0)
    pcm_data = (signal * (2**31 - 1)).astype(np.int32)
    
    return pcm_data.tobytes()

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python3 mock_sensor.py <type: motoserra|background> <output_file>")
        sys.exit(1)
        
    data_type = sys.argv[1]
    output_path = sys.argv[2]
    
    if data_type == "motoserra":
        data = generate_motoserra_data()
    else:
        data = generate_background_data()
        
    with open(output_path, "wb") as f:
        f.write(data)
        
    print(f"Generated {len(data)} bytes of {data_type} audio data to {output_path}")
