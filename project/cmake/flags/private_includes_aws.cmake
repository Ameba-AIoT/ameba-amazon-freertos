ameba_list_append(aws_includes
    ${c_CMPT_EXAMPLE_DIR}/application
)

# freertos
ameba_list_append(aws_includes
    ${c_FREERTOS_DIR}/portable/include
)
if(CONFIG_AMEBADPLUS)
ameba_list_append(aws_includes
    ${c_FREERTOS_DIR}/portable/AmebaDplus_KM4/secure
    ${c_FREERTOS_DIR}/portable/AmebaDplus_KM4/non_secure
)
elseif(CONFIG_AMEBALITE)
ameba_list_append(private_includes
    ${c_FREERTOS_DIR}/portable/AmebaLite_KM4/secure
    ${c_FREERTOS_DIR}/portable/AmebaLite_KM4/non_secure
)
elseif(CONFIG_AMEBASMART)
ameba_list_append(public_includes
    ${FREERTOS_DIR}/portable/GCC/ARM_CA7
)
endif()

# demos
ameba_list_append(aws_includes
    ${AWS_DEMOS_DIR}/include
    ${AWS_DEMOS_COMMON_DIR}/http_demo_helpers
    ${AWS_DEMOS_COMMON_DIR}/mqtt_demo_helpers
    ${AWS_DEMOS_COMMON_DIR}/mqtt_subscription_manager
    ${AWS_DEMOS_COMMON_DIR}/pkcs11_helpers
    ${AWS_DEMOS_DEVICE_DEFENDER_DIR}
    ${AWS_DEMOS_DEV_MODE_KEY_PROV_DIR}/include
    ${AWS_DEMOS_OTA_DIR}/ota_demo_mqtt_streams
    ${AWS_DEMO_ENTRY_DIR}
)

# jsmn & tinycbor
ameba_list_append(aws_includes
    ${AWS_THIRD_PARTY_DIR}/jsmn
    ${AWS_TINYCBOR_DIR}/src
)

# corePKCS11
ameba_list_append(aws_includes
    ${AWS_CORE_PKCS11_DIR}/source/dependency/3rdparty/pkcs11/published/2-40-errata-1
    ${AWS_CORE_PKCS11_DIR}/source/dependency/3rdparty/pkcs11
    ${AWS_CORE_PKCS11_DIR}/source/include
)

# abstractions
ameba_list_append(aws_includes
    ${AWS_ABSTRACTIONS_DIR}/mqtt_agent/include
    ${AWS_ABSTRACTIONS_DIR}/platform/include
    ${AWS_ABSTRACTIONS_DIR}/platform/include/platform
    ${AWS_ABSTRACTIONS_DIR}/platform/freertos/include
    ${AWS_ABSTRACTIONS_DIR}/secure_sockets/include
    ${AWS_ABSTRACTIONS_DIR}/transport/secure_sockets
    ${AWS_ABSTRACTIONS_DIR}/wifi/include
)

# c_sdk
ameba_list_append(aws_includes
    ${AWS_C_SDK_DIR}/standard/common/include
    ${AWS_C_SDK_DIR}/standard/common/include/private
)

# freertos_plus
ameba_list_append(aws_includes
    ${AWS_FREERTOS_PLUS_DIR}/standard/crypto/include
    ${AWS_FREERTOS_PLUS_DIR}/standard/utils/include
    ${AWS_FREERTOS_PLUS_DIR}/standard/tls/include
)

# logging
ameba_list_append(aws_includes
    ${AWS_LOGGING_DIR}/include
)

# backoffAlgorithm
ameba_list_append(aws_includes
    ${AWS_BACKOFF_ALGO_DIR}/source/include
)

# coreHTTP
ameba_list_append(aws_includes
    ${AWS_CORE_HTTP_DIR}/source/include
    ${AWS_CORE_HTTP_DIR}/source/interface
    ${AWS_CORE_HTTP_DIR}/source/dependency/3rdparty/llhttp/include
)

# coreJSON
ameba_list_append(aws_includes
    ${AWS_CORE_JSON_DIR}/source/include
)

# coreMQTT
ameba_list_append(aws_includes
    ${AWS_CORE_MQTT_DIR}/source/include
    ${AWS_CORE_MQTT_DIR}/source/interface
)

# coreMQTT-Agent
ameba_list_append(aws_includes
    ${AWS_CORE_MQTT_AGENT_DIR}/source/include
)

# device_defender_for_aws
ameba_list_append(aws_includes
    ${AWS_DEVICE_DEFENDER_DIR}/source/include
)

# device_shadow_for_aws
ameba_list_append(aws_includes
    ${AWS_DEVICE_SHADOW_DIR}/source/include
)

# jobs_for_aws
ameba_list_append(aws_includes
    ${AWS_JOBS_DIR}/source/include
    ${AWS_JOBS_DIR}/source/otaJobParser/include
)

# ota
ameba_list_append(aws_includes
    ${AWS_OTA_DIR}/source/include
    ${AWS_OTA_DIR}/source
    ${AWS_OTA_DIR}/source/portable
    ${AWS_OTA_DIR}/source/portable/os

    ${AWS_MQTT_FILE_STREAMS_DIR}/source/include
)

# ports
ameba_list_append(aws_includes
    ${AWS_PORTS_DIR}/config_files
    ${AWS_PORTS_DIR}/ota
)

# to access pk_*.h for iot_tls.c
ameba_list_append(aws_includes
    ${c_MBEDTLS_DIR}/library
)

ameba_list_append(private_includes
    ${aws_includes}
)
