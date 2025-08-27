from cryptography import x509
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import ec, utils
from cryptography.hazmat.primitives.serialization import load_pem_private_key, load_pem_public_key
import os
import base64

# Function to read file content
def read_file(filename):
    with open(filename, 'rb') as f:
        return f.read()

# Reading the Private key
pv_buf = read_file("ecdsa-sha256-signer.key.pem")
priv_key = load_pem_private_key(pv_buf, password=None)

# Reading the certificate
ss_buf = read_file("ecdsa-sha256-signer.crt.pem")
ss_cert = x509.load_pem_x509_certificate(ss_buf)

# Reading and padding the firmware
fw_bin = read_file('../../../../../../project/realtek_amebaz2_v0_example/GCC-RELEASE/application_is/Debug/bin/firmware_is.bin')
fw_bin_size = len(fw_bin)
fw_padding = 4096 - (fw_bin_size % 4096)

# Padding with zeros
padded_fw = fw_bin + bytes([0] * fw_padding)

# Writing padded firmware
with open("firmware_is_pad.bin", 'wb') as f:
    f.write(padded_fw)

# Reading OTA1 binary
ota1_bin = read_file('firmware_is_pad.bin')
ota1_bin_size = len(ota1_bin)

# Signing the OTA1 binary
signature = priv_key.sign(
    ota1_bin,
    ec.ECDSA(hashes.SHA256())
)

# Verifying the signature
public_key = ss_cert.public_key()
public_key.verify(
    signature,
    ota1_bin,
    ec.ECDSA(hashes.SHA256())
)

ota1_sig_size = len(signature)

# Output signature to AWS-IoT OTA-Signature
ota_sig1_base64 = base64.b64encode(signature)
print(ota_sig1_base64)
with open("IDT-OTA-Signature", 'w', encoding='utf-8') as f:
    f.write(ota_sig1_base64.decode('utf-8'))

# Uncomment if you want to create firmware_is_sig.bin
'''
with open("firmware_is_sig.bin", 'wb') as f:
    f.write(ota1_bin_size.to_bytes(4, 'little'))
    f.write(ota1_bin)
    f.write(signature)
    f.write(bytes([ota1_sig_size]))
'''

# Uncomment if you want to remove the padded firmware file
# os.remove('firmware_is_pad.bin')
