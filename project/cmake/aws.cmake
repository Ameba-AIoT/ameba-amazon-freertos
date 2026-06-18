# Amazon FreeRTOS Directories
#----------------------------------------#
set(AWS_DIR           "${EXAMPLEDIR}")
set(AWS_CMAKE_DIR     "${AWS_DIR}/project/cmake")
set(AWS_DEMOS_DIR     "${AWS_DIR}/demos")
set(AWS_LIBRARIES_DIR "${AWS_DIR}/libraries")
set(AWS_PORTS_DIR     "${AWS_DIR}/ports")
set(AWS_TOOLS_DIR     "${AWS_DIR}/tools")

# Amazon FreeRTOS libraries Subdirectories
#----------------------------------------#
set(AWS_THIRD_PARTY_DIR       "${AWS_LIBRARIES_DIR}/3rdparty")
set(AWS_MBEDTLS_DIR           "${AWS_LIBRARIES_DIR}/3rdparty/mbedtls")
set(AWS_TINYCBOR_DIR          "${AWS_LIBRARIES_DIR}/3rdparty/tinycbor")

set(AWS_LIBRARIES_COMMON_DIR  "${AWS_LIBRARIES_DIR}/common")
set(AWS_ABSTRACTIONS_DIR      "${AWS_LIBRARIES_DIR}/common/abstractions")
set(AWS_FREERTOS_PLUS_DIR     "${AWS_LIBRARIES_DIR}/common/freertos_plus")
set(AWS_LOGGING_DIR           "${AWS_LIBRARIES_DIR}/common/logging")
set(AWS_C_SDK_DIR             "${AWS_LIBRARIES_DIR}/common/c_sdk")

set(AWS_BACKOFF_ALGO_DIR      "${AWS_LIBRARIES_DIR}/backoffAlgorithm")

set(AWS_CORE_HTTP_DIR         "${AWS_LIBRARIES_DIR}/coreHTTP")
set(AWS_CORE_JSON_DIR         "${AWS_LIBRARIES_DIR}/coreJSON")
set(AWS_CORE_MQTT_DIR         "${AWS_LIBRARIES_DIR}/coreMQTT")
set(AWS_CORE_MQTT_AGENT_DIR   "${AWS_LIBRARIES_DIR}/coreMQTT-Agent")
set(AWS_CORE_PKCS11_DIR       "${AWS_LIBRARIES_DIR}/corePKCS11")

set(AWS_DEVICE_DEFENDER_DIR   "${AWS_LIBRARIES_DIR}/device_defender_for_aws")
set(AWS_DEVICE_SHADOW_DIR     "${AWS_LIBRARIES_DIR}/device_shadow_for_aws")
set(AWS_JOBS_DIR              "${AWS_LIBRARIES_DIR}/jobs_for_aws")
set(AWS_OTA_DIR               "${AWS_LIBRARIES_DIR}/ota_for_aws")
set(AWS_MQTT_FILE_STREAMS_DIR "${AWS_LIBRARIES_DIR}/mqtt_file_streams_for_aws")

# Amazon FreeRTOS demos Subdirectories
#----------------------------------------#
set(AWS_DEMOS_COMMON_DIR            "${AWS_DEMOS_DIR}/common")
set(AWS_DEMOS_OTA_DIR               "${AWS_DEMOS_DIR}/ota")
set(AWS_DEMOS_DEVICE_DEFENDER_DIR   "${AWS_DEMOS_DIR}/device_defender_for_aws")
set(AWS_DEMOS_DEVICE_SHADOW_DIR     "${AWS_DEMOS_DIR}/device_shadow_for_aws")
set(AWS_DEMOS_JOBS_DIR              "${AWS_DEMOS_DIR}/jobs_for_aws")
set(AWS_DEMOS_CORE_HTTP_DIR         "${AWS_DEMOS_DIR}/coreHTTP")
set(AWS_DEMOS_CORE_MQTT_DIR         "${AWS_DEMOS_DIR}/coreMQTT")
set(AWS_DEMOS_DEV_MODE_KEY_PROV_DIR "${AWS_DEMOS_DIR}/dev_mode_key_provisioning")

set(AWS_DEMO_ENTRY_DIR              "${AWS_DEMOS_DIR}/demo_entry")

# Amazon FreeRTOS example checking
#----------------------------------------#
# List of available Amazon FreeRTOS Examples
set(AWS_ALL_EXAMPLES
    "mqtt_mutual_auth"
    "http_mutual_auth"
    "device_shadow"
    "device_defender"
    "ota_over_mqtt"
    "ota_over_http"
    "ota_over_mqtt_streams"
)

# Check if example exists in the list
list(FIND AWS_ALL_EXAMPLES ${AWS_EXAMPLE} AWS_EXAMPLE_INDEX)

if(AWS_EXAMPLE_INDEX GREATER -1)
    message(STATUS      "'${AWS_EXAMPLE}' example is going to be compiled")
else()
    string(REPLACE ";" ", " TIDY_EXAMPLES "${AWS_ALL_EXAMPLES}")
    message(FATAL_ERROR "'${AWS_EXAMPLE}' example is not available. Available examples: ${TIDY_EXAMPLES}")
endif()

# LwIP configuration
set(LWIP_OPTS_FILE "${BASEDIR}/component/lwip/api/lwipopts.h")
# Check LWIP_DEBUG to set flag for LwIP
file(READ ${LWIP_OPTS_FILE} LWIP_OPTS_CONTENT)
string(REGEX MATCH "(^|\n)[ \t]*#define[ \t]+LWIP_UDP([ \t]|/\\*.*\\*/)+1" LWIP_UDP_ENABLED "${LWIP_OPTS_CONTENT}")
if(LWIP_UDP_ENABLED)
    message(STATUS "LWIP_UDP is set to 1")
    set(CONFIG_LWIP_UDP 1)
else()
    message(STATUS "LWIP_UDP is set to 0")
    set(CONFIG_LWIP_UDP 0)
endif()
