ameba_list_append(public_includes
    #Use relative path: from current CMakeLists.txt's dir
    ${AWS_DIR}
    ${AWS_THIRD_PARTY_DIR}
    ${AWS_THIRD_PARTY_DIR}/mbedtls_utils
)

if(CONFIG_MBEDTLS_AMAZON_DEFINED)
ameba_list_append(public_includes
    #Use relative path: from current CMakeLists.txt's dir
    ${AWS_MBEDTLS_DIR}/include
    ${AWS_MBEDTLS_DIR}/include/psa
    ${AWS_MBEDTLS_DIR}/include/mbedtls
    ${AWS_MBEDTLS_DIR}/library
    ${AWS_THIRD_PARTY_DIR}/mbedtls_config
)
endif()
