ameba_list_append(private_sources
    ${AWS_THIRD_PARTY_DIR}/mbedtls_rtk/net_sockets.c
    ${AWS_MBEDTLS_DIR}/library/aes.c
    ${AWS_MBEDTLS_DIR}/library/aesni.c
    #${AWS_MBEDTLS_DIR}/library/arc4.c #3.6
    ${AWS_MBEDTLS_DIR}/library/aria.c
    ${AWS_MBEDTLS_DIR}/library/asn1parse.c
    ${AWS_MBEDTLS_DIR}/library/asn1write.c
    ${AWS_MBEDTLS_DIR}/library/base64.c
    ${AWS_MBEDTLS_DIR}/library/bignum.c
    ${AWS_MBEDTLS_DIR}/library/bignum_core.c #3.6
    ${AWS_MBEDTLS_DIR}/library/block_cipher.c #3.6
    #${AWS_MBEDTLS_DIR}/library/blowfish.c #3.6
    ${AWS_MBEDTLS_DIR}/library/camellia.c
    ${AWS_MBEDTLS_DIR}/library/ccm.c
    #${AWS_MBEDTLS_DIR}/library/certs.c #3.6
    ${AWS_MBEDTLS_DIR}/library/chacha20.c
    ${AWS_MBEDTLS_DIR}/library/chachapoly.c
    ${AWS_MBEDTLS_DIR}/library/cipher.c
    ${AWS_MBEDTLS_DIR}/library/cipher_wrap.c
    ${AWS_MBEDTLS_DIR}/library/cmac.c
    ${AWS_MBEDTLS_DIR}/library/constant_time.c
    ${AWS_MBEDTLS_DIR}/library/ctr_drbg.c
    ${AWS_MBEDTLS_DIR}/library/debug.c
    ${AWS_MBEDTLS_DIR}/library/des.c
    ${AWS_MBEDTLS_DIR}/library/dhm.c
    ${AWS_MBEDTLS_DIR}/library/ecdh.c
    ${AWS_MBEDTLS_DIR}/library/ecdsa.c
    ${AWS_MBEDTLS_DIR}/library/ecjpake.c
    ${AWS_MBEDTLS_DIR}/library/ecp.c
    ${AWS_MBEDTLS_DIR}/library/ecp_curves.c
    ${AWS_MBEDTLS_DIR}/library/entropy.c
    ${AWS_MBEDTLS_DIR}/library/entropy_poll.c
    ${AWS_MBEDTLS_DIR}/library/error.c
    ${AWS_MBEDTLS_DIR}/library/gcm.c
    #${AWS_MBEDTLS_DIR}/library/havege.c
    ${AWS_MBEDTLS_DIR}/library/hkdf.c
    ${AWS_MBEDTLS_DIR}/library/hmac_drbg.c
    ${AWS_MBEDTLS_DIR}/library/md.c
    #${AWS_MBEDTLS_DIR}/library/md2.c #3.6
    #${AWS_MBEDTLS_DIR}/library/md4.c #3.6
    ${AWS_MBEDTLS_DIR}/library/md5.c
    ${AWS_MBEDTLS_DIR}/library/memory_buffer_alloc.c
    ${AWS_MBEDTLS_DIR}/library/mps_reader.c #3.6
    ${AWS_MBEDTLS_DIR}/library/mps_trace.c #3.6
    ${AWS_MBEDTLS_DIR}/library/nist_kw.c
    ${AWS_MBEDTLS_DIR}/library/oid.c
    ${AWS_MBEDTLS_DIR}/library/padlock.c
    ${AWS_MBEDTLS_DIR}/library/pem.c
    ${AWS_MBEDTLS_DIR}/library/pk.c
    ${AWS_MBEDTLS_DIR}/library/pk_ecc.c #3.6
    ${AWS_MBEDTLS_DIR}/library/pk_wrap.c
    #${AWS_MBEDTLS_DIR}/library/pkcs11.c #3.6
    ${AWS_MBEDTLS_DIR}/library/pkcs12.c
    ${AWS_MBEDTLS_DIR}/library/pkcs5.c
    ${AWS_MBEDTLS_DIR}/library/pkparse.c
    ${AWS_MBEDTLS_DIR}/library/pkwrite.c
    ${AWS_MBEDTLS_DIR}/library/platform.c
    ${AWS_MBEDTLS_DIR}/library/platform_util.c
    ${AWS_MBEDTLS_DIR}/library/poly1305.c

    ${AWS_MBEDTLS_DIR}/library/ripemd160.c
    ${AWS_MBEDTLS_DIR}/library/rsa.c
    ${AWS_MBEDTLS_DIR}/library/rsa_alt_helpers.c
    #${AWS_MBEDTLS_DIR}/library/rsa_internal.c #3.6
    ${AWS_MBEDTLS_DIR}/library/sha1.c 
    ${AWS_MBEDTLS_DIR}/library/sha3.c #3.6
    ${AWS_MBEDTLS_DIR}/library/sha256.c
    ${AWS_MBEDTLS_DIR}/library/sha512.c
    ${AWS_MBEDTLS_DIR}/library/ssl_cache.c
    ${AWS_MBEDTLS_DIR}/library/ssl_ciphersuites.c
    #${AWS_MBEDTLS_DIR}/library/ssl_cli.c
    ${AWS_MBEDTLS_DIR}/library/ssl_client.c #3.6
    ${AWS_MBEDTLS_DIR}/library/ssl_cookie.c
    ${AWS_MBEDTLS_DIR}/library/ssl_msg.c
    #${AWS_MBEDTLS_DIR}/library/ssl_srv.c
    ${AWS_MBEDTLS_DIR}/library/ssl_ticket.c
    ${AWS_MBEDTLS_DIR}/library/ssl_tls.c
    ${AWS_MBEDTLS_DIR}/library/ssl_tls12_client.c
    ${AWS_MBEDTLS_DIR}/library/ssl_tls12_client.c
    ${AWS_MBEDTLS_DIR}/library/ssl_tls13_client.c
    ${AWS_MBEDTLS_DIR}/library/ssl_tls13_generic.c
    ${AWS_MBEDTLS_DIR}/library/ssl_tls13_keys.c
    ${AWS_MBEDTLS_DIR}/library/ssl_tls13_server.c
    ${AWS_MBEDTLS_DIR}/library/threading.c
    ${AWS_MBEDTLS_DIR}/library/timing.c
    ${AWS_MBEDTLS_DIR}/library/version.c
    ${AWS_MBEDTLS_DIR}/library/version_features.c
    ${AWS_MBEDTLS_DIR}/library/x509.c
    ${AWS_MBEDTLS_DIR}/library/x509_create.c
    ${AWS_MBEDTLS_DIR}/library/x509_crl.c
    ${AWS_MBEDTLS_DIR}/library/x509_crt.c
    ${AWS_MBEDTLS_DIR}/library/x509_csr.c
    ${AWS_MBEDTLS_DIR}/library/x509write_crt.c
    ${AWS_MBEDTLS_DIR}/library/x509write_csr.c
    #${AWS_MBEDTLS_DIR}/library/xtea.c

    # ${AWS_THIRD_PARTY_DIR}/mbedtls_utils/mbedtls_error.c
    # ${AWS_THIRD_PARTY_DIR}/mbedtls_utils/mbedtls_utils.c

)
