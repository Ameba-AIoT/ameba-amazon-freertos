# AWS Examples Guidance

The AWS demo source code is centralized in [demos/demo_entry/aws_main.c](../demos/demo_entry/aws_main.c), with the primary entry point identified as aws_main. Realtek provides seven comprehensive examples to demonstrate core AWS IoT capabilities:

- [MQTT Mutual Authentication](#mqtt-mutual-authentication)
- [HTTP Mutual Authentication](#http-mutual-authentication)
- [Device Shadow](#device-shadow)
- [Device Defender](#device-defender)
- [OTA over MQTT](#ota-over-mqtt)
- [OTA over HTTP](#ota-over-http)
- [OTA over MQTT streams](#ota-over-mqtt-streams)

## MQTT Mutual Authentication

This demo demonstrates standard MQTT Publish and Subscribe operations using TLS mutual authentication.
- Source code: [`demos/coreMQTT/mqtt_demo_mutual_auth.c`](../demos/coreMQTT/mqtt_demo_mutual_auth.c).
- Entry point: [`RunCoreMqttMutualAuthDemo`](../demos/coreMQTT/mqtt_demo_mutual_auth.c#L511).

Expected flow of the example:
1. Connect to the MQTT broker.
2. Subscribe to the topic: `example/topic`.
3. Publish a message to `example/topic`.
4. Verify receipt of the published message from the broker.
5. Unsubscribe from `example/topic` MQTT topic.
6. Disconnect from MQTT broker.
7. The sequence repeats twice to verify connection stability.

For more details, please refer to the [AWS IoT MQTT Guide](https://docs.aws.amazon.com/iot/latest/developerguide/mqtt.html)

## HTTP Mutual Authentication

Demonstrates how to perform a secure HTTP POST request to the AWS IoT broker.
- Source code: [`demos/coreHTTP/http_demo_mutual_auth.c`](../demos/coreHTTP/http_demo_mutual_auth.c).
- Entry point: [`RunCoreHttpMutualAuthDemo`](../demos/coreHTTP/http_demo_mutual_auth.c#L237).

Expected flow of the example:
1. Publish HTTP request to the broker
2. Receive HTTP "OK" response from the broker

For more details, please refer to the [AWS IoT HTTP Guide](https://docs.aws.amazon.com/iot/latest/developerguide/http.html)

## Device Shadow

Demonstrates state synchronization between the device and the AWS Cloud using the Device Shadow service.
- Source code: [`demos/device_shadow_for_aws/shadow_demo_main.c`](../demos/device_shadow_for_aws/shadow_demo_main.c).
- Entry point: [`RunDeviceShadowDemo`](../demos/device_shadow_for_aws/shadow_demo_main.c#L784).

Expected flow of the example:
1. Establish an MQTT connection.
2. Clear existing shadow documents (Subscribe to delete/accepted and publish a delete request.)
    1. Subscribe to `shadow/delete/accepted` and `shadow/delete/rejected` MQTT topic
    2. Publish to shadow delete MQTT topic to attempt to delete any shadow documents
    3. Receive delete response from subscribed `shadow/delete/accepted` MQTT Topic
    4. Unsubscribe to `shadow/delete/accepted` and `shadow/delete/rejected` MQTT topic
3. Subscribe to `shadow/update/delta`, `shadow/update/accepted`, and `shadow/update/rejected` MQTT topic
4. Publish `"state"{"powerOn":1}` to `shadow/update` MQTT topic
5. Receive message from subscribed `shadow/update/delta` and `shadow/update/accepted` MQTT Topic
6. Publish `"state"{"powerOn":1}` again to `shadow/update` MQTT topic
7. Receive message from subscribed `shadow/update/accepted` MQTT Topic
8. Unsubscribe from `shadow/update/delta`, `shadow/update/accepted`, and `shadow/update/rejected` MQTT topic
9. Disconnect from MQTT broker

For more details, please refer to the [AWS Device Shadow Guide](https://docs.aws.amazon.com/iot/latest/developerguide/iot-device-shadows.html)

## Device Defender

Demonstrates how to collect and report device metrics to AWS IoT Device Defender for security monitoring.
- Source code: [`demos/device_defender_for_aws/defender_demo.c`](../demos/device_defender_for_aws/defender_demo.c).
- Entry point: [`RunDeviceDefenderDemo`](../demos/device_defender_for_aws/defender_demo.c#L598).

Expected flow of the example:
1. Connect to MQTT broker
2. Subscribe to `defender/metrics/json/accepted` and `defender/metrics/json/rejected` MQTT topic
3. Collect device metrics
    1. Get network stats
    2. Get open TCP ports
    3. Get open UDP ports
    4. Get established connections
    5. Get FreeRTOS stats
    6. Populate device metrics
4. Generate device metrics report into a JSON format
5. Publish device metrics report to `defender/metrics/json` MQTT topic
8. Unsubscribe from `defender/metrics/json/accepted` and `defender/metrics/json/rejected` MQTT topic
9. Disconnect from MQTT broker

For more details, please refer to the [AWS Device Defender Guide](https://docs.aws.amazon.com/greengrass/v2/developerguide/device-defender-component.html)

## OTA over MQTT

These example demonstrate updating device firmware remotely using the **OTA Core** Library.
Before building the example, please refer to [OTA README.md](../tools/ota/README.md) for OTA guide.
- Source code: [`demos/ota/ota_demo_core_mqtt/ota_demo_core_mqtt.c`](../demos/ota/ota_demo_core_mqtt/ota_demo_core_mqtt.c).
- Entry point: [`RunOtaCoreMqttDemo`](../demos/ota/ota_demo_core_mqtt/ota_demo_core_mqtt.c#L1891).

The following is the expected flow of the example:
1. Initialize semaphore for buffer operations
2. Connect to MQTT broker
3. Register `jobs/#` MQTT topic to OTA Agent which will be initialized later
4. Create MQTT Agent Task
5. Start OTA demo
    1. Initialize OTA agent
    2. Initialize OTA agent task
    3. Send start event to OTA agent
    4. Start a loop and wait for OTA job
6. Start OTA job from AWS IoT Console
    - Please refer to [this guide](https://docs.aws.amazon.com/freertos/latest/userguide/ota-console-workflow.html) for the details
7. OTA job will be triggered and wait for the OTA to be finished
8. Once OTA is successfully finished, clean up process will be initiated
    1. Unsubscribe from `jobs/#` MQTT topic
    2. Disconnect from MQTT broker
    3. Delete the semaphore created for buffer operations
9. Device will reset and start with newer version

For more details, please refer to:
- [AWS IoT Over the air (OTA) library](https://docs.aws.amazon.com/freertos/latest/userguide/ota-update-library.html)
- [Creating an OTA update (AWS IoT console)](https://docs.aws.amazon.com/freertos/latest/userguide/ota-console-workflow.html)
- [AWS IoT Jobs](https://docs.aws.amazon.com/iot/latest/developerguide/iot-jobs.html)

## OTA over HTTP

These example demonstrate updating device firmware remotely using the **OTA Core** Library, where the OTA job is controlled over MQTT while the firmware blocks are downloaded over HTTP from a pre-signed Amazon S3 URL.
Before building the example, please refer to [OTA README.md](../tools/ota/README.md) for OTA guide.
- Source code: [`demos/ota/ota_demo_core_http/ota_demo_core_http.c`](../demos/ota/ota_demo_core_http/ota_demo_core_http.c).
- Entry point: [`RunOtaCoreHttpDemo`](../demos/ota/ota_demo_core_http/ota_demo_core_http.c#L2389).

> [!NOTE]
> For AmebaD, please enable PSRAM by setting all `psram_dev_*` config to `TRUE` at `component/soc/realtek/amebad/fwlib/usrcfg/rtl8721dhp_intfcfg.c`, and enable `configUSE_PSRAM_FOR_HEAP_REGION` at `project/realtek_amebaD_va0_example/inc/inc_hp/FreeRTOSConfig.h`.

The following is the expected flow of the example:
1. Initialize semaphore for buffer operations
2. Connect to MQTT broker
3. Register `jobs/#` MQTT topic to OTA Agent which will be initialized later
4. Create MQTT Agent Task
5. Start OTA demo
    1. Initialize OTA agent with MQTT control interface and HTTP data interface
    2. Initialize OTA agent task
    3. Send start event to OTA agent
    4. Start a loop and wait for OTA job
6. Start OTA job from AWS IoT Console
    - Please refer to [this guide](https://docs.aws.amazon.com/freertos/latest/userguide/ota-console-workflow.html) for the details
7. OTA job will be triggered and the firmware blocks will be downloaded over HTTP from the pre-signed Amazon S3 URL
8. Once OTA is successfully finished, clean up process will be initiated
    1. Unsubscribe from `jobs/#` MQTT topic
    2. Disconnect from MQTT broker
    3. Disconnect the HTTP connection
    4. Delete the semaphore created for buffer operations
9. Device will reset and start with newer version

For more details, please refer to:
- [AWS IoT Over the air (OTA) library](https://docs.aws.amazon.com/freertos/latest/userguide/ota-update-library.html)
- [Creating an OTA update (AWS IoT console)](https://docs.aws.amazon.com/freertos/latest/userguide/ota-console-workflow.html)
- [AWS IoT Jobs](https://docs.aws.amazon.com/iot/latest/developerguide/iot-jobs.html)

## OTA over MQTT streams

These example demonstrate updating device firmware remotely using **MQTT Streams** Library.
Before building the example, please refer to [OTA README.md](../tools/ota/README.md) for OTA guide.
- Source code: [`demos/ota/ota_demo_mqtt_streams/ota_demo_mqtt_streams.c`](../demos/ota/ota_demo_mqtt_streams/ota_demo_mqtt_streams.c).
- Entry point: [`RunOtaCoreMqttStreamsDemo`](../demos/ota/ota_demo_mqtt_streams/ota_demo_mqtt_streams.c#L1047).

The following is the expected flow of the example:
1. Initialize semaphore for buffer operations
2. Initialize MQTT Agent
    1. Create semaphore for MQTT subscriptions
    2. Create MQTT event group
    3. Create MQTT Agent Task
3. Create OTA Agent Task
    1. Wait for MQTT Agent to be ready
    2. Initialize OTA Event
    3. Start a loop and wait for OTA job
4. Start OTA job from AWS IoT Console
    - Please refer to [this guide](https://docs.aws.amazon.com/freertos/latest/userguide/ota-console-workflow.html) for the details
5. OTA job will be triggered and wait for the OTA to be finished
6. Once OTA is successfully finished, clean up process will be initiated
    1. Unsubscribe from `jobs/#` MQTT topic
    2. Disconnect from MQTT broker
    3. Delete the semaphore created for buffer operations
7. Device will reset and start with newer version

For more details, please refer to:
- [AWS IoT Over the air (OTA) library](https://docs.aws.amazon.com/freertos/latest/userguide/ota-update-library.html)
- [Creating an OTA update (AWS IoT console)](https://docs.aws.amazon.com/freertos/latest/userguide/ota-console-workflow.html)
- [AWS IoT Core MQTT File Streams Embedded C](https://freertos.org/Documentation/03-Libraries/03-FreeRTOS-core/10-coreMQTT-Streams/01-coreMQTT-Streams)

## Additional References

- For information regarding memory usage by the AWS application, please refer to [AWS Example Memory Usage](aws_memory_usage.md).
- If you encounter any issues during building or runtime, please refer to [Troubleshooting Guide for Ameba AWS Solution](aws_troubleshooting_guide.md).
