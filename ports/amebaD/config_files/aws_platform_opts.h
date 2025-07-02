/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

#ifndef __AWS_PLATFORM_OPTS_H__
#define __AWS_PLATFORM_OPTS_H__

// Amazon FreeRTOS SDK example
#define CONFIG_EXAMPLE_AMAZON_FREERTOS   1

// Amazon FreeRTOS SDK sub-example, ONLY ENABLE ONE AT A TIME!!!
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_MQTT_MUTUAL_AUTO         1
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_HTTP_MUTUAL_AUTO         0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_DEVICE_SHADOW            0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_DEVICE_DEFENDER          0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_MQTT            0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_MQTT_STREAMS    0

// OTA use
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET     ( 0x001FB000 )

// PKCS11 use
#define pkcs11OBJECT_CERT_FLASH_OFFSET       ( 0x001FC000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET   ( 0x001FD000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET    ( 0x001FE000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET ( 0x001FF000 ) //Flash location for code verify Key

#endif //__AWS_PLATFORM_OPTS_H__
