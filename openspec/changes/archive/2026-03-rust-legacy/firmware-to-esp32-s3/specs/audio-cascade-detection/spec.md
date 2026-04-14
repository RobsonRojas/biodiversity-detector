## ADDED Requirements

### Requirement: Cascade Activation Logic
The system MUST implement a three-stage activation cascade to minimize power consumption: Stage 1 (RMS Volume) -> Stage 2 (DSP Frequency Match) -> Stage 3 (TinyML Inference).

#### Scenario: Volume Trigger
- **WHEN** the ambient sound volume (RMS) exceeds a configurable threshold (e.g., -40dBFS).
- **THEN** the system must transition from Low Power Sleep to DSP Processing mode within 10ms.

#### Scenario: DSP Frequency Pattern Match
- **WHEN** the I2S audio stream contains dominant frequencies between 100Hz and 2kHz (typical of chainsaw motors) for a duration of at least 500ms.
- **THEN** the system must trigger a full TinyML inference cycle.

#### Scenario: False Alarm Rejection
- **WHEN** the DSP stage identifies a sudden impulse (like a branch breaking) but no sustained harmonic pattern.
- **THEN** the system must immediately return to Low Power Deep Sleep without running AI inference.
