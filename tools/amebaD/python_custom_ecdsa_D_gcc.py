from OpenSSL import crypto
from socket import gethostname
import os
import array as arr
import subprocess

Major = 0
Minor = 0
Build = 0
with open('../../ports/amebaD/config_files/ota_demo_config.h') as f:
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

headernum = 0x00000001
headernum_byte = headernum.to_bytes(4,'little')

signature = 0x3141544f
signature_byte = signature.to_bytes(4,'little')

headerlen = 0x00000018
headerlen_byte = headerlen.to_bytes(4,'little')

checksum = 0;
with open("../../../../../../project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/asdk/image/km0_km4_image2.bin", "rb") as f:
    byte = f.read(1)
    num = int.from_bytes(byte, 'big')
    checksum += num
    while byte != b"":
        byte = f.read(1)
        num = int.from_bytes(byte, 'big')
        checksum += num
checksum_byte = checksum.to_bytes(4,'little')

imagelen = os.path.getsize("../../../../../../project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/asdk/image/km0_km4_image2.bin")
imagelen_bytes = imagelen.to_bytes(4, 'little')

offset = 0x00000020
offset_bytes = offset.to_bytes(4, 'little')

rvsd = 0x0800b000
rvsd_bytes = rvsd.to_bytes(4, 'little')

img2_bin = open('../../../../../../project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/asdk/image/km0_km4_image2.bin', 'br').read()

f = open("./OTA_All.bin", 'wb')
f.write(version_byte)
f.write(headernum_byte)
f.write(signature_byte)
f.write(headerlen_byte)
f.write(checksum_byte)
f.write(imagelen_bytes)
f.write(offset_bytes)
f.write(rvsd_bytes)
f.write(img2_bin)
f.close()

#Reading the Private key generated using openssl(should be generated using ECDSA P256 curve)
#The key pair is just for demo
f = open("ecdsa-sha256-signer.key.pem")
pv_buf = f.read()
f.close()
priv_key = crypto.load_privatekey(crypto.FILETYPE_PEM, pv_buf)

#Reading the certificate generated using openssl(should be generated using ECDSA P256 curve)
f = open("ecdsa-sha256-signer.crt.pem")
ss_buf = f.read()
f.close()
ss_cert = crypto.load_certificate(crypto.FILETYPE_PEM, ss_buf)

#Reading OTA1 binary and individually signing it using the ECDSA P256 curve
ota1_bin = open('../../../../../../project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/asdk/image/km0_km4_image2.bin', 'br').read()
# sign and verify PASS
ota1_sig = crypto.sign(priv_key, ota1_bin, 'sha256')
crypto.verify(ss_cert, ota1_sig, ota1_bin, 'sha256')
ota1_sig_size = len(ota1_sig)
#print(ota1_sig_size)

#caculate signature and output to IDT-OTA-Signature
subprocess.call(['sh', './signer_gcc.sh'])

#remove temp files
os.remove("km0_km4_image2_sig.bin")


#Debug info in case you want to check the actual signature binaries generated separately
'''
sf = open("ota1.sig", 'wb')
sf.write(ota1_sig)
sf.close()

sf = open("ota2.sig", 'wb')
sf.write(ota2_sig)
sf.close()
'''