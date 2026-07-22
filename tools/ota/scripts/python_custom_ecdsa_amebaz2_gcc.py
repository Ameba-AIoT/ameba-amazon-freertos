import os
import subprocess
import argparse

# AmebaZ2 AWS OTA image generator.
#
# The Z2 device verifies the signature over the firmware padded to a 4096-byte
# (flash sector) boundary, so this script zero-pads the image, then ECDSA-SHA256
# signs the padded image via signer_gcc_ameba.sh into IDT-OTA-Signature.

SECTOR = 4096

def get_parser():
    parser = argparse.ArgumentParser("Python Custom ECDSA AmebaZ2 GCC Script")

    parser.add_argument("-a", "--app-image", type=str,
                        help="Path to the Z2 firmware image to sign, e.g. "
                             "../../../../../../../project/realtek_amebaz2_v0_example/"
                             "GCC-RELEASE/application_is/Debug/bin/firmware_is.bin")
    parser.add_argument("-o", "--out-dir", type=str, default=None,
                        help="Optional, directory for the padded image (default: this script's directory, "
                             "alongside the generated IDT-OTA-Signature)")

    return parser

def main():
    parser = get_parser()
    args = parser.parse_args()

    if args.app_image is None:
        print("Missing -a/--app-image argument")
        parser.print_help()
        return

    if not os.path.exists(args.app_image):
        print(f"{args.app_image} does not exist")
        return

    # signer_gcc_ameba.sh reads its key and writes outputs relative to its own
    # directory, so run it from there.
    script_dir = os.path.dirname(os.path.abspath(__file__))

    out_dir = args.out_dir if args.out_dir is not None else script_dir
    if not os.path.exists(out_dir):
        print(f"{out_dir} does not exist")
        return

    # Zero-pad the firmware to a 4096-byte boundary.
    with open(args.app_image, 'rb') as f:
        fw_bin = f.read()

    fw_bin_size = len(fw_bin)
    fw_padding = SECTOR - (fw_bin_size % SECTOR)
    padded_fw = fw_bin + bytes([0] * fw_padding)

    stem = os.path.splitext(os.path.basename(args.app_image))[0]
    aws_ota_path = os.path.join(out_dir, f"{stem}_aws_ota.bin")

    with open(aws_ota_path, 'wb') as f:
        f.write(padded_fw)

    print(f"Original size : {fw_bin_size} bytes")
    print(f"Padding added : {fw_padding} bytes")
    print(f"Padded size   : {len(padded_fw)} bytes")
    print(f"Padded image  : {aws_ota_path}")

    # Sign the padded image -> IDT-OTA-Signature.
    subprocess.call(['sh', 'signer_gcc_ameba.sh', os.path.abspath(aws_ota_path)], cwd=script_dir)

    print(f"Signature written to {os.path.join(script_dir, 'IDT-OTA-Signature')}")

if __name__ == "__main__":
    main()
