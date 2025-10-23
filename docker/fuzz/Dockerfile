# MetaGraph Fuzzing Rig
#
# This image packs the Clang + libFuzzer toolchain, CMake, and Ninja so we can
# build and run fuzz targets in a controlled environment. It mirrors the
# GitHub Actions nightly fuzz job to keep behaviour consistent.

FROM ubuntu:22.04@sha256:0bced47fffa3361afa981854fcabcd4577cd43cebbb808cea2b1f33a3dd7f508

ARG LLVM_VERSION=18

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

# Base packages
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    curl \
    git \
    gnupg \
    lsb-release \
    software-properties-common \
    wget \
    build-essential \
    python3 \
    python3-pip \
    python3-venv \
    unzip \
    jq \
    ninja-build \
    && rm -rf /var/lib/apt/lists/*

# Install CMake from the Kitware APT repository to pick up modern releases
RUN wget -qO- https://apt.kitware.com/keys/kitware-archive-latest.asc | gpg --dearmor -o /usr/share/keyrings/kitware-archive-keyring.gpg && \
    echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main" > /etc/apt/sources.list.d/kitware.list && \
    apt-get update && apt-get install -y --no-install-recommends cmake && \
    rm -rf /var/lib/apt/lists/*

# Install Clang/LLVM toolchain plus libFuzzer runtime
RUN wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
    echo "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-${LLVM_VERSION} main" > /etc/apt/sources.list.d/llvm.list && \
    apt-get update && apt-get install -y --no-install-recommends \
    clang-${LLVM_VERSION} \
    clang-tools-${LLVM_VERSION} \
    clang-tidy-${LLVM_VERSION} \
    clang-format-${LLVM_VERSION} \
    lld-${LLVM_VERSION} \
    llvm-${LLVM_VERSION} \
    llvm-${LLVM_VERSION}-tools \
    llvm-${LLVM_VERSION}-runtime \
    llvm-${LLVM_VERSION}-dev \
    libclang-rt-${LLVM_VERSION}-dev \
    libfuzzer-${LLVM_VERSION}-dev \
    && rm -rf /var/lib/apt/lists/*

RUN ln -sf /usr/bin/clang-${LLVM_VERSION} /usr/bin/clang && \
    ln -sf /usr/bin/clang-${LLVM_VERSION} /usr/bin/clang++ && \
    ln -sf /usr/bin/clang-tidy-${LLVM_VERSION} /usr/bin/clang-tidy && \
    ln -sf /usr/bin/clang-format-${LLVM_VERSION} /usr/bin/clang-format && \
    ln -sf /usr/bin/lld-${LLVM_VERSION} /usr/bin/lld

# Add a non-root user for fuzzing runs
RUN groupadd -g 1000 fuzzer && \
    useradd -m -u 1000 -g fuzzer fuzzer && \
    install -d -o fuzzer -g fuzzer /home/fuzzer/corpus

# Default compiler environment
ENV CC=clang-${LLVM_VERSION}
ENV CXX=clang++-${LLVM_VERSION}
ENV AR=llvm-ar
ENV NM=llvm-nm
ENV RANLIB=llvm-ranlib
ENV LD=lld
ENV OBJCOPY=llvm-objcopy
ENV OBJDUMP=llvm-objdump
ENV STRIP=llvm-strip

# Minimal Python tooling for scripts and corpus helpers
RUN python3 -m pip install --no-cache-dir \
    lit==18.1.8 \
    pytest==8.0.0

WORKDIR /workspace

COPY docker/fuzz/entrypoint.sh /usr/local/bin/fuzz-entrypoint.sh
RUN chmod +x /usr/local/bin/fuzz-entrypoint.sh

USER fuzzer

ENTRYPOINT ["/usr/local/bin/fuzz-entrypoint.sh"]
