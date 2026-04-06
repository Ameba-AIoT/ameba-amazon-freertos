# Amazon FreeRTOS on Ameba RTOS

- [Ameba RTOS v1.2 SDK](#ameba-rtos-v12-sdk)
    - [Integrating Ameba RTOS SDK and Ameba Amazon FreeRTOS IoT](#integrating-ameba-rtos-sdk-and-ameba-amazon-freertos-iot)
    - [Configure the Realtek Toolchain](#configure-the-realtek-toolchain)
    - [Initialize the Amazon FreeRTOS Build Environment](#initialize-the-amazon-freertos-build-environment)
    - [Build Amazon FreeRTOS library and Final Firmware using ameba.py](#build-amazon-freertos-library-and-final-firmware-using-amebapy)
        - [Amazon FreeRTOS Helper Commands](#amazon-freertos-helper-commands)
        - [Configuring Amazon FreeRTOS Settings](#configuring-amazon-freertos-settings)
        - [Building the Project](#building-the-project)
        - [Cleaning the Build Environment](#cleaning-the-build-environment)
    - [Flashing the Firmware using ameba.py command](#flashing-the-firmware-using-amebapy-command)
    - [Monitoring Serial Logs](#monitoring-serial-logs)

## Ameba RTOS v1.2 SDK

    Tested on Ubuntu 24.04 or above

### Integrating Ameba RTOS SDK and Ameba Amazon FreeRTOS IoT

Before proceeding, ensure both the Ameba SDK and the Amazon FreeRTOS SDK are available. If they are not yet configured, follow the steps below to set up your development environment.

1. Create and Enter a Workspace Directory:

    ```bash
    mkdir dev
    cd dev
    ```

2. Clone the Ameba RTOS Repository:

    ```bash
    git clone https://github.com/Ameba-AIoT/ameba-rtos.git -b release/v1.2
    ```

3. Clone the Ameba Amazon FreeRTOS Repository:

    ```bash
    git clone https://github.com/Ameba-AIoT/ameba-amazon-freertos.git --recurse-submodules -b FreeRTOS-LTS-202406.xx
    ```

> [!NOTE]
> It is essential that both `ameba-rtos` and `ameba-amazon-freertos` are located at the same directory level to ensure relative paths resolve correctly.

    dev/
    ├── ameba-rtos
    └── ameba-amazon-freertos

### Configure the Realtek Toolchain

Install the pre-built toolchain to the standard `/opt` directory:

    # Create toolchain directory
    sudo mkdir /opt/rtk-toolchain
    sudo chown -R $(id -un):$(id -gn) /opt/rtk-toolchain/

    # Download toolchain
    wget -P /opt/rtk-toolchain/ https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/prebuilts-v1.0.3/prebuilts-linux-1.0.3.tar.gz

    # Extract toolchain
    tar xzvf /opt/rtk-toolchain/prebuilts-linux-1.0.3.tar.gz -C /opt/rtk-toolchain/

### Initialize the Amazon FreeRTOS Build Environment

    # Navigate to the FreeRTOS directory
    cd ameba-amazon-freertos
    chmod u+x aws_setup.sh

    # Link the Amazon SDK to the Ameba RTOS SDK
    ./aws_setup.sh ../ameba-rtos

    # Source the environment variables for the current session
    source aws_env.sh

    # Configure for the specific SoC (e.g., AmebaDplus)
    ameba.py soc < RTL8721Dx / RTL8726E / RTL8720E / RTL8713E / RTL8710E / RTL8730E >

### Build Amazon FreeRTOS library and Final Firmware using ameba.py

This section demonstrates the build process using the MQTT Mutual Authentication `mqtt_mutual_auth` example as a reference.

#### Amazon FreeRTOS Helper Commands

Once the Amazon FreeRTOS environment is activated, three specialized helper commands become available to streamline your workflow:
- `aws_apply_conf`: Initializes the project by applying the default Amazon FreeRTOS configuration settings.
- `aws_build_proj`: You must specify the desired example (e.g., `mqtt_mutual_auth`) when running this command.
- `aws_clean_proj`: Performs a full cleanup of the project build artifacts, ensuring a fresh state for subsequent builds.

> [!IMPORTANT]
> Whenever you open a new terminal session to build the application, you must first re-activate the environment.

    source aws_env.sh

#### Configuring Amazon FreeRTOS Settings

1. Navigate to the `ameba-amazon-freertos` directory:

    ```bash
    cd ameba-amazon-freertos
    ```

2. Apply the AWS Configuration in Menuconfig:

    ```bash
    aws_apply_conf
    ```

> [!NOTE]
> If you need to adjust additional project settings, you can open the interactive configuration menu:
>
> ```bash
> ameba.py menuconfig
> ```
>
> To verify the AWS status manually: Navigate to `CONFIG APPLICATION` → `AWS Config` and ensure `Enable AWS` is selected.

3. To enable your device to connect and authenticate with AWS IoT Core, you must manually update the following header files with your specific **Endpoint** and **Security keys**:
- `ameba-amazon-freertos/demos/include/aws_clientcredential.h` (Update IoT Endpoint and Thing Name)
- `ameba-amazon-freertos/demos/include/aws_clientcredential_keys.h` (Update Private Key and Certificate)
- `ameba-amazon-freertos/ports/config_files/ota_demo_config.h` (Required for OTA example only)

#### Building the Project

From within the `ameba-amazon-freertos` directory, execute the following command to compile the Amazon FreeRTOS libraries and generate the final firmware:

- Option A: Using AWS Helper **(Recommended)**

    If your environment is sourced, use the helper shortcut to build a specific example (e.g., MQTT Mutual Auth):

    ```bash
    aws_build_proj mqtt_mutual_auth
    ```

- Option B: Standard Build Command without AWS helper command

    Alternatively, you can call the build script directly with the required definitions:

    ```bash
    ameba.py build -D EXAMPLE="$PWD" AWS_EXAMPLE=mqtt_mutual_auth
    ```

#### Cleaning the Build Environment

To perform a complete cleanup of the project and remove all compiled artifacts, run within `ameba-amazon-freertos` folder:

- Option A: Using AWS Helper **(Recommended)**

    If you have the build environment correctly sourced, use the helper shortcut for a full clean:

    ```bash
    aws_clean_proj
    ```

- Option B: Standard Cleaning without AWS helper command

    Alternatively, use the base build script directly:

    ```bash
    ameba.py clean
    ```

### Flashing the Firmware using ameba.py command

After a successful build, the binary images are located in: `ameba-amazon-freertos/build_<CHIP_MODEL>/`
- **Bootloader**: `km4_boot_all.bin`
- **Application**:
    - RTL8721Dx: `km0_km4_app.bin`
    - RTL8726E / RTL8720E / RTL8713E / RTL8710E: `kr4_km4_app.bin`
    - RTL8730E: `km0_km4_ca32_app.bin`

The flashing process requires two primary files: `km4_boot_all.bin` and the application binary (e.g., `km0_km4_app.bin`). To flash the device via a specific port (e.g., `/dev/ttyUSB0`), run:

    ameba.py flash -p /dev/ttyUSB0

**Advanced Flashing (Large Images)**: If the application image exceeds standard size limits, use the --image (or -i) option to specify manual memory offsets:

<details>
  <summary>Advanced flash command to RTL8721Dx</summary>

    ameba.py flash -p /dev/ttyUSB0 -i km4_boot_all.bin 0x08000000 0x08014000 -i km0_km4_app.bin 0x08014000 0x08300000

</details>

<details>
  <summary>Advanced flash command to RTL8726E / RTL8720E / RTL8713E / RTL8710E</summary>

    ameba.py flash -p /dev/ttyUSB0 -i km4_boot_all.bin 0x08000000 0x08014000 -i kr4_km4_app.bin 0x08014000 0x08300000

</details>

<details>
  <summary>Advanced flash command to RTL8730E</summary>

    ameba.py flash -p /dev/ttyUSB0 -i km4_boot_all.bin 0x08000000 0x08040000 -i km0_km4_ca32_app.bin 0x08040000 0x08340000

</details>

### Monitoring Serial Logs

To view the real-time output from the Ameba board, use the monitor command. Ensure the baud rate matches the device configuration (typically 1500000):

    ameba.py monitor -p /dev/ttyUSB0 -b 1500000
