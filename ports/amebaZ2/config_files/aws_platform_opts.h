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


/* For Amazon FreeRTOS SDK example */
#define CONFIG_EXAMPLE_AMAZON_FREERTOS 1
#define CONFIG_EXAMPLE_AMAZON_AFQP_TESTS 0

// Amazon FreeRTOS SDK sub-example, ONLY ENABLE ONE AT A TIME!!!
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_MQTT_MUTUAL_AUTO         1
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_HTTP_MUTUAL_AUTO         0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_DEVICE_SHADOW            0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_DEVICE_DEFENDER          0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_MQTT            0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_MQTT_STREAMS    0

#undef  CONFIG_INCLUDE_SIMPLE_CONFIG
#define CONFIG_INCLUDE_SIMPLE_CONFIG		0

#undef  CONFIG_EXAMPLE_WLAN_FAST_CONNECT
#define CONFIG_EXAMPLE_WLAN_FAST_CONNECT	1

#undef  CONFIG_FAST_DHCP
#define CONFIG_FAST_DHCP 1

#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_SSL_CLIENT_PRIVATE_IN_TZ == 0)
#undef  SUPPORT_LOG_SERVICE
#define SUPPORT_LOG_SERVICE					0
#endif

// OTA use
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET     0x00003000 // Flash reserved section 0x0000_3000 - 0x0000_4000-1

// PKCS11 use
#define pkcs11OBJECT_CERT_FLASH_OFFSET       (0x200000 - 0xB000) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET   (0x200000 - 0xC000) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET    (0x200000 - 0xD000) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET (0x200000 - 0xE000) //Flash location for code verify Key

#endif //__AWS_PLATFORM_OPTS_H__
