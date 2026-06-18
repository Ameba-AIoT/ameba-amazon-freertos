ameba_list_append(aws_defintions
    
    CONFIG_AMAZON_FREERTOS=1

    # MbedTLS Amazon Settings
    MBEDTLS_THREADING_C
    MBEDTLS_THREADING_ALT
    MBEDTLS_ALLOW_PRIVATE_ACCESS=1
    MBEDTLS_SSL_DTLS_CONNECTION_ID_COMPAT=0

    # FreeRTOS Amazon Definitions
    configPRINTF=vLoggingPrintf
    configPRINT=vLoggingPrint
    configPRINT_STRING=DiagPrintf
    configLOGGING_MAX_MESSAGE_LENGTH=384
    configLOGGING_INCLUDE_TIME_AND_TASK_NAME=1
    configSUPPORT_DYNAMIC_ALLOCATION=1
    configSUPPORT_STATIC_ALLOCATION=1
    configUSE_MALLOC_FAILED_HOOK=1
    configTASK_NOTIFICATION_ARRAY_ENTRIES=4

    #LwIP Config
    LWIP_SNMP=${CONFIG_LWIP_UDP}
    MIB2_STATS=${CONFIG_LWIP_UDP}
    ERRNO=1
    SYS_LIGHTWEIGHT_PROT=1
    LWIP_SO_SNDRCVTIMEO_NONSTANDARD=1

)

if(CONFIG_AMEBADPLUS)
ameba_list_append(aws_defintions
    configPLATFORM_NAME="RealtekAmebaDplus"
)
elseif(CONFIG_AMEBALITE)
ameba_list_append(aws_defintions
    configPLATFORM_NAME="RealtekAmebaLite"
)
elseif(CONFIG_AMEBASMART)
ameba_list_append(aws_defintions
    configPLATFORM_NAME="RealtekAmebaSmart"
)
else()
ameba_list_append(aws_defintions
    configPLATFORM_NAME="RealtekAmebaUnknown"
)
endif()

if(CONFIG_MBEDTLS_AMAZON_DEFINED)
ameba_list_append(aws_defintions
    # use Amazon MbedTLS config
    MBEDTLS_CONFIG_FILE=\<aws_mbedtls_config.h\>
)
endif()

if(AWS_EXAMPLE STREQUAL "mqtt_mutual_auth")
ameba_list_append(aws_defintions
    CONFIG_EXAMPLE_AMAZON_FREERTOS_MQTT_MUTUAL_AUTH=1
)
elseif(AWS_EXAMPLE STREQUAL "http_mutual_auth")
ameba_list_append(aws_defintions
    CONFIG_EXAMPLE_AMAZON_FREERTOS_HTTP_MUTUAL_AUTH=1
)
elseif(AWS_EXAMPLE STREQUAL "device_shadow")
ameba_list_append(aws_defintions
    CONFIG_EXAMPLE_AMAZON_FREERTOS_DEVICE_SHADOW=1
)
elseif(AWS_EXAMPLE STREQUAL "device_defender")
ameba_list_append(aws_defintions
    CONFIG_EXAMPLE_AMAZON_FREERTOS_DEVICE_DEFENDER=1
)
elseif(AWS_EXAMPLE STREQUAL "ota_over_mqtt")
ameba_list_append(aws_defintions
    CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_MQTT=1
)
elseif(AWS_EXAMPLE STREQUAL "ota_over_http")
ameba_list_append(aws_defintions
    CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_HTTP=1
)
elseif(AWS_EXAMPLE STREQUAL "ota_over_mqtt_streams")
ameba_list_append(aws_defintions
    CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_MQTT_STREAMS=1
)
endif()

ameba_list_append(public_definitions
    ${aws_defintions}
)
