AMAZON FREERTOS SDK EXAMPLE

Description:
Start to run Amazon FreeRTOS SDK on Ameba

Configuration:

1. Update the menuconfig to run Amazon FreeRTOS example
- Navigate to project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
- run `make menuconfig`
- Under Amazon FreeRTOS Config, enable CONFIG_AMAZON_FREERTOS_EN
- Under SSL Config, choose MBEDTLS_AMAZON_FREERTOS_DEFINED for the MBEDTLS Version

2. Update aws_platform_opts.h
- Navigate to component/common/application/amazon-freertos/ports/amebaD/config_files/aws_platform_opts.h
- By default, CONFIG_EXAMPLE_AMAZON_FREERTOS is set to 1.
- Enable ONLY ONE of the sub-example macro

3. Configure aws_clientcredential.h and aws_clientcredential_keys.h
Refer to Section “Configure Your Project” in https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_ti.html, which will have the instructions.

Execution:
The example will run demos defined in aws_main.c.

