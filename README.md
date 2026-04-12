# Guardian: Chainsaw & Biodiversity Detection System

Guardian is an advanced, low-power Wireless Sensor Network (WSN) designed for the autonomous detection and localization of chainsaws and biodiversity sounds in dense forest environments.

## 🌲 Overview

The system leverages ESP32-S3 nodes equipped with acoustic sensors and LoRa mesh networking to monitor vast areas with minimal power consumption. It protects forests by providing real-time alerts to rangers, complete with precise geographic coordinates of the detected sounds.

### Key Features
- **Edge AI Detection**: Uses High-performance C++ (ESP-IDF) and Edge Impulse for real-time acoustic classification.
- **3-Stage Power Cascade**: Optimizes battery life through a tiered monitoring approach (Quiet → Transition → Active).
- **LoRa Mesh Networking**: Reliable, multi-hop communication for deep-forest coverage.
- **Geospatial Propagation**: Self-localizing mesh that propagates coordinates from a single gateway to all leaf nodes without individual GPS modules.
- **Acoustic Localization**: Triangulates sound sources (e.g., chainsaws) by correlating detections across multiple nodes.
- **Real-Time Dashboard**: A Supabase-powered web application for monitoring node health and mapping localized events.
- **Instant Alerts**: Telegram notifications including Google Maps links for immediate response.

---

## 🏗️ System Architecture

The project is divided into three main tiers:

### 1. Firmware (`src/esp32-s3-cpp`)
Native C++ implementation using the ESP-IDF framework.
- **`localization_module`**: Handles iterative multi-lateration for node positioning.
- **`inference_engine`**: Manages the TFLite Micro model for acoustic classification.
- **`esp_lora_driver`**: Custom driver for SX126x/SX127x LoRa chips.

### 2. Backend & Cloud (`src/web-manager/supabase`)
Serverless architecture using Supabase.
- **Edge Functions**: Handle telemetry ingestion, multi-node correlation, and alert routing.
- **Postgres Database**: Stores node health, detection logs, and localized acoustic events.

### 3. Web Dashboard (`src/web-manager/frontend`)
React-based monitoring portal.
- **Interactive Map**: Built with React-Leaflet, showing real-time node locations and pulsing detection highlights.
- **Analytics**: Historical data visualization for biodiversity trends.

---

## 🚀 Getting Started

### Prerequisites
- ESP-IDF v5.x
- Node.js & NPM
- Supabase CLI

### Installation
1. **Clone the repository**:
   ```bash
   git clone https://github.com/RobsonRojas/chainsaw-detector.git
   ```

2. **Configure Environment**:
   Copy `.env.example` to `.env` and fill in your Supabase and Telegram tokens.

3. **Deploy Backend**:
   ```bash
   cd src/web-manager/supabase
   supabase start
   supabase functions deploy ingest-mesh-data
   ```

4. **Flash Firmware**:
   ```bash
   cd src/esp32-s3-cpp
   idf.py build
   idf.py flash
   ```

---

## 📚 Documentation

Detailed technical design and architectural specifications can be found in the [docs/architecture](docs/architecture/README.md) directory:
- [High Level Overview](docs/architecture/high_level.md)
- [Technical Details (Mermaid Diagrams)](docs/architecture/technical_details.md)
- [Firmware Components](docs/architecture/firmware_components.md)

---

## 📜 License
This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.
