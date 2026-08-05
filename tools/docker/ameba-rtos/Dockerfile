# Inherit from the common base image
FROM ameba-rtos-aws:common

# Define toolchain directory
ARG TOOLCHAIN_DIR='/opt/rtk-toolchain/'

# Define toolchain ASDK arguments
ARG TOOLCHAIN_ASDK_LINUX_URL='https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/prebuilts-v1.0.3/prebuilts-linux-1.0.3.tar.gz'
ARG TOOLCHAIN_ASDK_LINUX_URL_ALIYUN='https://rs-wn.oss-cn-shanghai.aliyuncs.com/prebuilts-linux-1.0.3.tar.gz'
ARG TOOLCHAIN_ASDK_FILE_PATH='/opt/rtk-toolchain/prebuilts-linux-1.0.3.tar.gz'

# Redefine following build arguments to respective repo and tag/branch
ARG AMEBA_AWS_REPO=https://github.com/lhenry-realtek/ameba-amazon-freertos.git
ARG TAG_NAME=FreeRTOS-LTS-202604.xx

# Define fixed build arguments
ARG AMBRTOS_REPO=https://github.com/Ameba-AIoT/ameba-rtos.git
ARG AMBRTOS_TAG_NAME=v1.2.1
ARG AMEBA_AWS_DIR=/opt/ameba/ameba-amazon-freertos
ARG AMEBA_DIR=/opt/ameba

# Clone the repo
RUN git clone --branch ${AMBRTOS_TAG_NAME} --single-branch ${AMBRTOS_REPO} ameba-rtos \
    && git clone --recurse-submodules --branch ${TAG_NAME} --single-branch ${AMEBA_AWS_REPO} ${AMEBA_AWS_DIR}

# Download the toolchain ASDK
RUN mkdir -p ${TOOLCHAIN_DIR} \
    && ( \
        wget -q --show-progress -P ${TOOLCHAIN_DIR} ${TOOLCHAIN_ASDK_LINUX_URL} \
        || { \
            echo "Primary download failed, attempting mirror..." \
            && wget -q --show-progress -P ${TOOLCHAIN_DIR} ${TOOLCHAIN_ASDK_LINUX_URL_ALIYUN} \
            || ( echo "All download attempts failed"; exit 1 ) \
        } \
    ) \
    && tar xzvf ${TOOLCHAIN_ASDK_FILE_PATH} -C ${TOOLCHAIN_DIR}

# Setup Ameba Amazon FreeRTOS environment
RUN cd ${AMEBA_AWS_DIR} \
    && chmod u+x aws_setup.sh \
    && ./aws_setup.sh ../ameba-rtos \
    && source ${AMEBA_AWS_DIR}/aws_env.sh

# Set default working directory
WORKDIR ${AMEBA_AWS_DIR}

# Set default command to bash for interaction
CMD ["/bin/bash"]
