/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

#ifndef AWS_FREERTOS_CONFIG_H
#define AWS_FREERTOS_CONFIG_H

/**
* For Amazon FreeRTOS usage
*/
#if (__IASMARM__ != 1)
#include "diag.h"
extern void cli(void);

/* Map the FreeRTOS printf() to the logging task printf. */
    /* The function that implements FreeRTOS printf style output, and the macro
     * that maps the configPRINTF() macros to that function. */
extern void vLoggingPrintf( const char * pcFormat, ... );
#define configPRINTF( X )								vLoggingPrintf X

/* Non-format version thread-safe print. */
extern void vLoggingPrint( const char * pcMessage );
#define configPRINT( X )								vLoggingPrint( X )

/* Map the logging task's printf to the board specific output function. */
#define configPRINT_STRING( x )							DiagPrintf( x )

/* Sets the length of the buffers into which logging messages are written - so
 * also defines the maximum length of each log message. */
#undef configLOGGING_MAX_MESSAGE_LENGTH
#define configLOGGING_MAX_MESSAGE_LENGTH				384
// #define configLOGGING_MAX_MESSAGE_LENGTH				512  //AFQP


/* Set to 1 to prepend each log message with a message number, the task name,
 * and a time stamp. */
#undef configLOGGING_INCLUDE_TIME_AND_TASK_NAME
#define configLOGGING_INCLUDE_TIME_AND_TASK_NAME	    1

#undef configSUPPORT_DYNAMIC_ALLOCATION
#define configSUPPORT_DYNAMIC_ALLOCATION				1

#undef configSUPPORT_STATIC_ALLOCATION
#define configSUPPORT_STATIC_ALLOCATION					1

#undef configUSE_MALLOC_FAILED_HOOK
#define configUSE_MALLOC_FAILED_HOOK					1

#define configPLATFORM_NAME "RealtekAmebaD"

#endif
/**
* For Amazon FreeRTOS usage end
*/

#endif //AWS_FREERTOS_CONFIG_H
