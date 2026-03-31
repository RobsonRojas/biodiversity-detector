## Why

The current system is specialized in detecting chainsaws (motoserras). Expanding its detection capabilities to animal and forest sounds (fauna and flora) significantly increases its value for biodiversity monitoring and environmental conservation. Additionally, providing the ability to retransmit the detected audio via LoRaWAN allows for remote human verification and a more immersive monitoring experience via Telegram and the Web Manager.

## What Changes

- **Firmware (ESP32)**:
    - **Multi-class Detection**: Upgrade the `DetectionEngine` to use a new TFLite model capable of identifying multiple sound classes (chainsaw, birds, rain, wind, specific animals).
    - **Audio Retransmission**: Implement logic to capture, compress (likely Opus or simple PCM), and fragment audio buffers for transmission over the mesh network.
    - **Dynamic Configuration**: Support remote configuration to toggle between "detect-only" (standard alert) and "forward-audio" (retransmit sound clip).
- **Protocol (Mesh)**:
    - **Audio Data Packets**: Introduce a mechanism to handle multi-packet data transmission for audio clips, ensuring reliability or at least best-effort delivery of fragments.
- **Backend (Supabase/Database)**:
    - **Detection Schema**: Update `detections` table to include `sound_class` (e.g., 'chainsaw', 'bird', 'rain') and support for storing audio clip pointers/URLs.
    - **Node Configuration**: Update `nodes_config` to include `operation_mode` (detect_only, forward_audio).
- **Frontend (Web Manager)**:
    - **Enhanced Detection View**: Update the dashboard to display the sound class and provide an audio player for retransmitted clips.
- **Integration**:
    - **Telegram Bot**: Enhance the bot to send audio files when "forward-audio" mode is active.

## Capabilities

### New Capabilities
- `fauna-flora-classification`: Implementation of the multi-class TFLite model and inference logic on ESP32.
- `mesh-audio-streaming`: Protocol extensions and firmware logic for fragmenting and sending audio data over LoRaWAN mesh.
- `audio-presentation-layer`: Web-manager and Telegram bot updates to play back and manage audio detections.

### Modified Capabilities
- `network-topology-management`: Update configuration management to support sound-specific parameters and operational modes.

## Impact

- **Power Consumption**: Audio retransmission will significantly increase airtime and power usage on Leaf nodes; this must be managed via configuration.
- **Bandwidth**: LoRaWAN/Mesh bandwidth is limited; audio transmission will be low-bitrate and potentially slow.
- **Storage**: The backend will now need to handle binary audio data (via Supabase Storage).
