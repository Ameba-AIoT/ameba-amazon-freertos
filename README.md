# FreeRTOS AWS Reference Integrations

## How to use
This is the library sdk which provides Amazon freertos examples. To make sure example working correctly, please download the working chip sdk from related page with this submodule updated.

| Chip        | SDK        | AWS FreeRTOS Version   | mbedtls Version | Branch Link                                                                                    |
|-------------|------------|------------------------|-----------------|------------------------------------------------------------------------------------------------|
| Ameba Z2    | 7.1d       | FreeRTOS-LTS-202107.xx | 2.28.1          | https://github.com/Ameba-AIoT/ameba-amazon-freertos/tree/amebaZ2-7.1d-202107.00-LTS            |
|             | 7.1e       | FreeRTOS-LTS-202406.xx | 3.6.3           | https://github.com/Ameba-AIoT/ameba-amazon-freertos/tree/FreeRTOS-LTS-202406.xx_mbedtls-v3.6.3 |
| Ameba D     | 6.2c       | FreeRTOS-LTS-202406.xx | 2.28.1          | https://github.com/Ameba-AIoT/ameba-amazon-freertos/tree/FreeRTOS-LTS-202406.xx                |
|             | 6.2c       | FreeRTOS-LTS-202406.xx | 3.6.3           | https://github.com/Ameba-AIoT/ameba-amazon-freertos/tree/FreeRTOS-LTS-202406.xx_mbedtls-v3.6.3 |
| Ameba DPlus | rtos-v1.0a | FreeRTOS-LTS-202406.xx | 2.28.1          | https://github.com/Ameba-AIoT/ameba-amazon-freertos/tree/FreeRTOS-LTS-202406.xx                |
|             | rtos-v1.0a | FreeRTOS-LTS-202406.xx | 3.6.3           | https://github.com/Ameba-AIoT/ameba-amazon-freertos/tree/FreeRTOS-LTS-202406.xx_mbedtls-v3.6.3 |
|             | rtos-v1.1  | FreeRTOS-LTS-202406.xx | 3.6.3           | https://github.com/Ameba-AIoT/ameba-amazon-freertos/tree/FreeRTOS-LTS-202406.xx_mbedtls-v3.6.3 |
| Ameba Lite  | rtos-v1.1  | FreeRTOS-LTS-202406.xx | 3.6.3           | https://github.com/Ameba-AIoT/ameba-amazon-freertos/tree/FreeRTOS-LTS-202406.xx_mbedtls-v3.6.3 |

## Cloning
This repo uses [Git Submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules) to bring in dependent components.

Note: If you download the ZIP file provided by GitHub UI, you will not get the contents of the submodules. (The ZIP file is also not a valid git repository)

To clone using HTTPS:
```
git clone https://github.com/Ameba-AIoT/ameba-amazon-freertos.git --recurse-submodules
```
Using SSH:
```
git clone git@github.com:Ameba-AIoT/ameba-amazon-freertos.git --recurse-submodules
```

If you have downloaded the repo without using the `--recurse-submodules` argument, you need to run:
```
git submodule update --init --recursive
```

## Getting Started

For more information on FreeRTOS, refer to the [Getting Started section of FreeRTOS webpage](https://aws.amazon.com/freertos).

To directly access the **Getting Started Guide** for supported hardware platforms, click the corresponding link in the Supported Hardware section below.

For detailed documentation on FreeRTOS, refer to the [FreeRTOS User Guide](https://aws.amazon.com/documentation/freertos).

## Supported Hardware

For additional boards that are supported for FreeRTOS, please visit the [AWS Device Catalog](https://devices.amazonaws.com/search?kw=freertos)

The following MCU boards are supported for FreeRTOS:
1. **Realtek Boards** - Please refer to the support patch provided by our FAEs for the latest READMEs and Getting Started Documents.
     List of supported boards may be found in the table above and below

2. **Windows Simulator** - To evaluate FreeRTOS without using MCU-based hardware, you can use the Windows Simulator.
    * Requirements: Microsoft Windows 7 or newer, with at least a dual core and a hard-wired Ethernet connection
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_windows.html)
    * IDE: [Visual Studio Community Edition](https://www.visualstudio.com/downloads/)


## amazon-freeRTOS/projects
The ```./projects``` folder contains the IDE test and demo projects for each vendor and their boards. The majority of boards can be built with both IDE and cmake (there are some exceptions!). Please refer to the Getting Started Guides above for board specific instructions.

## Mbed TLS License
This repository uses Mbed TLS under Apache 2.0

# Support list

## AmebaZ2
Work with

sdk-ameba-v7.1e-rc.zip +

7.1e_patch_support_amazon_v202406_LTS_mbedtls363_iNA_r250618_(v01).zip

## AmebaD
Work with 

sdk-amebad_v6.2C-RC.tar.gz + 

6.2_patch_integrated_250626_15692ffb.zip

6.2c_patch_Support_Amazon_v202406_LTS_mbedtls363_i250626_r250818_(v02).zip

## AmebaDplus
Work with

sdk-ameba-rtos_v1.1.zip +

1.1_patch_dplus_and_lite_amazon_v202406-LTS_w_mbed363_iNA_r250806_v01.zip

## AmebaLite
Work with

sdk-ameba-rtos_v1.1.zip +

1.1_patch_dplus_and_lite_amazon_v202406-LTS_w_mbed363_iNA_r250806_v01.zip


## AmebaSmart
Work with

(WIP!)

# History link

This repository is continuing development from old repository link

https://github.com/Ameba-AIoT/amazon-freertos
