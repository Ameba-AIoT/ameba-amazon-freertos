include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
all:
	@make -C application all
ifeq ($(MBEDTLS_AMAZON_FREERTOS_ENABLE), y)
	@make -C mbedtls all
endif

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean:
	@make -C application clean
ifeq ($(MBEDTLS_AMAZON_FREERTOS_ENABLE), y)
	@make -C mbedtls clean
endif

-include $(DEPS)
