import os
import array as arr
import subprocess
import argparse

def get_parser():
    parser = argparse.ArgumentParser("Python Custom EDCSA Ameba GCC Script")

    parser.add_argument("-b", "--build-folder", type=str, help="Path to build folder, e.g ../../../build_RTL8721Dx")
    parser.add_argument("-a", "--app-image",    type=str, help="Path to image app, e.g. ../../../build_RTL8721Dx/km0_km4_app.bin")

    return parser

def main():
    parser = get_parser()
    args = parser.parse_args()

    if args.build_folder is None:
        print("Missing -b/--build-folder argument")
        parser.print_help()
        return

    if args.app_image is None:
        print("Missing -a/--app-image argument")
        parser.print_help()
        return

    if os.path.exists(args.build_folder):
        pass
    else:
        print(f"{args.build_folder} does not exist")
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
    with open(f"{args.build_folder}/ota_all.bin", 'r+b') as f:
        f.seek(0)
        f.write(version_byte)
        print("Successfully modified ota_all.bin version")

    #caculate signature and output to IDT-OTA-Signature
    subprocess.call(['sh', 'signer_gcc_ameba.sh', args.app_image])

if __name__ == "__main__":
    main()
