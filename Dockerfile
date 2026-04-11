# --- Stage 1: Build C++ and RPi Rust ---
FROM ubuntu:24.04 AS builder-rpi

# Configuração de repositórios multi-arch
RUN dpkg --add-architecture arm64 && \
    sed -i 's/^Types: deb/Types: deb\nArchitectures: amd64/' /etc/apt/sources.list.d/ubuntu.sources && \
    echo "Types: deb\nURIs: http://ports.ubuntu.com/ubuntu-ports/\nSuites: noble noble-updates noble-security\nComponents: main universe restricted multiverse\nArchitectures: arm64" > /etc/apt/sources.list.d/arm64.sources && \
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

# --- Stage 2: Build ESP32 C++ (ESP-IDF) ---
FROM espressif/idf:v5.3 AS builder-esp-cpp
WORKDIR /build-area
COPY src/esp32-s3-cpp/ .
# Garante que resíduos locais não interfiram no build do container
RUN rm -rf build managed_components sdkconfig dependencies.lock
RUN . /opt/esp/idf/export.sh && idf.py build

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

# Configuração de repositórios multi-arch para dependências de runtime AARCH64
RUN dpkg --add-architecture arm64 && \
    sed -i 's/^Types: deb/Types: deb\nArchitectures: amd64/' /etc/apt/sources.list.d/ubuntu.sources && \
    echo "Types: deb\nURIs: http://ports.ubuntu.com/ubuntu-ports/\nSuites: noble noble-updates noble-security\nComponents: main universe restricted multiverse\nArchitectures: arm64" > /etc/apt/sources.list.d/arm64.sources && \
    apt-get update && apt-get install -y \
    qemu-user-static \
    libcurl4:arm64 \
    libstdc++6:arm64 \
    libgcc-s1:arm64 \
    && rm -rf /var/lib/apt/lists/*

# Configura o QEMU para encontrar as bibliotecas arm64
ENV QEMU_LD_PREFIX=/

# Copia os binários do RPi
COPY --from=builder-rpi /app/build/guardian_node /app/guardian_node
# Copia o firmware do ESP32 como artefato
COPY --from=builder-esp-cpp /build-area/build/esp32-s3-cpp-firmware.bin /app/dist/esp32.bin

# Copia o Web Manager (Static Frontend - Vite dist)
COPY --from=builder-web-frontend /app/frontend/dist /app/web/frontend

EXPOSE 80
CMD ["/usr/bin/qemu-aarch64-static", "-L", "/", "/app/guardian_node"]

