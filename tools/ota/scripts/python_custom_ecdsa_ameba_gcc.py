import os
import array as arr
import subprocess
import argparse

def get_parser():
    parser = argparse.ArgumentParser("Python Custom EDCSA Ameba GCC Script")

    parser.add_argument("-o", "--ota-image", type=str, help="Path to ota image, e.g. ../../../build_RTL8721Dx/ota_all.bin")
    parser.add_argument("-a", "--app-image", type=str, help="Path to app image, e.g. ../../../build_RTL8721Dx/km0_km4_app.bin")

    return parser

def main():
    parser = get_parser()
    args = parser.parse_args()

    if args.ota_image is None:
        print("Missing -o/--ota-image argument")
        parser.print_help()
        return

    if args.app_image is None:
        print("Missing -a/--app-image argument")
        parser.print_help()
        return

    if os.path.exists(args.ota_image):
        pass
    else:
        print(f"{args.ota_image} does not exist")
        return

    if os.path.exists(args.app_image):
        pass
    else:
        print(f"{args.app_image} does not exist")
        return

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

    #version = 0xffffffff
    version = Major*1000000 + Minor*1000 + Build
    version_byte = version.to_bytes(4,'little')

    # fix ota_all header
    with open(f"{args.ota_image}", 'r+b') as f:
        f.seek(0)
        f.write(version_byte)
        print(f"Successfully modified {args.ota_image} version")

    #caculate signature and output to IDT-OTA-Signature
    subprocess.call(['sh', 'signer_gcc_ameba.sh', args.app_image])

if __name__ == "__main__":
    main()
