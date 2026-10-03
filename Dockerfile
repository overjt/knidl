FROM debian:12-slim

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        binutils-arm-none-eabi \
        ca-certificates \
        git \
        make \
        perl \
        python3 \
        python3-pip \
    && rm -rf /var/lib/apt/lists/*

# asm-differ runtime dependencies.
RUN pip3 install --break-system-packages \
        colorama \
        watchdog \
        levenshtein \
        cxxfilt

# decomp-permuter runtime dependencies (tools/decomp-permuter).
# toml is required (settings files); Levenshtein enables --algorithm
# levenshtein; pynacl/docker are only needed for permuter@home (-J) and are
# intentionally omitted.
RUN pip3 install --break-system-packages \
        toml

# agbcc: zhade's new_newlib_pret fork (the compiler needed to match the
# sibling KATAM decompilation), pinned for reproducibility.  The commit check
# makes a wrong or empty pin fail the build: the ARG was once spelled
# AGBC_COMMIT, so `checkout ${AGBCC_COMMIT}` checked out nothing and the image
# silently built the fork's default branch head.
ARG AGBCC_COMMIT=59b966ed1b8f371856dcf99f1546c2fe89c678ca
RUN git clone https://github.com/jiangzhengwenjz/agbcc /tmp/agbcc \
    && git -C /tmp/agbcc checkout ${AGBCC_COMMIT} \
    && test "$(git -C /tmp/agbcc rev-parse HEAD)" = "${AGBCC_COMMIT}" \
    && cd /tmp/agbcc && ./build.sh \
    && mkdir -p /opt/agbcc/bin \
    && mv agbcc old_agbcc agbcc_arm libc.a libgcc.a /opt/agbcc/bin/ \
    && rm -rf /tmp/agbcc

ENV PATH=/opt/agbcc/bin:$PATH
WORKDIR /src
