#ifndef EXAMPLE_AMAZON_FREERTOS_H
#define EXAMPLE_AMAZON_FREERTOS_H

#include "platform_autoconf.h"

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

#else
    #error "Please check relevant porting/legacy folder for specific flash offsets!"
#endif

#endif /* EXAMPLE_AMAZON_FREERTOS_H */
