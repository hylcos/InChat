/* 
 * Copyright (c) 2007-2012 TIBCO Software Inc. 
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: statdata.h 64748 2012-12-06 22:24:33Z $
 * 
 */

#ifndef _INCLUDED_tibems_statdata_h
#define _INCLUDED_tibems_statdata_h

#include "types.h"
#include "status.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern tibems_status 
tibemsStatData_GetByteRate(
    tibemsStatData              statData,
    tibems_long*                rate_size);

extern tibems_status 
tibemsStatData_GetMessageRate(
    tibemsStatData              statData,
    tibems_long*                rate_msgs);

extern tibems_status 
tibemsStatData_GetTotalBytes(
    tibemsStatData              statData,
    tibems_long*                size);

extern tibems_status 
tibemsStatData_GetTotalMessages(
    tibemsStatData              statData,
    tibems_long*                msgs);

#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_tibems_statdata_h */
