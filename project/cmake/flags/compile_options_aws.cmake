# add compile flags

# Compile Warnings
ameba_list_append(c_GLOBAL_COMMON_COMPILE_C_OPTIONS
	-Wno-undef
	-Wno-error
	-w
)
