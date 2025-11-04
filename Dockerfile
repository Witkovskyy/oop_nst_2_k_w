FROM debian:bookworm-slim

ARG DEBIAN_FRONTEND=noninteractive
ARG UID=1000
ARG GID=1000

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential gcc g++ cmake ninja-build git ccache pkg-config \
    libsqlite3-dev \
    clang clang-tidy gdb valgrind lcov gcovr \
    catch2 \
    nlohmann-json3-dev \
    # --- SFML deps ---
    libx11-dev libxext-dev libxrandr-dev libxinerama-dev libxcursor-dev \
    libgl1-mesa-dev \
    libudev-dev \
    libpulse-dev \
    libopenal-dev \
    libogg-dev libvorbis-dev libflac-dev libsndfile1-dev \
    libfreetype6-dev libpng-dev libjpeg-dev \
    wget ca-certificates \
 && rm -rf /var/lib/apt/lists/*

# Build and install SFML 2.6.2 from source (EXACT version)
RUN set -eux; \
    cd /tmp; \
    wget -O sfml-2.6.2.tar.gz https://github.com/SFML/SFML/archive/refs/tags/2.6.2.tar.gz; \
    tar xzf sfml-2.6.2.tar.gz; \
    mkdir -p sfml-build && cd sfml-build; \
    cmake ../SFML-2.6.2 \
      -DCMAKE_BUILD_TYPE=Release \
      -DSFML_BUILD_EXAMPLES=OFF \
      -DSFML_BUILD_TESTS=OFF \
      -DSFML_BUILD_DOC=OFF \
      -DCMAKE_INSTALL_PREFIX=/usr/local; \
    cmake --build . -- -j"$(nproc)"; \
    cmake --install . --strip; \
    rm -rf /tmp/sfml-* /tmp/SFML-*; \
    ldconfig || true

# ccache
ENV PATH="/usr/lib/ccache:${PATH}"
ENV CCACHE_DIR=/tmp/.ccache

# user
RUN groupadd -g ${GID} dev && useradd -m -u ${UID} -g ${GID} dev
USER dev
WORKDIR /work

# native toolchain
ENV CC=gcc CXX=g++

CMD ["/bin/bash"]
