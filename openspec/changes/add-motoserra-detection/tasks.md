## 1. Environment Setup

- [x] 1.1 Configure `tflite-micro` dependency in the project.
- [x] 1.2 Import the chainsaw detection model (`.tflite`) into the assets directory.

## 2. Audio Ingestion

- [x] 2.1 Implement the `AudioIn` class to read from `/dev/forest_audio`.
- [x] 2.2 Implement a 16,000-sample circular buffer for real-time audio capture.

## 3. AI Detection Engine

- [x] 3.1 Initialize the TFLite Micro interpreter with INT8 quantization settings.
- [x] 3.2 Implement the `detect_motoserra` function to process 1s audio frames.
- [x] 3.3 Add logic to check for the 0.85 confidence threshold.

## 4. Integration

- [x] 4.1 Connect the audio ingestion loop to the AI detection engine.
- [x] 4.2 Implement the duty cycle logic (5s active, 55s sleep) as per `config.yaml`.
