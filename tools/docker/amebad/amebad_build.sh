#!/bin/bash

set -e

aws_examples=(
  "CONFIG_EXAMPLE_AMAZON_FREERTOS_MQTT_MUTUAL_AUTH"
  "CONFIG_EXAMPLE_AMAZON_FREERTOS_HTTP_MUTUAL_AUTH"
  "CONFIG_EXAMPLE_AMAZON_FREERTOS_DEVICE_SHADOW"
  "CONFIG_EXAMPLE_AMAZON_FREERTOS_DEVICE_DEFENDER"
  "CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_MQTT"
  "CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_HTTP"
  "CONFIG_EXAMPLE_AMAZON_FREERTOS_OTA_OVER_MQTT_STREAMS"
  "CONFIG_EXAMPLE_AMAZON_FREERTOS_FLEET_PROVISIONING_KEYS_CERT"
  "CONFIG_EXAMPLE_AMAZON_FREERTOS_FLEET_PROVISIONING_CSR"
)

handle_error() {
  echo "Error: $1"
  exit 1
}

build_example() {
  aws_example=$1

  echo "Building AWS FreeRTOS with $aws_example enabled"
  make all || handle_error "Failed to build AWS FreeRTOS with $aws_example enabled"

  echo "Build AWS FreeRTOS with $aws_example enabled completed"
  make clean
}

if [ -z "$1" ]; then
  echo "Error: No base directory provided."
  echo "Usage: $0 <base_directory>"
  exit 1
fi

basedir=$1
amebadir=${basedir}/ameba-rtos-d
awsdir=${amebadir}/component/common/application/amazon-freertos
platform_opts=${awsdir}/ports/config_files/platform_opts_aws.h
tooldir=${awsdir}/tools

select_example() {
  for aws_example in "${aws_examples[@]}"; do
    if grep -q "^#define $aws_example[[:space:]][[:space:]]*1" "$platform_opts"; then
      if [ "$aws_example" == "$1" ]; then
        continue
      fi
    fi

    if [ "$aws_example" == "$1" ]; then
      sed -i "s/^#define $aws_example[[:space:]][[:space:]]*[01]*/#define $aws_example 1/" "$platform_opts"
    else
      if grep -q "^#define $aws_example[[:space:]][[:space:]]*1" "$platform_opts"; then
          sed -i "s/^#define $aws_example[[:space:]][[:space:]]*[01]*/#define $aws_example 0/" "$platform_opts"
      fi
    fi
  done
}

if [ ! -d "$amebadir" ]; then
  handle_error "Directory ${amebadir} does not exist."
fi

if [ ! -d "$awsdir" ]; then
  handle_error "Directory ${awsdir} does not exist."
fi

cp ${tooldir}/docker/amebad/platform_autoconf.h ${amebadir}/project/realtek_amebaD_va0_example/inc/inc_hp

cp ${tooldir}/docker/amebad/.config ${amebadir}/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/asdk/config

echo "Building project_hp"

cd "$amebadir/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp"
for aws_example in "${aws_examples[@]}"; do
  select_example $aws_example
  build_example $aws_example
done
