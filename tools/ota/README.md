# AWS OTA Guide

Follow this guide to carry out AWS standard Over the Air Software Update

## Prerequisites

OTA update prerequisites [AWS IoT OTA update prerequisites](https://docs.aws.amazon.com/freertos/latest/userguide/ota-prereqs.html)
1. Prerequisites for OTA updates using MQTT
2. Create an Amazon S3 bucket to store your update
3. Create an OTA Update service role
4. Create an OTA user policy
5. Create esdsasigner.key and ecdsasigner.crt by openSSL (optional), example:
    ```
    sudo openssl ecparam -name prime256v1 -genkey -out ecdsa-sha256-signer.key.pem
    sudo openssl req -new -x509 -days 3650 -key ecdsa-sha256-signer.key.pem -out ecdsa-sha256-signer.crt.pem
    ```

## Additional OTA firmware preparation steps

1. Change the `img_ver_minor` or `img_ver_major` version of `image2` in component/soc/amebaXXX/project/manifest.json5. It should be at least +1 of last version. This is used for the bootloader to recognize which OTA partition to read from when the device resets
    ```json
    image2: {
        // version: 2,
        img_id: 1,
        img_ver_major: 1,
        img_ver_minor: 2,
        ...
    }
    ```

2. Change the APP_VERSION_* in ameba-amazon-freertos/ports/config_files/ota_demo_config.h. It should be at least +1 of last version. This is used for OTA Updater to recognize if the OTA version requires upgrading
    ```c
    #ifndef APP_VERSION_MAJOR
        #define APP_VERSION_MAJOR    0
    #endif

    #ifndef APP_VERSION_MINOR
        #define APP_VERSION_MINOR    9
    #endif

    #ifndef APP_VERSION_BUILD
        #define APP_VERSION_BUILD    3	// 2
    #endif
    ```

3. Configure the flash layout.

<details>
  <summary>For RTL8721Dx OTA Image</summary>

Ensure that the OTA address is set correctly according to the device's flash size in the following files:

    component/soc/usrcfg/amebadplus/ameba_flashcfg.c (ameba-rtos_v1.2)

The file can be found in the base SDK.

### Example

In `ameba_flashcfg.c`
```c
const FlashLayoutInfo_TypeDef Flash_Layout[] = {
	/* Region_Type, [StartAddr, EndAddr] */
	{IMG_BOOT,      0x08000000, 0x08013FFF}, //Boot Manifest(4K) + KM4 Bootloader(76K)
	//Users should modify below according to their own memory
	{IMG_APP_OTA1,  0x08014000, 0x081FFFFF}, //Certificate(4K) + Manifest(4K) + KM4 Application OTA1 + RDP IMG OTA1

	{IMG_BOOT_OTA2, 0x08200000, 0x08213FFF}, //Boot Manifest(4K) + KM4 Bootloader(76K) OTA
	{IMG_APP_OTA2,  0x08214000, 0x083DCFFF}, //Certificate(4K) + Manifest(4K) + KM4 Application OTA2 + RDP IMG OTA2

	{FTL,           0x083DD000, 0x083DFFFF}, //FTL for BT(>=12K), The start offset of flash pages which is allocated to FTL physical map.
	{VFS1,          0x083E0000, 0x083FFFFF}, //VFS region 1 (128K)
	{VFS2,          0xFFFFFFFF, 0xFFFFFFFF}, //VFS region 2
	{USER,          0xFFFFFFFF, 0xFFFFFFFF}, //reserve for user

	/* End */
	{0xFF,          0xFFFFFFFF, 0xFFFFFFFF},
};
```

Once you've set the addresses correctly for `IMG_APP_OTA1` and `IMG_APP_OTA2`, ensure that you rebuild both the firmware correctly.

</details>

<details>
  <summary>For RTL8726E / RTL8720E / RTL8713E / RTL8710E OTA Image</summary>

Ensure that the OTA address is set correctly according to the device's flash size in the following files:

    component/soc/usrcfg/amebalite/ameba_flashcfg.c (ameba-rtos_v1.2)

The file can be found in the base SDK.

### Example

In `ameba_flashcfg.c`
```c
FlashLayoutInfo_TypeDef Flash_Layout[] = {
	/* Region_Type, [StartAddr, EndAddr] */
	{IMG_BOOT,      0x08000000, 0x08013FFF}, //Boot Manifest(4K) + KM4 Bootloader(76K)
	//Users should modify below according to their own memory
	{IMG_APP_OTA1,  0x08014000, 0x081FFFFF}, //Certificate(4K) + Manifest(4K) + KR4 & KM4 Application OTA1 + RDP IMG OTA1

	{IMG_BOOT_OTA2, 0x08200000, 0x08213FFF}, //Boot Manifest(4K) + KM4 Bootloader(76K) OTA
	{IMG_APP_OTA2,  0x08214000, 0x083DCFFF}, //Certificate(4K) + Manifest(4K) + KR4 & KM4 Application OTA2 + RDP IMG OTA2
	{FTL,           0x083DD000, 0x083DFFFF}, //FTL for BT(>=12K), The start offset of flash pages which is allocated to FTL physical map.
	{VFS1,          0x083E0000, 0x083FFFFF}, //VFS region 1 (128K)
	{IMG_DSP,       0x08400000, 0x086FFFFF}, //Manifest(4K) + DSP IMG, only one DSP region in layout
	{VFS2,          0xFFFFFFFF, 0xFFFFFFFF}, //VFS region 2
	{USER,          0xFFFFFFFF, 0xFFFFFFFF}, //reserve for user

	/* End */
	{0xFF,          0xFFFFFFFF, 0xFFFFFFFF},
};
```

Once you've set the addresses correctly for `IMG_APP_OTA1` and `IMG_APP_OTA2`, ensure that you rebuild both the firmware correctly.

</details>

<details>
  <summary>For RTL8730E OTA Image</summary>

Ensure that the OTA address is set correctly according to the device's flash size in the following files:

    component/soc/usrcfg/amebasmart/ameba_flashcfg.c (ameba-rtos_v1.2)

The file can be found in the base SDK.

### Example

In `ameba_flashcfg.c`
```c
FlashLayoutInfo_TypeDef Flash_Layout[] = {
	/* Region_Type, [StartAddr, EndAddr] */
	{IMG_BOOT,      0x08000000, 0x0801FFFF}, //Boot Manifest(4K) + KM4 Bootloader(124K)
	//Users should modify below according to their own memory
	{IMG_APP_OTA1,  0x08020000, 0x0831FFFF}, //Certificate(4K) + Manifest(4K) + KM0 & KM4 & CA32 Application OTA1 + RDP IMG OTA1
	// + AP IMG OTA1
	{IMG_BOOT_OTA2, 0x08320000, 0x0835FFFF}, //Boot Manifest(4K) + KM4 Bootloader(252K) OTA
	{IMG_APP_OTA2,  0x08360000, 0x0865FFFF}, //Certificate(4K) + Manifest(4K) + KM0 & KM4 & CA32 Application OTA2 + RDP IMG OTA2
	// + AP IMG OTA2
	{FTL,           0x08660000, 0x08662FFF}, //FTL for BT(>=12K), The start offset of flash pages which is allocated to FTL physical map.
	{VFS1,          0x08663000, 0x08682FFF}, //VFS region 1 (128K)
	{VFS2,          0xFFFFFFFF, 0xFFFFFFFF}, //VFS region 2
	{USER,          0xFFFFFFFF, 0xFFFFFFFF}, //reserve for user
	/* End */
	{0xFF,          0xFFFFFFFF, 0xFFFFFFFF},
};
```

Once you've set the addresses correctly for `IMG_APP_OTA1` and `IMG_APP_OTA2`, ensure that you rebuild both the firmware correctly.

</details>

## How is the OTA firmware signature generated

> [!CAUTION]
> !!!!!!! The key pair in SDK are just for example, please generated new key by openssl !!!!!!
>
> !!!!!!! The key pair in SDK are just for example, please generated new key by openssl !!!!!!
>
> !!!!!!! The key pair in SDK are just for example, please generated new key by openssl !!!!!!

1.	Build the project and ensure that ota_all.bin is generated
2.	Run ameba-amazon-freertos/tools/ota/scripts/python_custom_ecdsa_ameba_gcc.py to output IDT-OTA-Signature. The script requires the following pre-requisites to work
    1.	Python must be installed in the system with version 3.7.x or later
    2.	Pyopenssl library must be installed using 'pip install pyopenssl'
    3.	The ECDSA signing key and the Certificate pair must be present in the same folder as the python script and must be named 'ecdsa-sha256-signer.key.pem' and 'ecdsa-sha256-signer.crt.pem' respectively.
        - There might be some error if there are packages lack in your environment (like openssl...). Please install the package and run the script again.
3.	After getting the IDT-OTA-Signature, you can upload ameba-amazon-freertos/build_RTLXXX/ota_all.bin to the S3 bucket.

## How to trigger a custom signed OTA job in amazon AWS IOT core.

1. Add certificate pem(ecdsa-sha256-signer.crt.pem) into ameba-amazon-freertos/ports/config_files/ota_demo_config.h
2. Click on 'Create OTA update job', select your thing/things group and then select next.
3. In the following page, choose the option 'Use my custom signed firmware image'
4. Choose your custom signed firmware binary that was generated by the python script, and pick a S3 bucket to upload to
5. In the signature field please enter the content of 'IDT-OTA-Signature'
6. Choose hash algorithm as 'SHA-256'
7. Choose encryption algorithm as 'ECDSA'.
8. In "pathname of code signing certificate" and the "Pathname of file on device", both enter '/'
9. Choose the IAM role for OTA update job.(This is the same IAM role as any OTA update job)
10. Click next, give a unique name for the job and create.

## Expected Outcome

- OTA agent task will wait for the OTA job.
- After job has been created, AWS IoT will transfer the ota image by blocks.
- After completion, device will send a report to the AWS IoT.
- Upon receiving reply from AWS IoT, the device will countdown seconds before rebooting.
- If the OTA is successful, the device will reboot into the new image, and OTA job will be finished succesully. 

## Common Mistakes

- ameba-amazon-freertos/ports/config_files/ota_demo_config.h is not updated which will result in OTA fail.
- Version of the new OTA image is not higher than the current image's version. OTA will only be allowed to execute if new image is newer than the current image.
