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
    && rm -rf /var/lib/apt/lists/*

# agbcc: zhade's new_newlib_pret fork (the compiler needed to match the
# sibling KATAM decompilation), pinned for reproducibility.
ARG AGBC_COMMIT=59b966ed1b8f371856dcf99f1546c2fe89c678ca
RUN git clone https://github.com/jiangzhengwenjz/agbcc /tmp/agbcc \
    && git -C /tmp/agbcc checkout ${AGBCC_COMMIT} \
    && cd /tmp/agbcc && ./build.sh \
    && mkdir -p /opt/agbcc/bin \
    && mv agbcc old_agbcc agbcc_arm libc.a libgcc.a /opt/agbcc/bin/ \
    && rm -rf /tmp/agbcc

ENV PATH=/opt/agbcc/bin:$PATH
WORKDIR /src
