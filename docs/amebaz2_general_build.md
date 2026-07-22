# Amazon FreeRTOS on Ameba Z2

- [Ameba Z2 SDK](#ameba-z2-sdk)
    - [Integrating Ameba Z2 SDK and Ameba Amazon FreeRTOS IoT](#integrating-ameba-z2-sdk-and-ameba-amazon-freertos-iot)
    - [Configure the Realtek Toolchain](#configure-the-realtek-toolchain)
    - [Build Amazon FreeRTOS library and Final Firmware](#build-amazon-freertos-library-and-final-firmware)
        - [Configuring Amazon FreeRTOS Settings](#configuring-amazon-freertos-settings)
        - [Building the Project](#building-the-project)
        - [Cleaning the Build Environment](#cleaning-the-build-environment)
    - [Flashing the Firmware](#flashing-the-firmware)
    - [Monitoring Serial Logs](#monitoring-serial-logs)

## Ameba Z2 SDK

    Tested on Ubuntu 24.04 or above

### Integrating Ameba Z2 SDK and Ameba Amazon FreeRTOS IoT

Before proceeding, ensure both the Ameba SDK and the Amazon FreeRTOS SDK are available. If they are not yet configured, follow the steps below to set up your development environment.

1. Create and Enter a Workspace Directory:

    ```bash
    mkdir dev
    cd dev
    ```

2. Clone the Ameba Z2 Repository:

    ```bash
    git clone https://github.com/Ameba-AIoT/ameba-rtos-z2.git
    ```

3. Clone the Ameba Amazon FreeRTOS and FreeRTOS v10.4.3 Repository using `aws_setup.sh` script:

    ```bash
    cd ameba-rtos-z2
    chmod +x aws_setup.sh
    ./aws_setup.sh amebaz2
    ```

### Configure the Realtek Toolchain

To extract the legacy Ameba Z2 asdk-6.5.0 toolchain, run the following commands:

    cd ameba-rtos-z2/project/realtek_amebaz2_v0_example/GCC-RELEASE
    make toolchain

### Build Amazon FreeRTOS library and Final Firmware

This section demonstrates the build process using the MQTT Mutual Authentication example as a reference.

#### Configuring Amazon FreeRTOS Settings

1. AWS Configurations has been applied to the project once `aws_setup.sh` is run.

2. To enable your device to connect and authenticate with AWS IoT Core, you must manually update the following header files with your specific **Endpoint** and **Security keys**:
- `ameba-rtos-z2/component/common/application/amazon-freertos/demos/include/aws_clientcredential.h` (Update IoT Endpoint and Thing Name)
- `ameba-rtos-z2/component/common/application/amazon-freertos/demos/include/aws_clientcredential_keys.h` (Update Private Key and Certificate)
- `ameba-rtos-z2/component/common/application/amazon-freertos/ports/config_files/ota_demo_config.h` (Required for OTA example only)

3. To compile MQTT Mutual Authentication, please enable `CONFIG_EXAMPLE_AMAZON_FREERTOS_MQTT_MUTUAL_AUTH` that is located at `ameba-rtos-z2/component/common/application/amazon-freertos/ports/config_files/platform_opts_aws.h`:

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

4. To configure the amazon mbedtls version to be used, please navigate to `ameba-rtos-z2/component/common/application/amazon-freertos/project/makefile/amebaz2/Makefile.include.aws` and update `MBEDTLS_VERSION` accordingly. Please only set the value to `mbedtls-3.6.3` or `mbedtls-2.28.1`. By default, `MBEDTLS_VERSION` is set to `mbedtls-2.28.1`.

    ```Makefile
    # mbedtls versions : mbedtls-3.6.3 / mbedtls-2.28.1
    MBEDTLS_VERSION    = mbedtls-2.28.1
    ```

#### Building the Project

Navigate to the `GCC-RELEASE` directory:

    cd ameba-rtos-z2/project/realtek_amebaz2_v0_example/GCC-RELEASE/

Build the Amazon FreeRTOS library:

    make amazon

In the same folder, build the Amazon FreeRTOS firmware:

    make is_amazon

#### Cleaning the Build Environment

In `GCC-RELEASE` directory, clean Amazon FreeRTOS library:

    make clean_amazon

In the same folder, clean the Amazon FreeRTOS firmware:

    make clean

### Flashing the Firmware

Method 1:

- Find more detail in AN0500 Chapter4

Method 2 (using image tool and flashing script):

- Follow the Image Tool [README](../tools/Image_Tool_Linux/README.md)

### Monitoring Serial Logs

To view the real-time output from the Ameba Z2 board, use serial port monitor application such as **Tera Term**, **PuTTY**, or `screen` command in Linux. Ensure the baud rate matches the device configuration (typically 115200)
