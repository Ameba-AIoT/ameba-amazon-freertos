include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/common/application/amazon-freertos
vpath %.c $(DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

##amazon_freertos_LTS - demos
#common
CSRC += $(DIR)/demos/common/http_demo_helpers/http_demo_utils.c
CSRC += $(DIR)/demos/common/mqtt_demo_helpers/mqtt_demo_helpers.c
CSRC += $(DIR)/demos/common/mqtt_subscription_manager/mqtt_subscription_manager.c
CSRC += $(DIR)/demos/common/ota_demo_helpers/ota_application_version.c
CSRC += $(DIR)/demos/common/pkcs11_helpers/pkcs11_helpers.c
CSRC += $(DIR)/demos/dev_mode_key_provisioning/src/aws_dev_mode_key_provisioning.c
#coreHTTP
CSRC += $(DIR)/demos/coreHTTP/http_demo_mutual_auth.c
#coreMQTT
CSRC += $(DIR)/demos/coreMQTT/mqtt_demo_mutual_auth.c
#device_shadow_for_aws
CSRC += $(DIR)/demos/device_shadow_for_aws/shadow_demo_main.c
#device_defender_for_aws
CSRC += $(DIR)/demos/device_defender_for_aws/metrics_collector/lwip/metrics_collector.c
CSRC += $(DIR)/demos/device_defender_for_aws/defender_demo.c
CSRC += $(DIR)/demos/device_defender_for_aws/report_builder.c
#jobs_for_aws
CSRC += $(DIR)/demos/jobs_for_aws/jobs_demo.c
#ota
CSRC += $(DIR)/demos/ota/ota_demo_core_mqtt/ota_demo_core_mqtt.c
CSRC += $(DIR)/demos/ota/ota_demo_core_http/ota_demo_core_http.c
#ota w/ mqtt file streams aws
CSRC += $(DIR)/demos/ota/ota_demo_mqtt_streams/ota_demo_mqtt_streams.c
CSRC += $(DIR)/demos/ota/ota_demo_mqtt_streams/ota_event_freertos.c
CSRC += $(DIR)/demos/ota/ota_demo_mqtt_streams/mqtt_agent_task.c
CSRC += $(DIR)/demos/ota/ota_demo_mqtt_streams/mqtt_wrapper.c
#fleet provisioning demo
CSRC += $(DIR)/demos/fleet_provisioning/fleet_provisioning_csr/fleet_provisioning_csr_demo.c
CSRC += $(DIR)/demos/fleet_provisioning/fleet_provisioning_keys_cert/fleet_provisioning_keys_cert_demo.c
CSRC += $(DIR)/demos/fleet_provisioning/common_operations/mqtt_operations.c
CSRC += $(DIR)/demos/fleet_provisioning/common_operations/pkcs11_operations.c
CSRC += $(DIR)/demos/fleet_provisioning/common_operations/fleet_provisioning_serializer.c

###libraries - 3rdparty
#jsmn
CSRC += $(DIR)/libraries/3rdparty/jsmn/jsmn.c
#mbedtls_utils
CSRC += $(DIR)/libraries/3rdparty/mbedtls_utils/mbedtls_error.c
CSRC += $(DIR)/libraries/3rdparty/mbedtls_utils/mbedtls_utils.c
#tinycbor
CSRC += $(DIR)/libraries/3rdparty/tinycbor/src/cborencoder.c
CSRC += $(DIR)/libraries/3rdparty/tinycbor/src/cborencoder_close_container_checked.c
CSRC += $(DIR)/libraries/3rdparty/tinycbor/src/cborerrorstrings.c
CSRC += $(DIR)/libraries/3rdparty/tinycbor/src/cborparser.c
CSRC += $(DIR)/libraries/3rdparty/tinycbor/src/cborparser_dup_string.c
CSRC += $(DIR)/libraries/3rdparty/tinycbor/src/cborpretty.c
CSRC += $(DIR)/libraries/3rdparty/tinycbor/src/cborpretty_stdio.c
CSRC += $(DIR)/libraries/3rdparty/tinycbor/src/cborvalidation.c

##common
#mqtt_agent
CSRC += $(DIR)/libraries/common/abstractions/mqtt_agent/freertos_agent_message.c
CSRC += $(DIR)/libraries/common/abstractions/mqtt_agent/freertos_command_pool.c

#transport - secure_sockets
CSRC += $(DIR)/libraries/common/abstractions/transport/secure_sockets/transport_secure_sockets.c

##freertos_plus - standard
#crypto
CSRC += $(DIR)/libraries/common/freertos_plus/standard/crypto/src/iot_crypto.c
#tls
CSRC += $(DIR)/libraries/common/freertos_plus/standard/tls/src/iot_tls.c

##logging
CSRC += $(DIR)/libraries/common/logging/iot_logging.c
CSRC += $(DIR)/libraries/common/logging/iot_logging_task_dynamic_buffers.c


##backoffAlgorithm
CSRC += $(DIR)/libraries/backoffAlgorithm/source/backoff_algorithm.c

##coreHTTP + llhttp
CSRC += $(DIR)/libraries/coreHTTP/source/dependency/3rdparty/llhttp/src/api.c
CSRC += $(DIR)/libraries/coreHTTP/source/dependency/3rdparty/llhttp/src/http.c
CSRC += $(DIR)/libraries/coreHTTP/source/dependency/3rdparty/llhttp/src/llhttp.c
CSRC += $(DIR)/libraries/coreHTTP/source/core_http_client.c

##coreJSON
CSRC += $(DIR)/libraries/coreJSON/source/core_json.c

##coreMQTT
CSRC += $(DIR)/libraries/coreMQTT/source/core_mqtt.c
CSRC += $(DIR)/libraries/coreMQTT/source/core_mqtt_serializer.c
CSRC += $(DIR)/libraries/coreMQTT/source/core_mqtt_state.c
CSRC += $(DIR)/libraries/coreMQTT/source/core_mqtt_serializer_private.c
CSRC += $(DIR)/libraries/coreMQTT/source/core_mqtt_prop_serializer.c
CSRC += $(DIR)/libraries/coreMQTT/source/core_mqtt_prop_deserializer.c

##coreMQTT-Agent
CSRC += $(DIR)/libraries/coreMQTT-Agent/source/core_mqtt_agent.c
CSRC += $(DIR)/libraries/coreMQTT-Agent/source/core_mqtt_agent_command_functions.c

##corePKCS11
CSRC += $(DIR)/libraries/corePKCS11/source/portable/mbedtls/core_pkcs11_mbedtls.c
CSRC += $(DIR)/libraries/corePKCS11/source/core_pkcs11.c
CSRC += $(DIR)/libraries/corePKCS11/source/core_pki_utils.c

##device_defender_for_aws
CSRC += $(DIR)/libraries/device_defender_for_aws/source/defender.c

##device_shadow_for_aws
CSRC += $(DIR)/libraries/device_shadow_for_aws/source/shadow.c

##jobs_for_aws
CSRC += $(DIR)/libraries/jobs_for_aws/source/jobs.c
CSRC += $(DIR)/libraries/jobs_for_aws/source/otaJobParser/job_parser.c
CSRC += $(DIR)/libraries/jobs_for_aws/source/otaJobParser/ota_job_handler.c

##ota_for_aws
CSRC += $(DIR)/libraries/ota_for_aws/source/portable/os/ota_os_freertos.c
CSRC += $(DIR)/libraries/ota_for_aws/source/ota.c
CSRC += $(DIR)/libraries/ota_for_aws/source/ota_base64.c
CSRC += $(DIR)/libraries/ota_for_aws/source/ota_cbor.c
CSRC += $(DIR)/libraries/ota_for_aws/source/ota_http.c
CSRC += $(DIR)/libraries/ota_for_aws/source/ota_interface.c
CSRC += $(DIR)/libraries/ota_for_aws/source/ota_mqtt.c

##mqtt_file_streams_for_aws
CSRC += $(DIR)/libraries/mqtt_file_streams_for_aws/source/MQTTFileDownloader.c
CSRC += $(DIR)/libraries/mqtt_file_streams_for_aws/source/MQTTFileDownloader_cbor.c
CSRC += $(DIR)/libraries/mqtt_file_streams_for_aws/source/MQTTFileDownloader_base64.c

##fleet_provisioning
CSRC += $(DIR)/libraries/fleet_provisioning_for_aws/source/fleet_provisioning.c

#demo_entry
CSRC += $(DIR)/demos/demo_entry/example_amazon_freertos.c
CSRC += $(DIR)/demos/demo_entry/aws_main.c

#ports
CSRC += $(DIR)/ports/ota/ota_pal.c
CSRC += $(DIR)/ports/ota/ota_pal_streams.c
CSRC += $(DIR)/ports/ota/aws_ota_ameba.c
CSRC += $(DIR)/ports/ota/aws_ota_streams_ameba.c
CSRC += $(DIR)/ports/pkcs11/core_pkcs11_pal_ameba.c
CSRC += $(DIR)/ports/secure_sockets/iot_secure_sockets.c
CSRC += $(DIR)/ports/wifi/iot_wifi.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS =  $(CSRC:.c=.o)

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                                   PATCH                                     #
#*****************************************************************************#
# Apply patch to libraries/ota_for_aws/source/ota.c, removing hardfault issue from AWS HTTP OTA example.
# The patch truncates the update_data_url log so the long pre-signed S3 URL does not overflow the log buffer.
OTA_PATCH_SRC = $(DIR)/ports/patches/ota_for_aws/ota.c
OTA_PATCH_DST = $(DIR)/libraries/ota_for_aws/source/ota.c

AWS_OTA_PATCH:
	cp -f $(OTA_PATCH_SRC) $(OTA_PATCH_DST)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
all: AWS_OTA_PATCH CORE_TARGETS COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d
	$(REMOVE) *.c

-include $(DEPS)
