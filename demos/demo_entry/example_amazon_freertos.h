#ifndef EXAMPLE_AMAZON_FREERTOS_H
#define EXAMPLE_AMAZON_FREERTOS_H

#if !defined(CONFIG_AMEBAZ2)
#include "platform_autoconf.h"
#endif

void example_amazon_freertos(void);

#if defined(CONFIG_AMEBADPLUS)
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET            ( 0x1DB000 )
#define pkcs11OBJECT_CERT_FLASH_OFFSET              ( 0x1DC000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET          ( 0x1DD000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET           ( 0x1DE000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET        ( 0x1DF000 ) //Flash location for code verify Key
#define pkcs11OBJECT_CLAIM_CERT_FLASH_OFFSET        ( 0x1E0000 )
#define pkcs11OBJECT_CLAIM_PRIV_KEY_FLASH_OFFSET    ( 0x1E1000 )
#define pkcs11OBJECT_JITP_CERT_FLASH_OFFSET         ( 0x1E2000 )
#define pkcs11OBJECT_THING_NAME_OFFSET              ( 0x1E3000 ) //Saved ThingName location if fleet provisioned
#elif defined(CONFIG_AMEBALITE)
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET            ( 0x1DB000 )
#define pkcs11OBJECT_CERT_FLASH_OFFSET              ( 0x1DC000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET          ( 0x1DD000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET           ( 0x1DE000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET        ( 0x1DF000 ) //Flash location for code verify Key
#define pkcs11OBJECT_CLAIM_CERT_FLASH_OFFSET        ( 0x1E0000 )
#define pkcs11OBJECT_CLAIM_PRIV_KEY_FLASH_OFFSET    ( 0x1E1000 )
#define pkcs11OBJECT_JITP_CERT_FLASH_OFFSET         ( 0x1E2000 )
#define pkcs11OBJECT_THING_NAME_OFFSET              ( 0x1E3000 ) //Saved ThingName location if fleet provisioned
#elif defined(CONFIG_AMEBASMART)
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET            ( 0x2DB000 )
#define pkcs11OBJECT_CERT_FLASH_OFFSET              ( 0x2DC000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET          ( 0x2DD000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET           ( 0x2DE000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET        ( 0x2DF000 ) //Flash location for code verify Key
#define pkcs11OBJECT_CLAIM_CERT_FLASH_OFFSET        ( 0x2E0000 )
#define pkcs11OBJECT_CLAIM_PRIV_KEY_FLASH_OFFSET    ( 0x2E1000 )
#define pkcs11OBJECT_JITP_CERT_FLASH_OFFSET         ( 0x2E2000 )
#define pkcs11OBJECT_THING_NAME_OFFSET              ( 0x2E3000 ) //Saved ThingName location if fleet provisioned
#elif defined(CONFIG_AMEBAL2)
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET            ( 0x1DB000 )
#define pkcs11OBJECT_CERT_FLASH_OFFSET              ( 0x1DC000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET          ( 0x1DD000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET           ( 0x1DE000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET        ( 0x1DF000 ) //Flash location for code verify Key
#define pkcs11OBJECT_CLAIM_CERT_FLASH_OFFSET        ( 0x1E0000 )
#define pkcs11OBJECT_CLAIM_PRIV_KEY_FLASH_OFFSET    ( 0x1E1000 )
#define pkcs11OBJECT_JITP_CERT_FLASH_OFFSET         ( 0x1E2000 )
#define pkcs11OBJECT_THING_NAME_OFFSET              ( 0x1E3000 ) //Saved ThingName location if fleet provisioned
#elif defined(CONFIG_AMEBAGREEN2)
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET            ( 0x1DB000 )
#define pkcs11OBJECT_CERT_FLASH_OFFSET              ( 0x1DC000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET          ( 0x1DD000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET           ( 0x1DE000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET        ( 0x1DF000 ) //Flash location for code verify Key
#define pkcs11OBJECT_CLAIM_CERT_FLASH_OFFSET        ( 0x1E0000 )
#define pkcs11OBJECT_CLAIM_PRIV_KEY_FLASH_OFFSET    ( 0x1E1000 )
#define pkcs11OBJECT_JITP_CERT_FLASH_OFFSET         ( 0x1E2000 )
#define pkcs11OBJECT_THING_NAME_OFFSET              ( 0x1E3000 ) //Saved ThingName location if fleet provisioned
#elif defined(CONFIG_AMEBAD)
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET            ( 0x1FB000 ) // 0x0810_0000 - 0x0810_2000-1
#define pkcs11OBJECT_CERT_FLASH_OFFSET              ( 0x1FC000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET          ( 0x1FD000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET           ( 0x1FE000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET        ( 0x1FF000 ) //Flash location for code verify Key
#define pkcs11OBJECT_CLAIM_CERT_FLASH_OFFSET        ( 0x200000 )
#define pkcs11OBJECT_CLAIM_PRIV_KEY_FLASH_OFFSET    ( 0x201000 )
#define pkcs11OBJECT_JITP_CERT_FLASH_OFFSET         ( 0x202000 )
#define pkcs11OBJECT_THING_NAME_OFFSET              ( 0x203000 ) //Saved ThingName location if fleet provisioned
#elif defined(CONFIG_AMEBAZ2)
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET            ( 0x003000 ) // Flash reserved section 0x0000_3000 - 0x0000_4000-1
#define pkcs11OBJECT_CERT_FLASH_OFFSET              ( 0x1D2000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET          ( 0x1D3000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET           ( 0x1D4000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET        ( 0x1D5000 ) //Flash location for code verify Key
#define pkcs11OBJECT_CLAIM_CERT_FLASH_OFFSET        ( 0x1D6000 )
#define pkcs11OBJECT_CLAIM_PRIV_KEY_FLASH_OFFSET    ( 0x1D7000 )
#define pkcs11OBJECT_JITP_CERT_FLASH_OFFSET         ( 0x1D8000 )
#define pkcs11OBJECT_THING_NAME_OFFSET              ( 0x1D9000 ) //Saved ThingName location if fleet provisioned
#else
    #error "Please check relevant porting/legacy folder for specific flash offsets!"
#endif

#endif /* EXAMPLE_AMAZON_FREERTOS_H */
