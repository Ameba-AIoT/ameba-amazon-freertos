#include "ameba.h"
#include "os_wrapper.h"
#include "log.h"

extern void aws_main(void);

static void example_amazon_freertos_thread(void *param)
{
    (void)param;

    RTK_LOGI(NOTAG, "Starting aws_main()!\n");
    aws_main();

	rtos_task_delete(NULL);
	return;
}

void example_amazon_freertos(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_amazon_freertos"), example_amazon_freertos_thread, NULL, 2048 * 6, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(init_thread) failed", __FUNCTION__);
	}
}