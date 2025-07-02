
#include <platform_opts.h>

#if (CONFIG_EXAMPLE_AMAZON_FREERTOS)

#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"

extern int aws_main(void);

static void example_amazon_freertos_thread(void *pvParameters)
{
    aws_main();

    vTaskDelete(NULL);
    return;
}

void example_amazon_freertos(void)
{
    if(xTaskCreate(example_amazon_freertos_thread, ((const char*)"example_amazon_freertos_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
        printf("\n\r%s xTaskCreate(example_amazon_freertos_thread) failed", __FUNCTION__);
}

#endif // #if (CONFIG_EXAMPLE_AMAZON_FREERTOS)
