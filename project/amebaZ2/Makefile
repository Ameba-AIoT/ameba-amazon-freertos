
.PHONY: amazon
amazon: toolchain
	@$(MAKE) -f $(AWS_FREERTOS_BUILDDIR)/lib_amazon.mk

.PHONY: is_amazon
is_amazon: toolchain
	@$(MAKE) -f application.is.amazon.mk

.PHONY: clean_amazon
clean_amazon:
	@$(MAKE) -f $(AWS_FREERTOS_BUILDDIR)/lib_amazon.mk clean
	@$(MAKE) -f application.is.amazon.mk clean
