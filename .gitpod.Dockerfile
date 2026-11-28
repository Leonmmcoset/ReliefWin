FROM gitpod/workspace-full-vnc

USER gitpod

# Install custom tools, runtime, etc. using apt-get
# For example, the command below would install "bastet" - a command line tetris clone:
#
# RUN sudo apt-get -q update && #     sudo apt-get install -yq bastet && #     sudo rm -rf /var/lib/apt/lists/*
#
# More information: https://www.gitpod.io/docs/config-docker/
RUN sudo apt-get -q update && \
    sudo apt-get -yq upgrade && \
    sudo apt-get -yq install qemu-system-x86 qemu-utils gdb-mingw-w64 && \
    sudo rm -rf /var/lib/apt/lists/*

RUN sudo apt-get -q update && \
    sudo apt-get -yq install build-essential git ninja-build flex bison m4 ccache curl wget xz-utils tar patch pkg-config python3 python3-venv python3-mako python3-yaml python3-packaging glslang-tools msitools && \
    sudo rm -rf /var/lib/apt/lists/*

RUN python3 -m venv --system-site-packages /home/gitpod/.local/opt/reliefwin-build && \
    /home/gitpod/.local/opt/reliefwin-build/bin/python3 -m pip install --no-cache-dir cmake==3.31.10

ENV PATH="/home/gitpod/.local/opt/reliefwin-build/bin:${PATH}"

RUN curl -fsSL https://raw.githubusercontent.com/LiberNT/libernt-build-toolchain/main/rosbe-unix-bootstrap.sh -o /tmp/rosbe-bootstrap.sh && \
    sh /tmp/rosbe-bootstrap.sh && \
    rm -f /tmp/rosbe-bootstrap.sh
