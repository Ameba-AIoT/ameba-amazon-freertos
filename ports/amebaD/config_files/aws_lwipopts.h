/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

#ifndef LWIP_HDR_AWS_LWIPOPTS_H
#define LWIP_HDR_AWS_LWIPOPTS_H

/**
* For Amazon FreeRTOS usage
*/
#undef LWIP_COMPAT_MUTEX_ALLOWED
#define LWIP_COMPAT_MUTEX_ALLOWED

#undef ERRNO
#define ERRNO                      1

#undef LWIP_SO_SNDTIMEO
#define LWIP_SO_SNDTIMEO           1

#undef SYS_LIGHTWEIGHT_PROT
#define SYS_LIGHTWEIGHT_PROT       1

#undef SO_REUSE
#define SO_REUSE                   1

#undef LWIP_TCPIP_CORE_LOCKING
#define LWIP_TCPIP_CORE_LOCKING    1

#undef LWIP_SOCKET_SET_ERRNO
#define LWIP_SOCKET_SET_ERRNO      1

#undef LWIP_STATS
#define LWIP_STATS                 1

#undef MIB2_STATS
#define MIB2_STATS                 1
/**
* For Amazon FreeRTOS usage end
*/

#endif //LWIP_HDR_AWS_LWIPOPTS_H
