# The emulator image for `make boottest` (issue #36, docs/data.md section
# 8.4): mGBA's core library, built from a pinned release tag with every
# optional dependency and frontend off, and the boot-test harness
# (boottest.c) linked against it.  Kept apart from the knidl-builder image
# (the Dockerfile at the root), which stays the toolchain only.
#
# mGBA is MPL-2.0 (https://github.com/mgba-emu/mgba); its source is cloned
# here at build time and never vendored into the repository, and its LICENSE
# is shipped next to the binary.
#
#   docker build -t knidl-boottest tools/boottest

FROM debian:12-slim AS build

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        cmake \
        git \
    && rm -rf /var/lib/apt/lists/*

# The latest 0.10.x release; the commit check makes a moved tag fail.
ARG MGBA_TAG=0.10.5
ARG MGBA_COMMIT=26b7884bc25a5933960f3cdcd98bac1ae14d42e2
RUN git clone --depth 1 --branch ${MGBA_TAG} https://github.com/mgba-emu/mgba /tmp/mgba \
    && test "$(git -C /tmp/mgba rev-parse HEAD)" = "${MGBA_COMMIT}" \
    && cmake -S /tmp/mgba -B /tmp/mgba/build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/opt/mgba \
        -DBUILD_STATIC=ON -DBUILD_SHARED=OFF -DBUILD_LTO=OFF \
        -DBUILD_QT=OFF -DBUILD_SDL=OFF -DBUILD_LIBRETRO=OFF \
        -DBUILD_GL=OFF -DBUILD_GLES2=OFF -DBUILD_GLES3=OFF -DUSE_EPOXY=OFF \
        -DDISABLE_DEPS=ON -DUSE_FFMPEG=OFF -DUSE_ZLIB=OFF -DUSE_MINIZIP=OFF \
        -DUSE_PNG=OFF -DUSE_LIBZIP=OFF -DUSE_SQLITE3=OFF -DUSE_ELF=OFF \
        -DUSE_LZMA=OFF -DUSE_LUA=OFF -DENABLE_SCRIPTING=OFF \
        -DUSE_DEBUGGERS=OFF -DUSE_EDITLINE=OFF -DUSE_GDB_STUB=OFF \
        -DUSE_DISCORD_RPC=OFF -DM_CORE_GB=OFF \
    && cmake --build /tmp/mgba/build -j"$(nproc)" \
    && cmake --install /tmp/mgba/build \
    && cp /tmp/mgba/LICENSE /opt/mgba/LICENSE \
    && rm -rf /tmp/mgba

# The harness, in its own layer: editing it does not rebuild mGBA.
COPY boottest.c /tmp/boottest.c
RUN gcc -O2 -Wall -Wextra -Wno-unused-parameter -I/opt/mgba/include \
        -o /opt/mgba/knidl-boottest /tmp/boottest.c \
        /opt/mgba/lib/libmgba.a -lm -lpthread

FROM debian:12-slim

# make: `make boottest-run INSIDE_DOCKER=1` runs in this image.
RUN apt-get update \
    && apt-get install -y --no-install-recommends make \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /opt/mgba/knidl-boottest /usr/local/bin/knidl-boottest
COPY --from=build /opt/mgba/LICENSE /usr/share/doc/mgba/LICENSE
WORKDIR /src
