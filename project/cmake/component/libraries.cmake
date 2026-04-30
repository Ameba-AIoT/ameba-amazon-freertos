# tinycbor
ameba_list_append(private_sources
    ${AWS_TINYCBOR_DIR}/src/cborencoder.c
    ${AWS_TINYCBOR_DIR}/src/cborencoder_close_container_checked.c
    ${AWS_TINYCBOR_DIR}/src/cborerrorstrings.c
    ${AWS_TINYCBOR_DIR}/src/cborparser.c
    ${AWS_TINYCBOR_DIR}/src/cborparser_dup_string.c
    ${AWS_TINYCBOR_DIR}/src/cborpretty.c
    ${AWS_TINYCBOR_DIR}/src/cborpretty_stdio.c
    ${AWS_TINYCBOR_DIR}/src/cborvalidation.c
)

# mqtt_agent
ameba_list_append(private_sources
    ${AWS_ABSTRACTIONS_DIR}/mqtt_agent/freertos_agent_message.c
    ${AWS_ABSTRACTIONS_DIR}/mqtt_agent/freertos_command_pool.c
)

# secure_sockets
ameba_list_append(private_sources
    ${AWS_ABSTRACTIONS_DIR}/transport/secure_sockets/transport_secure_sockets.c
)

# freertos_plus
ameba_list_append(private_sources
    ${AWS_FREERTOS_PLUS_DIR}/standard/crypto/src/iot_crypto.c
    ${AWS_FREERTOS_PLUS_DIR}/standard/tls/src/iot_tls.c
)

# logging
ameba_list_append(private_sources
    ${AWS_LOGGING_DIR}/iot_logging.c
    ${AWS_LOGGING_DIR}/iot_logging_task_dynamic_buffers.c
)

# backoffAlgorithm
ameba_list_append(private_sources
    ${AWS_BACKOFF_ALGO_DIR}/source/backoff_algorithm.c
)

# coreHTTP
ameba_list_append(private_sources
    ${AWS_CORE_HTTP_DIR}/source/dependency/3rdparty/llhttp/src/api.c
    ${AWS_CORE_HTTP_DIR}/source/dependency/3rdparty/llhttp/src/http.c
    ${AWS_CORE_HTTP_DIR}/source/dependency/3rdparty/llhttp/src/llhttp.c
    ${AWS_CORE_HTTP_DIR}/source/core_http_client.c
)

# coreJSON
ameba_list_append(private_sources
    ${AWS_CORE_JSON_DIR}/source/core_json.c
)

# coreMQTT
ameba_list_append(private_sources
    ${AWS_CORE_MQTT_DIR}/source/core_mqtt.c
    ${AWS_CORE_MQTT_DIR}/source/core_mqtt_serializer.c
    ${AWS_CORE_MQTT_DIR}/source/core_mqtt_state.c
    ${AWS_CORE_MQTT_DIR}/source/core_mqtt_serializer_private.c
    ${AWS_CORE_MQTT_DIR}/source/core_mqtt_prop_serializer.c
    ${AWS_CORE_MQTT_DIR}/source/core_mqtt_prop_deserializer.c
)

## coreMQTT Agent
#ameba_list_append(private_sources
#    ${AWS_CORE_MQTT_AGENT_DIR}/source/core_mqtt_agent.c
#    ${AWS_CORE_MQTT_AGENT_DIR}/source/core_mqtt_agent_command_functions.c
#)

# corePKCS11
ameba_list_append(private_sources
    ${AWS_CORE_PKCS11_DIR}/source/portable/mbedtls/core_pkcs11_mbedtls.c
    ${AWS_CORE_PKCS11_DIR}/source/core_pkcs11.c
    ${AWS_CORE_PKCS11_DIR}/source/core_pki_utils.c
)

## device defender
ameba_list_append(private_sources
    ${AWS_DEVICE_DEFENDER_DIR}/source/defender.c
)

# device shadow
ameba_list_append(private_sources
    ${AWS_DEVICE_SHADOW_DIR}/source/shadow.c
)

# jobs_for_aws
ameba_list_append(private_sources
    ${AWS_JOBS_DIR}/source/jobs.c
    ${AWS_JOBS_DIR}/source/otaJobParser/job_parser.c
    ${AWS_JOBS_DIR}/source/otaJobParser/ota_job_handler.c
)

# ota_for_aws
ameba_list_append(private_sources
    #${AWS_OTA_DIR}/source/portable/os/ota_os_freertos.c
    #${AWS_OTA_DIR}/source/ota.c
    #${AWS_OTA_DIR}/source/ota_base64.c
    #${AWS_OTA_DIR}/source/ota_cbor.c
    #${AWS_OTA_DIR}/source/ota_http.c
    #${AWS_OTA_DIR}/source/ota_interface.c
    #${AWS_OTA_DIR}/source/ota_mqtt.c
)

# mbedtls_utils
ameba_list_append(private_sources
    ${AWS_THIRD_PARTY_DIR}/mbedtls_utils/mbedtls_error.c
    ${AWS_THIRD_PARTY_DIR}/mbedtls_utils/mbedtls_utils.c
)

# mqtt_file_streams_for_aws
ameba_list_append(private_sources
    ${AWS_MQTT_FILE_STREAMS_DIR}/source/MQTTFileDownloader.c
    ${AWS_MQTT_FILE_STREAMS_DIR}/source/MQTTFileDownloader_cbor.c
    ${AWS_MQTT_FILE_STREAMS_DIR}/source/MQTTFileDownloader_base64.c
)
