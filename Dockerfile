FROM ubuntu:22.04

# Non-interactive apt
ENV DEBIAN_FRONTEND=noninteractive

# System dependencies and ARM cross toolchain
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    python3 \
    pkg-config \
    libpng-dev \
    zlib1g-dev \
    gcc-arm-none-eabi \
    binutils-arm-none-eabi \
    libnewlib-arm-none-eabi \
    perl \
    ca-certificates \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

# Copy source into image
COPY . /workspace

# Build the project (tools + assets + ROM)
# - Bypass git history check for container builds
# - Rebuild all tools inside the container to ensure correct libstdc++
# - Then build the ROM
RUN set -eux; \
    touch .histignore; \
    make clean-tools; \
    make tools; \
    make -j"$(nproc)" NO_MULTIBOOT=1
