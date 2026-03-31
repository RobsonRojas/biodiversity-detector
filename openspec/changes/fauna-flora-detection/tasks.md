## 1. Firmware (ESP32)

- [x] 1.1 Update `DetectionEngine` to load `forest_sounds_v1.tflite`
- [x] 1.2 Implement Mu-law encoding in `audio_i2s.rs`
- [x] 1.3 Add audio-specific packet types to `mesh_protocol.rs`
- [x] 1.4 Implement fragment transmission loop in `main.rs`
- [x] 1.5 Support `operation_mode` configuration in firmware

## 2. Backend (Supabase)

- [x] 2.1 Add `sound_class` and `audio_url` to `detections` table
- [x] 2.2 Set up Supabase Storage bucket 'detection-audio'
- [x] 2.3 Update gateway processing logic to reassemble fragments (if applicable)

## 3. Frontend (Web Manager)

- [x] 3.1 Update `DetectionTable` UI for multiple sound classes
- [x] 3.2 Add audio playback support to the dashboard
- [x] 3.3 Add 'Operation Mode' toggle to node configuration

## 4. Integration & Notification

- [x] 4.1 Update Telegram bot to handle and send audio files
- [x] 4.2 End-to-end verification with simulated audio data
