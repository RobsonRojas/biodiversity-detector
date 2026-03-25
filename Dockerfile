# Stage 1: Build C++ and Rust projects
FROM ubuntu:24.04 AS builder

# Configuração de repositórios multi-arch para permitir instalação de pacotes arm64 no host amd64
RUN dpkg --add-architecture arm64 && \
    rm /etc/apt/sources.list.d/ubuntu.sources && \
    echo "Types: deb\n\
URIs: http://archive.ubuntu.com/ubuntu/\n\
Suites: noble noble-updates noble-backports\n\
Components: main universe restricted multiverse\n\
Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg\n\
Architectures: amd64 i386" > /etc/apt/sources.list.d/amd64.sources && \
    echo "Types: deb\n\
URIs: http://ports.ubuntu.com/ubuntu-ports/\n\
Suites: noble noble-updates noble-backports\n\
Components: main universe restricted multiverse\n\
Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg\n\
Architectures: arm64" > /etc/apt/sources.list.d/arm64.sources

# Instalação de dependências: Clang, GCC Cross, Kernel Headers e Rust
RUN apt-get update && apt-get install -y \
    curl cmake git build-essential \
    crossbuild-essential-arm64 g++-aarch64-linux-gnu qemu-user-static \
    linux-headers-generic:arm64

# Instalação do Rust e adição do target em uma única camada
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y --default-toolchain stable && \
    . "$HOME/.cargo/env" && \
    rustup target add aarch64-unknown-linux-gnu

# Garante que o PATH esteja disponível
ENV PATH="/root/.cargo/bin:${PATH}"

# Define o diretório de trabalho
WORKDIR /app

# Copia todo o código fonte
COPY . .

# Compilar o projeto C++ (GuardianEdgeAI)
RUN rm -rf build && mkdir build && cd build && \
    cmake -DCMAKE_SYSTEM_NAME=Linux \
          -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
          -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
          -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
          .. && \
    make -j$(nproc)

# Compilar o módulo do kernel em Rust (Guardian Audio Module)
# O build real requer o kernel source do RPi inteiro configurado (feito no Dockerfile.kernel-test).
# Aqui na imagem principal do app, apenas simulamos a presença do .ko para o app C++.
RUN mkdir -p src/kernel && cd src/kernel && \
    echo "Simulando guardian_audio.ko para build do node" && \
    aarch64-linux-gnu-gcc -shared -o guardian_audio.ko /dev/null

# Stage 2: Final Image
FROM ubuntu:24.04
WORKDIR /app
# Copia especificamente os arquivos desejados para a imagem final
COPY --from=builder /app/build/guardian_node /app/guardian_node
COPY --from=builder /app/src/kernel/guardian_audio.ko /app/guardian_audio.ko

CMD ["./guardian_node"]

