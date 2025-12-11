# #!/bin/bash

# Use the sign.sh script if you select custom code signing for OTA tests.

# openssl dgst -sha256 -sign C:/<absolute-path-to>/<privare-key-file> -out C:/<absolute-path-to>/<signature-destination> %1
# openssl base64 -A -in C:/<absolute-path-to>/<signature-destination> -out %2


keypath="ecdsa-sha256-signer.key.pem"
outsha256="app_image_sig.bin"
image2="$1"
outsignature="IDT-OTA-Signature"

set echo off

openssl dgst -sha256 -sign $keypath -out $outsha256 $image2
openssl base64 -A -in $outsha256 -out $outsignature

# read -p "Press enter to continue"
