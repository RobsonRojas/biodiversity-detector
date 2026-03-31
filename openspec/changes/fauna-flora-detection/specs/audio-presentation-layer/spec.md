## ADDED Requirements

### Requirement: web-audio-player
The web manager dashboard SHALL provide an embedded audio player for detections that have an associated audio clip.

#### Scenario: play-detection-audio
- **WHEN** a user clicks the "Play" icon on a fauna detection row
- **THEN** the system MUST stream the audio from Supabase Storage

### Requirement: telegram-audio-alert
The Telegram bot SHALL send a voice message or audio file when a high-confidence detection occurs in 'forward-audio' mode.

#### Scenario: telegram-alert-with-sound
- **WHEN** a bird detection is received by the backend in 'forward-audio' mode
- **THEN** the bot MUST send a notification message followed by the audio clip
