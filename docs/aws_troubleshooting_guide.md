# Troubleshooting Guide for Ameba AWS Solution

- [Ameba RTOS SDK Issues](#ameba-rtos-sdk-issues)
    - [Toolchain Not Found](#toolchain-not-found)
    - [Permission Denied for Serial Port](#permission-denied-for-serial-port)
- [AWS Runtime Issue](#aws-runtime-issues)
    - [Device Unable to Enter Sleep Mode](#device-unable-to-enter-sleep-mode)
    - [MQTT Connection Drops (Keep-Alive Timeout)](#mqtt-connection-drops-keep-alive-timeout)
    - [MQTT Error -30848 (No Bytes Read)](#mqtt-error--30848-no-bytes-read)

## Ameba RTOS SDK Issues

### Toolchain Not Found

**Issue:** RTK software suite not exist, failed to download and/or unzip toolchain.

    find: ‘/opt/rtk-toolchain’: No such file or directory
    RTK software suite not exist, please download from https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/prebuilts-v1.0.3/prebuilts-linux-1.0.3.tar.gz or https://rs-wn.oss-cn-shanghai.aliyuncs.com/prebuilts-linux-1.0.3.tar.gz and unzip it at /opt/rtk-toolchain

**Root cause:** `/opt/rtk-toolchain/` is not owned by user and/or toolchain does not exist

**Solution:** Refer to the [Configure the Realtek Toolchain](ameba-rtos_general_build.md#configure-the-realtek-toolchain) section to manually download and extract the toolchain.

### Permission Denied for Serial Port

**Issue:** The flash command fails because the system cannot access the USB port.

    Error: Could not open port /dev/ttyUSB0: permission denied

**Root cause:** User is not within the dialout group

**Solution:** Add your user account to the `dialout` group to grant the necessary permissions. You must restart your computer for these changes to take effect.

    sudo usermod -aG dialout $USER

## AWS Runtime Issues

### Device Unable to Enter Sleep Mode

**Symptom:** The board remains in high-power mode and fails to enter the expected low-power sleep state.

**Root Cause:** The MQTT process loop executes too frequently, preventing the system from switching context to the Idle Task, which is required to trigger sleep.

**Solution:** Adjust the event queue timeout to allow the board enough time to context switch.

**Action:** Tune the value of `MQTT_AGENT_MAX_EVENT_QUEUE_WAIT_TIME` in [ports/config_files/core_mqtt_agent_config.h](../ports/config_files/core_mqtt_agent_config.h#L65).

### MQTT Connection Drops (Keep-Alive Timeout)

**Symptom:** The MQTT connection is lost after a period of time, even with a valid Keep-Alive configuration.

**Root Cause:** A race condition in the coreMQTT library. If the board enters sleep immediately before or after a packet arrives at the LWIP stack, it may skip one process loop. This delay can exceed the MQTT timeout.

**Solution:** Implement a Wakelock during the `PINGREQ` process.

**Action:** Hold the board awake after sending a `PINGREQ` until a response or subsequent packet is received (typically ~1s). Release the wakelock only after the transaction is complete.
 
### MQTT Error -30848 (No Bytes Read)

**Symptom:** The log shows `MQTT error -30848` followed by a failure to read bytes from the stream.

**Root Cause:** If certificates are valid and CONNACK was previously successful, this usually indicates a `Client ID conflict`. AWS IoT Core only allows one active connection per unique credential (Certificate/Private Key/Thing Name). If a second device connects using the same credentials, AWS will disconnect the existing session.

**Solution:** Ensure every physical device (DUT) is provisioned with its own unique certificate and Client ID.
