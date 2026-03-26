# --- Stage 1: Build C++ and RPi Rust ---
FROM ubuntu:24.04 AS builder-rpi

# Configuração de repositórios multi-arch
RUN dpkg --add-architecture arm64 && \
    apt-get update && apt-get install -y \
    curl cmake git build-essential \
    crossbuild-essential-arm64 g++-aarch64-linux-gnu qemu-user-static \
    linux-headers-generic:arm64 \
    libcurl4-openssl-dev:arm64

# Instalação do Rust para RPi
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y --default-toolchain stable && \
    . "$HOME/.cargo/env" && \
    rustup target add aarch64-unknown-linux-gnu

ENV PATH="/root/.cargo/bin:${PATH}"
WORKDIR /app
COPY . .

# Compilar o projeto C++ (GuardianEdgeAI)
RUN rm -rf build && mkdir build && cd build && \
    cmake -DCMAKE_SYSTEM_NAME=Linux \
          -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
          -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
          -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
          .. && \
    make -j$(nproc)

# --- Stage 2: Build ESP32 Rust ---
FROM ubuntu:24.04 AS builder-esp32

RUN apt-get update && apt-get install -y \
    curl git python3 python3-pip python3-venv \
    libusb-1.0-0-dev libssl-dev pkg-config

# Instalação do Rust e ferramenta espup
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y --default-toolchain stable
ENV PATH="/root/.cargo/bin:${PATH}"

# Setup toolchain ESP32 (simulado para build em contêiner)
RUN cargo install ldproxy espup cargo-espflash && \
    espup install --targets esp32s3

WORKDIR /app
COPY src/esp32/ src/esp32/
WORKDIR /app/src/esp32
# Nota: Build real requer ambiente espup source
RUN /bin/bash -c "source $HOME/export-esp.sh && cargo build --release"

# --- Stage 3: Build Web Frontend (Vite) ---
FROM node:20 AS builder-web-frontend
WORKDIR /app/frontend
COPY src/web-manager/frontend/package*.json ./
RUN npm install
COPY src/web-manager/frontend/ .
RUN npm run build

# --- Stage 4: Final Image ---
FROM ubuntu:24.04
WORKDIR /app

# Dependências de runtime para o app C++
RUN apt-get update && apt-get install -y \
    libcurl4 qemu-user-static \
    && rm -rf /var/lib/apt/lists/*

# Copia os binários do RPi
COPY --from=builder-rpi /app/build/guardian_node /app/guardian_node
# Copia o firmware do ESP32 como artefato
COPY --from=builder-esp32 /app/src/esp32/target/xtensa-esp32s3-espidf/release/esp32 /app/dist/esp32.bin

# Copia o Web Manager (Static Frontend - Vite dist)
COPY --from=builder-web-frontend /app/frontend/dist /app/web/frontend

EXPOSE 80
CMD ["./guardian_node"]

