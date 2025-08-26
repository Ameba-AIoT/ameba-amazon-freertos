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
#undef  MEM_SIZE
#define MEM_SIZE                (10*1024)

#undef  PBUF_POOL_SIZE
#define PBUF_POOL_SIZE          30

#undef  IP_REASS_MAX_PBUFS
#define IP_REASS_MAX_PBUFS              30

#undef  TCP_WND
#define TCP_WND                 (4*TCP_MSS)

#undef  LWIP_STATS
#define LWIP_STATS 1

#undef  LWIP_SNMP
#define LWIP_SNMP                  LWIP_UDP

#undef  MIB2_STATS
#define MIB2_STATS                 LWIP_SNMP

#undef  LWIP_COMPAT_MUTEX_ALLOWED
#define LWIP_COMPAT_MUTEX_ALLOWED

#undef  ERRNO
#define ERRNO   1

#undef  LWIP_SO_SNDTIMEO
#define LWIP_SO_SNDTIMEO                1

#undef  SYS_LIGHTWEIGHT_PROT
#define SYS_LIGHTWEIGHT_PROT    1

#undef  SO_REUSE
#define SO_REUSE                        1

#undef  LWIP_TCPIP_CORE_LOCKING
#define LWIP_TCPIP_CORE_LOCKING         1

#undef  LWIP_SOCKET_SET_ERRNO
#define LWIP_SOCKET_SET_ERRNO           1
/**
* For Amazon FreeRTOS usage end
*/

#endif //LWIP_HDR_AWS_LWIPOPTS_H
