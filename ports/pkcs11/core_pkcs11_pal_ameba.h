#ifndef CORE_PKCS11_PAL_H
#define CORE_PKCS11_PAL_H

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey,
    eAwsClaimCertificate,
    eAwsClaimPrivateKey,
    eAwsJitpCertificate,
    eAwsRootCertificate,
    eAwsThingName,
};

#endif