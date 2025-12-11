# Building Guide for Ameba AWS Solution

This general guide provides an overview of the AWS integration process and serves as a foundational resource for understanding how to build AWS on various ICs supported by Ameba.

Before building and running the example, please follow the [AWS IoT Setup Guidance](aws_setup_guide.md) to make sure the AWS IoT resources has been setup.

- [Building Guides for Specific ICs](#building-guides-for-specific-ics)
- [Building Different Applications](#building-different-applications)

## Building Guides for Specific ICs

This general guide provides an overview of the AWS integration process and serves as a foundational resource for understanding how to build AWS on various ICs supported by Ameba.

- [Ameba RTOS SDK Building Guide](ameba-rtos_general_build.md) - Step-by-step instructions for building AWS on Ameba RTOS SDK.

## Building Different Applications

### Ameba RTOS v1.2

Each IoT feature is provided as a standalone example. Use the `aws_build_proj` command followed by the specific device types applicable in Realtek’s SDK to initiate the build process.

The following commands represent the primary examples supported within the Realtek AWS SDK:

    # Build the MQTT Mutual Authentication demo
    aws_build_proj mqtt_mutual_auth

    # Build the HTTP Mutual Authentication demo
    aws_build_proj http_mutual_auth

    # Build the Device Shadow demo
    aws_build_proj device_shadow

    # Build the Device Defender demo
    aws_build_proj device_defender

    # Build the OTA (Over-the-Air) Update demos
    aws_build_proj ota_over_mqtt
    aws_build_proj ota_over_mqtt_streams

### Additional References

- For a detailed walkthrough of each example’s logic and implementation, please refer to [Realtek AWS Examples Guide](aws_examples_guide.md).
- For information regarding memory usage by the AWS application, please refer to [AWS Example Memory Usage](aws_memory_usage.md).
- If you encounter any issues during building or runtime, please refer to [Troubleshooting Guide for Ameba AWS Solution](aws_troubleshooting_guide.md).
