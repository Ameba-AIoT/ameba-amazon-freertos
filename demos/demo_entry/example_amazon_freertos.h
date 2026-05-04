#ifndef EXAMPLE_AMAZON_FREERTOS_H
#define EXAMPLE_AMAZON_FREERTOS_H

void example_amazon_freertos(void);

#if defined(CONFIG_AMEBADPLUS)
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET     ( 0x001DB000 )
#define pkcs11OBJECT_CERT_FLASH_OFFSET       ( 0x001DC000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET   ( 0x001DD000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET    ( 0x001DE000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET ( 0x001DF000 ) //Flash location for code verify Key
#elif defined(CONFIG_AMEBALITE)
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET     ( 0x001DB000 )
#define pkcs11OBJECT_CERT_FLASH_OFFSET       ( 0x001DC000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET   ( 0x001DD000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET    ( 0x001DE000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET ( 0x001DF000 ) //Flash location for code verify Key
#elif defined(CONFIG_AMEBASMART)
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET     ( 0x002DB000 )
#define pkcs11OBJECT_CERT_FLASH_OFFSET       ( 0x002DC000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET   ( 0x002DD000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET    ( 0x002DE000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET ( 0x002DF000 ) //Flash location for code verify Key
#elif defined(CONFIG_AMEBAD)
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET     ( 0x001FB000 ) // 0x0810_0000 - 0x0810_2000-1
#define pkcs11OBJECT_CERT_FLASH_OFFSET       ( 0x001FC000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET   ( 0x001FD000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET    ( 0x001FE000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET ( 0x001FF000 ) //Flash location for code verify Key
#elif defined(CONFIG_AMEBAZ2)
#define AWS_OTA_IMAGE_STATE_FLASH_OFFSET     ( 0x00003000 ) // Flash reserved section 0x0000_3000 - 0x0000_4000-1
#define pkcs11OBJECT_CERT_FLASH_OFFSET       ( 0x001D2000 ) //Flash location for CERT
#define pkcs11OBJECT_PRIV_KEY_FLASH_OFFSET   ( 0x001D3000 ) //Flash location for Priv Key
#define pkcs11OBJECT_PUB_KEY_FLASH_OFFSET    ( 0x001D4000 ) //Flash location for Pub Key
#define pkcs11OBJECT_VERIFY_KEY_FLASH_OFFSET ( 0x001D5000 ) //Flash location for code verify Key
#endif

#endif /* EXAMPLE_AMAZON_FREERTOS_H */
