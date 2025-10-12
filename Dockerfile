FROM debian:bookworm-slim

ARG DEBIAN_FRONTEND=noninteractive
ARG UID=1000
ARG GID=1000

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential gcc g++ cmake ninja-build git ccache libsqlite3-dev pkg-config \
    && rm -rf /var/lib/apt/lists/*


ENV CCACHE_DIR=/home/dev/.ccache
ENV PATH="/usr/lib/ccache:${PATH}"

RUN groupadd -g ${GID} dev && useradd -m -u ${UID} -g ${GID} dev
USER dev
WORKDIR /work

# Domyślnie GCC
ENV CC=gcc CXX=g++

CMD ["/bin/bash"]
