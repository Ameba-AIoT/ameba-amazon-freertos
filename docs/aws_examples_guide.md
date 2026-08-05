# AWS Examples Guidance

The AWS demo source code is centralized in [demos/demo_entry/aws_main.c](../demos/demo_entry/aws_main.c), with the primary entry point identified as aws_main. Realtek provides six comprehensive examples to demonstrate core AWS IoT capabilities:

- [MQTT Mutual Authentication](#mqtt-mutual-authentication)
- [HTTP Mutual Authentication](#http-mutual-authentication)
- [Device Shadow](#device-shadow)
- [Device Defender](#device-defender)
- [OTA over MQTT](#ota-over-mqtt)
- [OTA over MQTT streams](#ota-over-mqtt-streams)
- [Fleet Provisioning with Keys & Cert](#fleet-provisioning-keys-cert)
- [Fleet Provisioning with Certificate Signing Request](#fleet-provisioning-csr)

**Important Note**

If you are using the hardcoded key & certificate for testing, ensure that `#define KEY_PLAINTEXT 1` is set 
- Location: [`ports/config_files/core_pkcs11_config.h`](../ports/config_files/core_pkcs11_config.h)

If you are testing the `Fleet Provisioning` examples and perform a provisioning flow, ensure that `#define KEY_PLAINTEXT 0` is set, along with defining the Claim Credential Certificate and Private Key. This will skip the use of the hardcoded certificate and key.

After the provisioning flow completes, a new device certificate and key will be downloaded onto the device and persists across resets. After this you may attempt to run any of the other examples (e.g `mqtt_mutual_auth`) and it will connect with the new credentials instead.

Please note that currently, the flash addresses defined in the examples do not take into account OTA address regions in flash, meaning that there is a chance that the OTA may overwrite your saved credentials region. Please adjust your flash address regions accordingly to account for space for your FW, OTA space, and persistent data.

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

## Fleet Provisioning (Certificate & Key)

This example demonstrates provisioning flow for devices, using the Claim mechanism to register with AWS, and then obtain a new Thing / Key / Certificate registration via the `Fleet Provisioning` library.

- Source code: [`demos/fleet_provisioning/fleet_provisioning_keys_cert/fleet_provisioning_keys_cert_demo.c`](../demos/fleet_provisioning/fleet_provisioning_keys_cert/fleet_provisioning_keys_cert_demo.c).
- Entry point: [`RunFleetProvisioningKeysCertDemo`](../demos/fleet_provisioning/fleet_provisioning_keys_cert/fleet_provisioning_keys_cert_demo.c#L488).

Ensure `#define KEY_PLAINTEXT 0` is set in [`ports/config_files/core_pkcs11_config.h`](../ports/config_files/core_pkcs11_config.h), and your claim credentials are defined in [`aws_clientcredential_fleet.h`](../demos/include/aws_clientcredential_fleet.h)

Ensure that your provisioning templates are created acording to the [`Setup for Fleet Provisioning Demos`] (https://github.com/aws/aws-iot-device-sdk-embedded-C/blob/main/demos/fleet_provisioning/readme.md)

Expected flow of the example:

Entry check — already provisioned?

The demo first looks for a persisted device certificate, private key in PKCS#11 flash and a stored Thing name. If both are found it
skips the claim flow, loads the device credentials, connects with them, publishes a test message to `test/hello`, and
exits.

Fresh provisioning path:

1. Load claim credentials — the factory-provisioned `claim cert/key` are inserted into the PKCS#11 session.

2. Connect with claim credentials — MQTT session established to AWS IoT Core using the claim certificate and key.

3. `CreateKeysAndCertificate`
- Subscribe to the accepted/rejected topics
    - `$aws/certificates/create/cbor/accepted`
    - `$aws/certificates/create/cbor/rejected`
- Publish an empty payload to `$aws/certificates/create/cbor`.
- Wait for the broker response; on acceptance, extract the new private key, certificate, certificate ID, and ownership token from the CBOR payload.
- Save the private key and certificate into PKCS#11 flash.
- Unsubscribe from the `CreateKeysAndCertificate` topics.

4. `RegisterThing`
- Subscribe to the accepted/rejected `RegisterThing` topics.
    - `$aws/provisioning-templates/<templateName>/provision/cbor/accepted`
    - `$aws/provisioning-templates/<templateName>/provision/cbor/rejected`
- Publish a CBOR payload containing the ownership token and the device serial number to `$aws/provisioning-templates/<templateName>/provision/cbor`.
- Wait for the broker response; on acceptance, extract the assigned Thing name and persist it to flash.
- Unsubscribe from the `RegisterThing` topics.

5. Disconnect the claim-credential session.

6. Reconnect with provisioned credentials — a new MQTT session is established using the **newly stored device certificate and the Thing name**D as the MQTT client ID.

7. Finish — disconnect and exit. On success, optionally write the certificate and private key to additional flash slots defined by `DOWNLOADED_CERT_LABEL` / `DOWNLOADED_PRIVATE_KEY_LABEL`.

For more details, please refer to:
- [AWS Fleet Provisioning Guide](https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html)
- [AWS Fleet Provisioning Setup Guide](https://github.com/aws/aws-iot-device-sdk-embedded-C/tree/main/demos/fleet_provisioning)
- [AWS Fleet Provisioning Library](https://github.com/aws/Fleet-Provisioning-for-AWS-IoT-embedded-sdk)

## Fleet Provisioning (Certificate Signing Request)

This example demonstrates provisioning flow for devices, using the Claim mechanism to register with AWS, and then obtain a new Thing / Certificate registration via the `Fleet Provisioning` library via the Certificate Signing Request (CSR) pathway.

- Source code: [`demos/fleet_provisioning/fleet_provisioning_csr/fleet_provisioning_csr_demo.c`](../demos/fleet_provisioning/fleet_provisioning_csr/fleet_provisioning_csr_demo.c).
- Entry point: [`RunFleetProvisioningCsrDemo`](../demos/fleet_provisioning/fleet_provisioning_csr/fleet_provisioning_csr_demo.c#L488).

Ensure `#define KEY_PLAINTEXT 0` is set in [`ports/config_files/core_pkcs11_config.h`](../ports/config_files/core_pkcs11_config.h), and your claim credentials are defined in [`aws_clientcredential_fleet.h`](../demos/include/aws_clientcredential_fleet.h)

Ensure that your provisioning templates are created acording to the [`Setup for Fleet Provisioning Demos`] (https://github.com/aws/aws-iot-device-sdk-embedded-C/blob/main/demos/fleet_provisioning/readme.md)

Expected flow of the example:

Fresh provisioning path:

1. Load claim credentials — the factory-provisioned `claim cert/key` are inserted into the PKCS#11 session.

2. Connect with claim credentials — MQTT session established using the claim certificate and key.

3. `CreateCertificateFromCsr` (key difference from KeysCert)
- Subscribe to the accepted/rejected topics
    - `$aws/certificates/create-from-csr/cbor`
    - `$aws/certificates/create-from-csr/cbor/accepted`
    - `$aws/certificates/create-from-csr/cbor/rejected`
- Generate a new device key pair on-device (private key stays in PKCS#11 flash, never leaves the device) and produce a CSR from it.
- Wrap the CSR into a CBOR payload and publish to `$aws/certificates/create-from-csr/cbor`.
- Wait for the broker response; on acceptance, extract the signed certificate, certificate ID, and ownership token from the CBOR payload. Note: no private key is returned — it was generated locally.
- Save the certificate into PKCS#11 flash.
- Unsubscribe from the CSR topics.

4. `RegisterThing`
- Subscribe to the accepted/rejected `RegisterThing` topics.
    - `$aws/provisioning-templates/<templateName>/provision/cbor/accepted`
    - `$aws/provisioning-templates/<templateName>/provision/cbor/rejected`
- Publish a CBOR payload containing the ownership token and the device serial number to `$aws/provisioning-templates/<templateName>/provision/cbor`.
- Wait for the broker response; on acceptance, extract the assigned Thing name and persist it to flash.
- Unsubscribe from the `RegisterThing` topics.

5. Disconnect the claim-credential session.

6. Reconnect with provisioned credentials — new MQTT session using the stored device certificate and Thing name as the
MQTT client ID.

7. Finish — disconnect and exit. Optionally writes the certificate to an additional flash slot via
`DOWNLOADED_CERT_LABEL` (no private key label, because the key never left the device).

For more details, please refer to:
- [AWS Fleet Provisioning Guide](https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html)
- [AWS Fleet Provisioning Setup Guide](https://github.com/aws/aws-iot-device-sdk-embedded-C/tree/main/demos/fleet_provisioning)
- [AWS Fleet Provisioning Library](https://github.com/aws/Fleet-Provisioning-for-AWS-IoT-embedded-sdk)

## Additional References

- For information regarding memory usage by the AWS application, please refer to [AWS Example Memory Usage](aws_memory_usage.md).
- If you encounter any issues during building or runtime, please refer to [Troubleshooting Guide for Ameba AWS Solution](aws_troubleshooting_guide.md).
