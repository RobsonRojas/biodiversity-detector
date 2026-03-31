## 1. Firmware (ESP32)

- [ ] 1.1 Update `DetectionEngine` to load `forest_sounds_v1.tflite`
- [ ] 1.2 Implement Mu-law encoding in `audio_i2s.rs`
- [ ] 1.3 Add audio-specific packet types to `mesh_protocol.rs`
- [ ] 1.4 Implement fragment transmission loop in `main.rs`
- [ ] 1.5 Support `operation_mode` configuration in firmware

## 2. Backend (Supabase)

- [ ] 2.1 Add `sound_class` and `audio_url` to `detections` table
- [ ] 2.2 Set up Supabase Storage bucket 'detection-audio'
- [ ] 2.3 Update gateway processing logic to reassemble fragments (if applicable)

## 3. Frontend (Web Manager)

- [ ] 3.1 Update `DetectionTable` UI for multiple sound classes
- [ ] 3.2 Add audio playback support to the dashboard
- [ ] 3.3 Add 'Operation Mode' toggle to node configuration

## 4. Integration & Notification

- [ ] 4.1 Update Telegram bot to handle and send audio files
- [ ] 4.2 End-to-end verification with simulated audio data
