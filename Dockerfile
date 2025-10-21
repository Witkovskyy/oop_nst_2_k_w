FROM debian:bookworm-slim

ARG DEBIAN_FRONTEND=noninteractive
ARG UID=1000
ARG GID=1000

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential gcc g++ cmake ninja-build git ccache pkg-config \
    libsqlite3-dev \
    # narzędzia pomocnicze (opcjonalnie – polecam na projekt/CI)
    clang clang-tidy gdb valgrind lcov gcovr \
    catch2 \
    nlohmann-json3-dev \
    # jeśli będziesz używać Postgresa:
    # libpq-dev libpqxx-dev \
 && rm -rf /var/lib/apt/lists/*

# ccache
ENV PATH="/usr/lib/ccache:${PATH}"
ENV CCACHE_DIR=/tmp/.ccache

# user
RUN groupadd -g ${GID} dev && useradd -m -u ${UID} -g ${GID} dev
USER dev
WORKDIR /work

# native toolchain
ENV CC=gcc CXX=g++

# compose override command
CMD ["/bin/bash"]
