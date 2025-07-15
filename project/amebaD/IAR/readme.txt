The IAR build files are still located inside the patch.

Please refer to docs/amebad_general_build.md for the build.

llhttp.c.patch is a patch for llhttp.c to remove the IAR compile error by casting (void*) when assigning a pointer to state->_span_cb0.

The llhttp.c is located at sdk-amebad/component/common/application/amazon-freertos/libraries/coreHTTP/source/dependency/3rdparty/llhttp/src/llhttp.c
