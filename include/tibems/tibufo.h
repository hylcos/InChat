/* 
 * Copyright (c) 2012-2012 TIBCO Software Inc. 
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: tibufo.h 64748 2012-12-06 22:24:33Z $
 * 
 */

#ifndef _INCLUDED_tibufo_h
#define _INCLUDED_tibufo_h

#ifdef __VMS
#include "emsvms.h"
#endif

#include "types.h"
#include "status.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern tibemsConnectionFactory
tibemsUFOConnectionFactory_Create(void);

extern tibemsConnectionFactory
tibemsUFOConnectionFactory_CreateFromConnectionFactory(
    tibemsConnectionFactory             emsFactory);

extern tibems_status
tibemsUFOConnectionFactory_RecoverConnection(
    tibemsConnectionFactory             factory,
    tibemsConnection                    ufoConnection);

#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_tibufo_h */
