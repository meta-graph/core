FROM ubuntu:24.04

ARG LLVM_VERSION=18
ENV DEBIAN_FRONTEND=noninteractive \
    LLVM_VERSION=${LLVM_VERSION}

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    wget \
    gnupg \
    software-properties-common \
    lsb-release \
    build-essential \
    ninja-build \
    cmake \
    git \
    python3 \
    python3-pip \
    python3-setuptools \
    python3-wheel \
    valgrind \
    pkg-config \
    file \
    binutils \
    curl \
    unzip \
    && rm -rf /var/lib/apt/lists/*

RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh ${LLVM_VERSION} all && \
    rm llvm.sh

RUN ln -sf /usr/bin/clang-${LLVM_VERSION} /usr/bin/clang && \
    ln -sf /usr/bin/clang++-${LLVM_VERSION} /usr/bin/clang++ && \
    ln -sf /usr/bin/clang-tidy-${LLVM_VERSION} /usr/bin/clang-tidy && \
    ln -sf /usr/bin/clang-format-${LLVM_VERSION} /usr/bin/clang-format

RUN pip3 install --no-cache-dir semgrep

RUN useradd --create-home --shell /bin/bash metagraph && \
    mkdir -p /workspace && chown -R metagraph:metagraph /workspace

USER metagraph

WORKDIR /workspace
CMD ["bash"]
