# Use the latest Ubuntu LTS version for stability
FROM ubuntu:24.04

# Install common dependencies and clean up to reduce image size
RUN set -x \
    && apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -fy --no-install-recommends \
    git \
    gcc \
    python3-venv \
    python3-dev \
    python3-pip \
    wget \
    bzip2 \
    cmake \
    libgirepository1.0-dev \
    libcairo2-dev \
    libreadline-dev \
    libssl-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set python3 as the default python version, and then check the version
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 1 \
    && python --version \
    && python3 --version

# Link `sh` to `bash`
RUN ln -sf /bin/bash /bin/sh

# Define build arguments for repositories
ARG AMEBA_DIR=/opt/ameba

# Create the necessary directory, clone the connectedhomeip repo
RUN mkdir -p ${AMEBA_DIR} \
    && cd ${AMEBA_DIR}

# Set the working directory for the rest of the Dockerfile to work
WORKDIR ${AMEBA_DIR}

# Set default command to bash for interaction
CMD ["/bin/bash"]
