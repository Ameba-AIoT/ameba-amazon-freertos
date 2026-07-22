# Amazon FreeRTOS on Ameba D

- [Ameba D SDK](#ameba-d-sdk)
    - [Integrating Ameba D SDK and Ameba Amazon FreeRTOS IoT](#integrating-ameba-d-sdk-and-ameba-amazon-freertos-iot)
    - [Configure the Realtek Toolchain](#configure-the-realtek-toolchain)
    - [Build Amazon FreeRTOS library and Final Firmware](#build-amazon-freertos-library-and-final-firmware)
        - [Configuring Amazon FreeRTOS Settings](#configuring-amazon-freertos-settings)
        - [Building the Project](#building-the-project)
            - [Make project_lp](#make-project_lp)
            - [Make project_hp](#make-project_hp)
        - [Cleaning the Build Environment](#cleaning-the-build-environment)
    - [Flashing the Firmware](#flashing-the-firmware)
    - [Monitoring Serial Logs](#monitoring-serial-logs)

## Ameba D SDK

    Tested on Ubuntu 24.04 or above

### Integrating Ameba D SDK and Ameba Amazon FreeRTOS IoT

Before proceeding, ensure both the Ameba SDK and the Amazon FreeRTOS SDK are available. If they are not yet configured, follow the steps below to set up your development environment.

1. Create and Enter a Workspace Directory:

    ```bash
    mkdir dev
    cd dev
    ```

2. Clone the Ameba D Repository:

    ```bash
    git clone https://github.com/Ameba-AIoT/ameba-rtos-d.git
    ```

3. Clone the Ameba Amazon FreeRTOS Repository using `aws_setup.sh` script:

    ```bash
    cd ameba-rtos-d
    chmod +x aws_setup.sh
    ./aws_setup.sh amebad
    ```

### Configure the Realtek Toolchain

To download and extract the legacy Ameba D asdk-6.4.1 toolchain, run the following commands:

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make -C asdk/ check_toolchain

The legacy Ameba D asdk-6.4.1 toolchain is 32-bit x86, so the 64-bit OS system needs i386 multiarch + 32-bit runtime libs to run it. Please run the following command:

    sudo dpkg --add-architecture i386
    sudo apt-get update
    sudo apt-get install libc6:i386 libstdc++6:i386 zlib1g:i386

### Build Amazon FreeRTOS library and Final Firmware

This section demonstrates the build process using the MQTT Mutual Authentication example as a reference.

#### Configuring Amazon FreeRTOS Settings

1. Navigate to the `project_hp` directory:

    ```bash
    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    ```

2. Apply the AWS Configuration in `Menuconfig`:

    ```bash
    make menuconfig
    ```

    - Navigate to `Amazon FreeRTOS Config` and activate `Enable Amazon FreeRTOS`.
    - Navigate to `SSL Config` and change `MBEDTLS Version` to `MBEDTLS_AMAZON_FREERTOS_DEFINED`.
    - Exit `menuconfig` and save the new kernel configuration.

3. To enable your device to connect and authenticate with AWS IoT Core, you must manually update the following header files with your specific **Endpoint** and **Security keys**:
- `ameba-rtos-d/component/common/application/amazon-freertos/demos/include/aws_clientcredential.h` (Update IoT Endpoint and Thing Name)
- `ameba-rtos-d/component/common/application/amazon-freertos/demos/include/aws_clientcredential_keys.h` (Update Private Key and Certificate)
- `ameba-rtos-d/component/common/application/amazon-freertos/ports/config_files/ota_demo_config.h` (Required for OTA example only)

4. To compile MQTT Mutual Authentication, please enable `CONFIG_EXAMPLE_AMAZON_FREERTOS_MQTT_MUTUAL_AUTH` that is located at `ameba-rtos-d/component/common/application/amazon-freertos/ports/config_files/platform_opts_aws.h`:

    ```C
    // Amazon FreeRTOS SDK sub-example, ONLY ENABLE ONE AT A TIME!!!
    #define CONFIG_EXAMPLE_AMAZON_FREERTOS_MQTT_MUTUAL_AUTH         1
    #define CONFIG_EXAMPLE_AMAZON_FREERTOS_HTTP_MUTUAL_AUTH         0
    #define CONFIG_EXAMPLE_AMAZON_FREERTOS_DEVICE_SHADOW            0
    #define CONFIG_EXAMPLE_AMAZON_FREERTOS_DEVICE_DEFENDER          0
    #define CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_MQTT            0
    #define CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_HTTP            0
    #define CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_MQTT_STREAMS    0
    ```

5. To configure the amazon mbedtls version to be used, please navigate to `ameba-rtos-d/component/common/application/amazon-freertos/project/makefile/amebad/Makefile.include.aws` and update `MBEDTLS_VERSION` accordingly. Please only set the value to `mbedtls-3.6.3` or `mbedtls-2.28.1`. By default, `MBEDTLS_VERSION` is set to `mbedtls-2.28.1`.

    ```Makefile
    # mbedtls versions : mbedtls-3.6.3 / mbedtls-2.28.1
    MBEDTLS_VERSION    = mbedtls-2.28.1
    ```

#### Building the Project

##### Make project_lp

Navigate to the `project_lp` directory:

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp/

Build the `project_lp`:

    make all

##### Make project_hp

Navigate to the `project_hp` directory:

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/

Build the `project_hp`:

    make all

#### Cleaning the Build Environment

    make clean

### Flashing the Firmware

Method 1:

- Find more detail in AN0400 Chapter 8.

Method 2 (using image tool and flashing script):

- Follow the Image Tool [README](../tools/Image_Tool_Linux/README.md)

### Monitoring Serial Logs

To view the real-time output from the Ameba D board, use serial port monitor application such as **Tera Term**, **PuTTY**, or `screen` command in Linux. Ensure the baud rate matches the device configuration (typically 115200)
