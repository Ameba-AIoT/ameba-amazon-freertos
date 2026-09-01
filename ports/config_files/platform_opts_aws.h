/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

#ifndef __PLATFORM_OPTS_AWS_H__
#define __PLATFORM_OPTS_AWS_H__

// Amazon FreeRTOS SDK example
#undef  CONFIG_EXAMPLE_AMAZON_FREERTOS
#define CONFIG_EXAMPLE_AMAZON_FREERTOS   1

// Amazon FreeRTOS SDK sub-example, ONLY ENABLE ONE AT A TIME!!!
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_MQTT_MUTUAL_AUTH                1
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_HTTP_MUTUAL_AUTH                0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_DEVICE_SHADOW                   0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_DEVICE_DEFENDER                 0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_MQTT                   0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_HTTP                   0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_MQTT_STREAMS           0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_FLEET_PROVISIONING_KEYS_CERT    0
#define CONFIG_EXAMPLE_AMAZON_FREERTOS_FLEET_PROVISIONING_CSR          0

#endif //__PLATFORM_OPTS_AWS_H__
