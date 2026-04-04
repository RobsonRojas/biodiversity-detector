import os

def inject_audio(machine, wav_path):
    print(f"[Audio Ingest] Preparing to inject {wav_path} into {machine.Name} audio DMA block...")
    
    if not os.path.exists(wav_path):
        print(f"[Audio Ingest] ERROR: File {wav_path} not found!")
        return
        
    try:
        with open(wav_path, "rb") as f:
            wav_data = f.read()
            
        # Write wav data to the mocked I2S memory region
        # 0x3F203000 is our audio_mock base
        base_addr = 0x3F203000
        length = min(len(wav_data), 0x1000) # up to 4KB for mock trigger
        
        # In a real environment, we'd trigger an interrupt here.
        # We just write it directly to sysbus
        for i in range(length):
            machine.SystemBus.WriteByte(base_addr + i, wav_data[i])
            
        print(f"[Audio Ingest] Successfully injected {length} bytes. Firmware should wake up and process.")
    except Exception as e:
        print(f"[Audio Ingest] Failure: {e}")
