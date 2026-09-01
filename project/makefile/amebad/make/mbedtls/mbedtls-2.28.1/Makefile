include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR            = $(AMAZON_FREERTOS_DIR)/libraries/3rdparty/mbedtls-2.28.1/library
MBEDTLSRTK_DIR = $(AMAZON_FREERTOS_DIR)/libraries/3rdparty/mbedtls_rtk
RAMMAP_DIR     = $(BASEDIR)/component/common/network/ssl/ssl_ram_map
vpath %.c $(DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

#mbedtls_library
CSRC += $(DIR)/aes.c
CSRC += $(DIR)/aesni.c
CSRC += $(DIR)/aria.c
CSRC += $(DIR)/asn1parse.c
CSRC += $(DIR)/asn1write.c
CSRC += $(DIR)/base64.c
CSRC += $(DIR)/bignum.c
CSRC += $(DIR)/camellia.c
CSRC += $(DIR)/ccm.c
CSRC += $(DIR)/chacha20.c
CSRC += $(DIR)/chachapoly.c
CSRC += $(DIR)/cipher.c
CSRC += $(DIR)/cipher_wrap.c
CSRC += $(DIR)/cmac.c
CSRC += $(DIR)/constant_time.c
CSRC += $(DIR)/ctr_drbg.c
CSRC += $(DIR)/debug.c
CSRC += $(DIR)/des.c
CSRC += $(DIR)/dhm.c
CSRC += $(DIR)/ecdh.c
CSRC += $(DIR)/ecdsa.c
CSRC += $(DIR)/ecjpake.c
CSRC += $(DIR)/ecp.c
CSRC += $(DIR)/ecp_curves.c
CSRC += $(DIR)/entropy.c
CSRC += $(DIR)/entropy_poll.c
CSRC += $(DIR)/error.c
CSRC += $(DIR)/gcm.c
CSRC += $(DIR)/hkdf.c
CSRC += $(DIR)/hmac_drbg.c
CSRC += $(DIR)/md.c
CSRC += $(DIR)/md5.c
CSRC += $(DIR)/memory_buffer_alloc.c
CSRC += $(DIR)/nist_kw.c
CSRC += $(DIR)/oid.c
CSRC += $(DIR)/padlock.c
CSRC += $(DIR)/pem.c
CSRC += $(DIR)/pk.c
CSRC += $(DIR)/pk_wrap.c
CSRC += $(DIR)/pkcs12.c
CSRC += $(DIR)/pkcs5.c
CSRC += $(DIR)/pkparse.c
CSRC += $(DIR)/pkwrite.c
CSRC += $(DIR)/platform.c
CSRC += $(DIR)/platform_util.c
CSRC += $(DIR)/poly1305.c
CSRC += $(DIR)/ripemd160.c
CSRC += $(DIR)/rsa.c
CSRC += $(DIR)/sha1.c
CSRC += $(DIR)/sha256.c
CSRC += $(DIR)/sha512.c
CSRC += $(DIR)/ssl_cache.c
CSRC += $(DIR)/ssl_ciphersuites.c
CSRC += $(DIR)/ssl_cookie.c
CSRC += $(DIR)/ssl_msg.c
CSRC += $(DIR)/ssl_ticket.c
CSRC += $(DIR)/ssl_tls.c
CSRC += $(DIR)/threading.c
CSRC += $(DIR)/timing.c
CSRC += $(DIR)/version.c
CSRC += $(DIR)/version_features.c
CSRC += $(DIR)/x509.c
CSRC += $(DIR)/x509_create.c
CSRC += $(DIR)/x509_crl.c
CSRC += $(DIR)/x509_crt.c
CSRC += $(DIR)/x509_csr.c
CSRC += $(DIR)/x509write_crt.c
CSRC += $(DIR)/x509write_csr.c

CSRC += $(DIR)/arc4.c
CSRC += $(DIR)/blowfish.c
CSRC += $(DIR)/certs.c
CSRC += $(DIR)/havege.c
CSRC += $(DIR)/md2.c
CSRC += $(DIR)/md4.c
CSRC += $(DIR)/pkcs11.c
CSRC += $(DIR)/rsa_internal.c
CSRC += $(DIR)/ssl_cli.c
CSRC += $(DIR)/ssl_srv.c
CSRC += $(DIR)/xtea.c

CSRC += $(MBEDTLSRTK_DIR)/net_sockets.c
#mbedtls_ssl_ram_map
CSRC += $(RAMMAP_DIR)/ssl_ram_map.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS =  $(CSRC:.c=.o)

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
all: CORE_TARGETS COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d
	$(REMOVE) *.c

-include $(DEPS)
