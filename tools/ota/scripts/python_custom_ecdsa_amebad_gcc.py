import os
import subprocess
import argparse

# AmebaD AWS OTA image generator.
#
# The ameba-rtos-d build has no ota_all.bin, only a raw km0_km4_image2.bin, so this
# script builds the 32-byte OTA header itself, then ECDSA-SHA256 signs the raw
# image via signer_gcc_ameba.sh into IDT-OTA-Signature.
#
# Header layout (little-endian, 4 bytes each):
#   version   = Major*1000000 + Minor*1000 + Build
#   headernum = 0x00000001
#   signature = 0x3141544f  ('OTA1' magic)
#   headerlen = 0x00000018  (24)
#   checksum  = sum of all image bytes
#   imagelen  = size of image
#   offset    = 0x00000020  (32)
#   rvsd      = 0x0800b000
#   <image bytes>

HEADER_NUM = 0x00000001
MAGIC      = 0x3141544f   # 'OTA1'
HEADER_LEN = 0x00000018   # 24
OFFSET     = 0x00000020   # 32
RVSD       = 0x0800b000

def get_parser():
    parser = argparse.ArgumentParser("Python Custom ECDSA AmebaD GCC Script")

    parser.add_argument("-a", "--app-image", type=str,
                        help="Path to the AmebaD image to wrap and sign, e.g. "
                             "../../../../../../../project/realtek_amebaD_va0_example/"
                             "GCC-RELEASE/project_hp/asdk/image/km0_km4_image2.bin")
    parser.add_argument("-o", "--out-dir", type=str, default=None,
                        help="Optional, output path for OTA_All.bin (default: this script's directory, "
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
    out_dir = args.out_dir if args.out_dir is not None else os.path.join(script_dir, "OTA_All.bin")

    Major = 0
    Minor = 0
    Build = 0

    with open('../../../ports/config_files/ota_demo_config.h') as f:
        for line in f:
            if line.find('define APP_VERSION_MAJOR') != -1:
                x = line.split()
                Major = int(x[2])
            if line.find('define APP_VERSION_MINOR') != -1:
                x = line.split()
                Minor = int(x[2])
            if line.find('define APP_VERSION_BUILD') != -1:
                x = line.split()
                Build = int(x[2])

    print('Major:' + str(Major))
    print('Minor:' + str(Minor))
    print('Build:' + str(Build))

    version = Major * 1000000 + Minor * 1000 + Build

    with open(args.app_image, 'rb') as f:
        img2_bin = f.read()

    checksum = sum(img2_bin) & 0xFFFFFFFF
    imagelen = len(img2_bin)

    # Write OTA_All.bin = header + raw image.
    with open(out_dir, 'wb') as f:
        f.write(version.to_bytes(4, 'little'))
        f.write(HEADER_NUM.to_bytes(4, 'little'))
        f.write(MAGIC.to_bytes(4, 'little'))
        f.write(HEADER_LEN.to_bytes(4, 'little'))
        f.write(checksum.to_bytes(4, 'little'))
        f.write(imagelen.to_bytes(4, 'little'))
        f.write(OFFSET.to_bytes(4, 'little'))
        f.write(RVSD.to_bytes(4, 'little'))
        f.write(img2_bin)

    print(f"Image size    : {imagelen} bytes")
    print(f"Checksum      : 0x{checksum:08x}")
    print(f"Version       : {version}")
    print(f"OTA image     : {out_dir}")

    # Sign the raw image -> IDT-OTA-Signature.
    subprocess.call(['sh', 'signer_gcc_ameba.sh', os.path.abspath(args.app_image)], cwd=script_dir)

    print(f"Signature written to {os.path.join(script_dir, 'IDT-OTA-Signature')}")

if __name__ == "__main__":
    main()
