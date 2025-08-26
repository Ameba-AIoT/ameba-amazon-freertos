# Amazon FreeRTOS on AmebaZ2

- [Get Ameba SDK & Amazon FreeRTOS SDK](#get-ameba-sdk--amazon-freertos-sdk)
- [Ameba Amazon FreeRTOS Configuration](#ameba-amazon-freertos-configuration)
- [Build Amazon FreeRTOS Example & Final Firmware](#build-amazon-freertos-example--final-firmware)
- [Flash Image](#flash-image)

## Get Ameba SDK & Amazon FreeRTOS SDK

The current amazon-freertos submodule is working with the following Ameba Z2 SDK and patches:

- sdk-ameba-v7.1e-rc.zip +
- 7.1e_patch_support_amazon_v202406_LTS_optimized_iNA_r2508XX_(v01).zip

After applying the patches to the SDK, run the amazon_freertos_setup.sh script for linux

```bash
cd sdk-amebaz2
chmod u+x amazon_freertos_setup.sh
./amazon_freertos_setup.sh amebaz2
```

The linux script clones the ameba-amazon-freertos, freertos V10.4.3 repo, and enable AWS Makefile build.

## Ameba Amazon FreeRTOS Configuration

### Enable Amazon FreeRTOS Example

Once `./amazon_freertos_setup.sh amebaz2` is run, it clones the submodules needed and it enabled the `make amazon` and `make is_amazon` makefile command.

### Choosing Amazon FreeRTOS Sub-Example

Navigate to `sdk-amebaz2/component/common/application/amazon-freertos/ports/amebaZ2/config_files` directory. To choose which Amazon FreeRTOS sub-example to run, please modify `aws_platform_opts.h`:

```c
// Amazon FreeRTOS SDK sub-example, ONLY ENABLE ONE AT A TIME!!!
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_MQTT_MUTUAL_AUTO         1
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_HTTP_MUTUAL_AUTO         0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_DEVICE_SHADOW            0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_DEVICE_DEFENDER          0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_MQTT            0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_MQTT_STREAMS    0
```

If Amazon FreeRTOS OTA example is run, at `ota_demo_config.h`, please update the `otapalconfigCODE_SIGNING_CERTIFICATE`. Please refer more to [AmebaZ2 OTA guide](amebaz2_ota_guide.md).

### Updating Amazon FreeRTOS Credentials

Please refer to [Amazon FreeRTOS Configuration User Guide](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-configure.html) on setting up the device credentials. After it is configured, navigate to `sdk-amebaz2/component/common/application/amazon-freertos/demos/include` directory and update the following credential header files:
- at `aws_clientcredential.h`, update the following credentials macros:
    - `clientcredentialMQTT_BROKER_ENDPOINT`
    - `clientcredentialIOT_THING_NAME`
    - `clientcredentialWIFI_SSID`
    - `clientcredentialWIFI_PASSWORD`
- at `aws_clientcredential_keys.h`, update the following credentials macros:
    - `keyCLIENT_CERTIFICATE_PEM`
    - `keyCLIENT_PRIVATE_KEY_PEM`

## Build Amazon FreeRTOS Example & Final Firmware

Navigate to the `GCC-RELEASE` directory, then build the amazon library and the final firmware using the following command:

```bash
cd sdk-amebaz2/project/realtek_amebaZ2_va0_example/GCC-RELEASE/
make amazon
make is_amazon
```

### Clean Ameba Amazon FreeRTOS libraries and application

Navigate to the `GCC-RELEASE` directory, then clean the amazon library and the final firmware using the following command:

```bash
cd sdk-amebaz2/project/realtek_amebaZ2_va0_example/GCC-RELEASE/
make clean_amazon
```

## Flash Image

Find more details in AN0500 Chapter 4.
