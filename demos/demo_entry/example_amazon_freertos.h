#ifndef EXAMPLE_AMAZON_FREERTOS_H
#define EXAMPLE_AMAZON_FREERTOS_H

void example_amazon_freertos(void);

#if defined(CONFIG_AMEBADPLUS)
#define pkcs11OBJECT_CERT_FLASH_OFFSET       ( 0x1DC000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET   ( 0x1DD000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET    ( 0x1DE000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET ( 0x1DF000 ) //Flash location for code verify Key
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET             ( 0x1DB000 )
#elif defined(CONFIG_AMEBALITE)
#define pkcs11OBJECT_CERT_FLASH_OFFSET       ( 0x1DC000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET   ( 0x1DD000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET    ( 0x1DE000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET ( 0x1DF000 ) //Flash location for code verify Key
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET             ( 0x1DB000 )
#elif defined(CONFIG_AMEBASMART)
#define pkcs11OBJECT_CERT_FLASH_OFFSET       ( 0x2DC000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET   ( 0x2DD000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET    ( 0x2DE000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET ( 0x2DF000 ) //Flash location for code verify Key
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET             ( 0x2DB000 )
#endif

#endif /* EXAMPLE_AMAZON_FREERTOS_H */
