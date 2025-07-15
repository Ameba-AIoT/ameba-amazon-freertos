# #!/bin/bash

# Use the sign.sh script if you select custom code signing for OTA tests.

# openssl dgst -sha256 -sign C:/<absolute-path-to>/<privare-key-file> -out C:/<absolute-path-to>/<signature-destination> %1
# openssl base64 -A -in C:/<absolute-path-to>/<signature-destination> -out %2


keypath="ecdsa-sha256-signer.key.pem"
outsha256="km0_km4_image2_sig.bin"
image2="../../../../../../project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/asdk/image/km0_km4_image2.bin"
outsignature="IDT-OTA-Signature"

set echo off

openssl dgst -sha256 -sign $keypath -out $outsha256 $image2
openssl base64 -A -in $outsha256 -out $outsignature

# read -p "Press enter to continue"