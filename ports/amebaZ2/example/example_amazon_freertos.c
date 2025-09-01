
#include <platform_opts.h>
#include <platform_stdlib.h>

#if (CONFIG_EXAMPLE_AMAZON_FREERTOS)

#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"

extern void vApplicationDaemonTaskStartupHook(void);

static void *app_mbedtls_calloc_func(size_t nelements, size_t elementSize)
{
	size_t size;
	void *ptr = NULL;

	size = nelements * elementSize;
	ptr = pvPortMalloc(size);

	if (ptr) {
		memset(ptr, 0, size);
	}

	return ptr;
}

void app_mbedtls_init(void)
{
	mbedtls_platform_set_calloc_free(app_mbedtls_calloc_func, vPortFree);
}

#if !defined(configSUPPORT_STATIC_ALLOCATION) || (configSUPPORT_STATIC_ALLOCATION != 1)
/* #define configSUPPORT_STATIC_ALLOCATION 1 */
#error configSUPPORT_STATIC_ALLOCATION must be defined in FreeRTOSConfig.h as 1.
#endif

extern int aws_main(void);

static void example_amazon_freertos_thread(void *pvParameters)
{
	/* Init AWS mbedTLS*/
	app_mbedtls_init();
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtw_create_secure_context(512);
#endif
	aws_main();
#if defined(configUSE_DAEMON_TASK_STARTUP_HOOK) && (configUSE_DAEMON_TASK_STARTUP_HOOK == 1)
	vApplicationDaemonTaskStartupHook();
#endif
	vTaskDelete(NULL);
	return;
}

void example_amazon_freertos(void)
{
	if (xTaskCreate(example_amazon_freertos_thread, ((const char *)"example_amazon_freertos_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\n\r%s xTaskCreate(example_amazon_freertos_thread) failed", __FUNCTION__);
	}
}

#endif // #if (CONFIG_EXAMPLE_AMAZON_FREERTOS)
