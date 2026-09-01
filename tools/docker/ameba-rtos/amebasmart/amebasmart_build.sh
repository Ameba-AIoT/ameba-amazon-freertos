#!/bin/bash

set -e

PATH_TO_AMEBA_RTOS=$(cat .PATH_TO_AMEBA_RTOS)
AMEBA_PY="python $PATH_TO_AMEBA_RTOS/ameba.py"

aws_examples=(
  "mqtt_mutual_auth"
  "http_mutual_auth"
  "device_shadow"
  "device_defender"
  "ota_over_mqtt"
  "ota_over_http"
  "ota_over_mqtt_streams"
  "fleet_provisioning_keys_cert"
  "fleet_provisioning_csr"
)

handle_error() {
  echo "Error: $1"
  exit 1
}

build_example() {
  aws_example=$1

  echo "Building $aws_example example"
  aws_build_proj $aws_example || handle_error "Failed to build $aws_example"

  echo "Build $aws_example example completed"
  aws_clean_proj
}

if [ -z "$1" ]; then
  echo "Error: No base directory provided."
  echo "Usage: $0 <base_directory>"
  exit 1
fi

basedir=$1
amebadir=${basedir}/ameba-rtos
awsdir=${basedir}/ameba-amazon-freertos

if [ ! -d "$amebadir" ]; then
  handle_error "Directory ${amebadir} does not exist."
fi

if [ ! -d "$awsdir" ]; then
  handle_error "Directory ${awsdir} does not exist."
fi

cd ${awsdir}

source aws_env.sh

echo "Building firmware"

$AMEBA_PY soc RTL8730E

aws_apply_conf

for aws_example in "${aws_examples[@]}"; do
  build_example $aws_example
done
