/* 
 * Copyright (c) 2001-2011 TIBCO Software Inc. 
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: tibemsUtilities.h 54254 2011-07-06 18:07:14Z $
 * 
 */

#ifndef _INCLUDED_tibemsUtilities_h
#define _INCLUDED_tibemsUtilities_h

#ifdef __MVS__  /* TIBEMSOS_ZOS  */
#include <tibems.h>
#include <emsadmin.h>
#else
#include <tibems/tibems.h>
#include <tibems/emsadmin.h>
#endif

#if defined(_WIN64)

#  
#    include <windows.h>
#    include <process.h>

#  define THREAD_RETVAL         unsigned __stdcall
#  define THREAD_OBJ            HANDLE
#  define THREAD_CREATE(t,f,a)  t=(HANDLE)_beginthreadex(NULL,0,(f),(a),0,NULL)

#else

#  include <pthread.h>
#  include <errno.h>
#  define THREAD_RETVAL         void*
#  define THREAD_OBJ            pthread_t
#  define THREAD_CREATE(t,f,a)  pthread_create(&(t),NULL,(f),(a))

#endif

extern void
ThreadJoin(
    int              threads,
    THREAD_OBJ*      threadArray);

extern tibems_long currentMillis(void);

extern char* stringdup(const char* s);

extern void
sslUsage();

extern void
setSSLParams(
    tibemsSSLParams ssl_params,
    int argc, 
    char* argv[],
    char* *pk_password);

extern void
ldapUsage();

extern tibems_status
setLookupParams(
    tibemsLookupParams lookup_params,
    int argc, 
    char* argv[]);

#ifdef __MVS__  /* TIBEMSOS_ZOS  */
extern void createArgs(int* pargc,char*** pargv);
extern signed long int tibems_MVS_BreakFunction (void * pConsumer);
extern void tibems_print_sslParams(tibemsSSLParams parms);
#endif

#endif /* _INCLUDED_tibemsUtilities_h */
