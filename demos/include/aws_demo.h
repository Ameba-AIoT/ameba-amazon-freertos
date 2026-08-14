/*
 * FreeRTOS V202012.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef _AWS_DEMO_H_
#define _AWS_DEMO_H_

#include "platform/iot_network.h"
#include "platform/iot_threads.h"
#include "lwip_netconf.h"

/**
 * @brief All C SDK demo functions have this signature.
 */
typedef int (* demoFunction_t)( bool awsIotMqttMode,
                                const char * pIdentifier,
                                void * pNetworkServerInfo,
                                void * pNetworkCredentialInfo,
                                const IotNetworkInterface_t * pNetworkInterface );


typedef void (* networkConnectedCallback_t)( bool awsIotMqttMode,
                                             const char * pIdentifier,
                                             void * pNetworkServerInfo,
                                             void * pNetworkCredentialInfo,
                                             const IotNetworkInterface_t * pNetworkInterface );

typedef void (* networkDisconnectedCallback_t)( const IotNetworkInterface_t * pNetworkInteface );

void runDemoTask( void * pArgument );

void DEMO_RUNNER_RunDemos( void );

typedef struct demoContext
{
    /* Network types for the demo */
    uint32_t networkTypes;
    /* Function pointers to be set by the implementations for the demo */
    demoFunction_t demoFunction;
    networkConnectedCallback_t networkConnectedCallback;
    networkDisconnectedCallback_t networkDisconnectedCallback;
} demoContext_t;

/* For Realtek SDK */
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBAGREEN2)
#include "ameba_rtos_version.h"
/* For ameba-rtos supported boards */
#if defined(AMEBA_RTOS_VERSION_MAJOR) && (AMEBA_RTOS_VERSION_MAJOR == 1) && defined(AMEBA_RTOS_VERSION_MINOR) && (AMEBA_RTOS_VERSION_MINOR == 2)
/* if SDK version is 1.2.x, use lwip_check_connectivity API with interface index */
#define RTK_SDK_CHECK_CONNECTIVITY() do { \
    LogInfo( ( "Waiting for the network link up event..." ) ); \
    vTaskDelay( pdMS_TO_TICKS( 2000U ) ); \
} while( lwip_check_connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID )
#elif defined(AMEBA_RTOS_VERSION_MAJOR) && (AMEBA_RTOS_VERSION_MAJOR == 1) && defined(AMEBA_RTOS_VERSION_MINOR) && (AMEBA_RTOS_VERSION_MINOR == 1)
/* if SDK version is 1.1.x, use LwIP_Check_Connectivity API*/
#define RTK_SDK_CHECK_CONNECTIVITY() LwIP_Check_Connectivity()
#else
/* Use old method of connectivity detection */
#define RTK_SDK_CHECK_CONNECTIVITY() do { \
    LogInfo( ( "Waiting for the network link up event..." ) ); \
    vTaskDelay( pdMS_TO_TICKS( 2000U ) ); \
} while( wifi_is_connected_to_ap() != 0 )
#endif

#else /* For other boards (Z2, D) */

#define RTK_SDK_CHECK_CONNECTIVITY() do { \
    LogInfo( ( "Waiting for the network link up event..." ) ); \
    vTaskDelay( pdMS_TO_TICKS( 2000U ) ); \
} while( wifi_is_connected_to_ap() != 0 )
#endif

#endif /* _DEMO_SELECTION_H_ */
