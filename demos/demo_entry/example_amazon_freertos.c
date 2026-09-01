#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBAGREEN2)
#include "ameba.h"
#include "os_wrapper.h"
#include "log.h"
#elif defined(CONFIG_AMEBAD) || defined(CONFIG_AMEBAZ2)
#include "platform_opts.h"
#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#endif

extern void aws_main(void);

static void example_amazon_freertos_thread(void *param)
{
    (void)param;

    printf("Starting aws_main()!\n");
    aws_main();

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBAGREEN2)
    rtos_task_delete(NULL);
#elif defined(CONFIG_AMEBAD) || defined(CONFIG_AMEBAZ2)
    vTaskDelete(NULL);
#endif
}

void example_amazon_freertos(void)
{
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBAGREEN2)
    if (rtos_task_create(NULL, ((const char *)"example_amazon_freertos"), example_amazon_freertos_thread, NULL, 12 * 1024, 1) != RTK_SUCCESS) {
#elif defined(CONFIG_AMEBAD) || defined(CONFIG_AMEBAZ2)
    if(xTaskCreate(example_amazon_freertos_thread, ((const char*)"example_amazon_freertos_thread"), 3 * 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
#endif
        printf("\n\r%s failed to create task", __FUNCTION__);
    }
}