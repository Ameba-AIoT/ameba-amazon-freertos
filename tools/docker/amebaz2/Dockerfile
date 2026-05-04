# Inherit from the common base image
FROM ameba-rtos-aws:common

# Redefine following build arguments to respective repo and tag/branch
ARG AMEBA_AWS_REPO=https://github.com/mikaelajiwidodo/ameba-amazon-freertos.git
ARG TAG_NAME=FreeRTOS-LTS-202406.xx_D_Z2_restructuring

# Define fixed build arguments
ARG AMBRTOS_REPO=https://github.com/Ameba-AIoT/ameba-rtos-z2.git
ARG AMBRTOS_TAG_NAME=main
ARG AMEBA_AWS_DIR=/opt/ameba/ameba-rtos-z2/component/common/application/amazon-freertos
ARG AMEBA_DIR=/opt/ameba

# Clone the repo
RUN git clone --branch ${AMBRTOS_TAG_NAME} --single-branch ${AMBRTOS_REPO} ameba-rtos-z2 \
    && git clone --recurse-submodules --branch ${TAG_NAME} --single-branch ${AMEBA_AWS_REPO} ${AMEBA_AWS_DIR}

# Setup Ameba Amazon FreeRTOS environment
RUN cd ${AMEBA_DIR}/ameba-rtos-z2 \
    && chmod u+x aws_setup.sh \
    && ./aws_setup.sh amebaz2 \
    && cd project/realtek_amebaz2_v0_example/GCC-RELEASE \
    && make toolchain

# Set default working directory
WORKDIR ${AMEBA_DIR}/ameba-rtos-z2

# Set default command to bash for interaction
CMD ["/bin/bash"]
