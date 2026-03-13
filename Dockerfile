FROM ubuntu:24.04

# Instalação de dependências: Clang 19, LLD, Make, Git, Python
RUN apt-get update && apt-get install -y \
    curl clang-19 lld-19 cmake git build-essential \
    crossbuild-essential-arm64 qemu-user-static

# Instalação do Rust
# Instalação do Rust e adição do target em uma única camada
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y --default-toolchain stable && \
    . "$HOME/.cargo/env" && \
    rustup target add aarch64-unknown-linux-gnu

# Garante que o PATH esteja disponível para as próximas camadas e para o container rodando
ENV PATH="/root/.cargo/bin:${PATH}"

