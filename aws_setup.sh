#!/bin/bash

if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <path/to/ameba-rtos-sdk>"
  exit 1
fi

PATH_TO_AMEBA_AMAZON_FREERTOS="$PWD"
PATH_TO_AMEBA_RTOS_MATTER_SCRIPTS="$PATH_TO_AMEBA_AMAZON_FREERTOS/tools/scripts"

cd $1
PATH_TO_AMEBA_RTOS="$PWD"
AMEBA_ENV_SCRIPT="$PATH_TO_AMEBA_RTOS/env.sh"

AMAZON_FREERTOS_ENV_SCRIPT="$PATH_TO_AMEBA_AMAZON_FREERTOS/aws_env.sh"

create_amazon_freertos_env_script(){
  echo "Creating aws_env.sh"

  echo '#!/bin/bash' > "$AMAZON_FREERTOS_ENV_SCRIPT"
  echo '' >> "$AMAZON_FREERTOS_ENV_SCRIPT"

  if [ -e "$AMEBA_ENV_SCRIPT" ]; then
    echo "File $AMEBA_ENV_SCRIPT exist."
    echo "# Ameba Env Activate" >> "$AMAZON_FREERTOS_ENV_SCRIPT"
    echo "cd \"$PATH_TO_AMEBA_RTOS\"" >> "$AMAZON_FREERTOS_ENV_SCRIPT"
    echo "source \"$AMEBA_ENV_SCRIPT\"" >> "$AMAZON_FREERTOS_ENV_SCRIPT"
    echo '' >> "$AMAZON_FREERTOS_ENV_SCRIPT"
  else
    echo "File $AMEBA_ENV_SCRIPT does not exist."
  fi

  echo "# Additional Environment Configuration" >> "$AMAZON_FREERTOS_ENV_SCRIPT"
  echo "cd \"$PATH_TO_AMEBA_AMAZON_FREERTOS\"" >> "$AMAZON_FREERTOS_ENV_SCRIPT"
  echo "echo \"$PATH_TO_AMEBA_RTOS\" > .PATH_TO_AMEBA_RTOS" >> "$AMAZON_FREERTOS_ENV_SCRIPT"
  echo "export PATH=\"$PATH_TO_AMEBA_RTOS_MATTER_SCRIPTS:\$PATH\"" >> "$AMAZON_FREERTOS_ENV_SCRIPT"
  echo "chmod u+x \"$PATH_TO_AMEBA_RTOS_MATTER_SCRIPTS\"/aws_*" >> "$AMAZON_FREERTOS_ENV_SCRIPT"
}

echo "Configuring Ameba RTOS SDK"

create_amazon_freertos_env_script

cd "$PATH_TO_AMEBA_AMAZON_FREERTOS"

echo "Amazon FreeRTOS setup complete"
