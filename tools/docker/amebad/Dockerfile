# Inherit from the common base image
FROM ameba-rtos-aws:common

# Redefine following build arguments to respective repo and tag/branch
ARG AMEBA_AWS_REPO=https://github.com/mikaelajiwidodo/ameba-amazon-freertos.git
ARG TAG_NAME=FreeRTOS-LTS-2026XX.xx_D_and_Z2_restructuring

# Define fixed build arguments
ARG AMBRTOS_REPO=https://github.com/Ameba-AIoT/ameba-rtos-d.git
ARG AMBRTOS_TAG_NAME=main
ARG AMEBA_AWS_DIR=/opt/ameba/ameba-rtos-d/component/common/application/amazon-freertos
ARG AMEBA_DIR=/opt/ameba

# The legacy AmebaD asdk-6.4.1 toolchain is 32-bit x86, so the 64-bit base image
# needs i386 multiarch + 32-bit runtime libs to run it.
RUN dpkg --add-architecture i386 \
    && apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -fy --no-install-recommends \
    libc6:i386 \
    libstdc++6:i386 \
    zlib1g:i386 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Clone the repo
RUN git clone --branch ${AMBRTOS_TAG_NAME} --single-branch ${AMBRTOS_REPO} ameba-rtos-d \
    && git clone --recurse-submodules --branch ${TAG_NAME} --single-branch ${AMEBA_AWS_REPO} ${AMEBA_AWS_DIR}

# Setup Ameba Amazon FreeRTOS environment
RUN cd ${AMEBA_DIR}/ameba-rtos-d \
    && chmod u+x aws_setup.sh \
    && ./aws_setup.sh amebad \
    && cd project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp \
    && make -C asdk/ check_toolchain

# Set default working directory
WORKDIR ${AMEBA_DIR}/ameba-rtos-d

# Set default command to bash for interaction
CMD ["/bin/bash"]
