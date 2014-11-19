/*
****************************************************************************
*
** \file      include.h
**
** \version   $Id: include.h 2411 2014-11-18 13:43:30Z houwentao $
**
** \brief     
**
** \attention THIS SAMPLE CODE IS PROVIDED AS IS. GOFORTUNE SEMICONDUCTOR
**            ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR 
**            OMMISSIONS.
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
****************************************************************************
*/

#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include <string.h>
#include <stdlib.h>
#include "err.h"
#include "cgic.h"
#include "page.h"
#include "params.h"
#include "base.h"
#include "utils.h"
#include "map.h"

/* #####   DEBUG MACROS   ########################################################### */
#define     INOUT_DEBUG                          1  /*1 is open , o is close */
#define     CGI_DEBUG                            1  /*1 is open , 0 is close */
#define     CGI_ERROR                            1  /*1 is open , 0 is close */

/* #####   EXPORTED MACROS   ######################################################## */

#if CGI_ERROR
#define PRT_ERR(fmt, args...)                                                                       \
do                                                                                                  \
{                                                                                                   \
    FILE *dout;                                                                                     \
    dout = fopen("/tmp/demo_debug", "a");                                                           \
    fprintf(dout, "\\033[5;41;32m [ERROR] ---> %s ():line[%d]:\\033[0m\n", __func__, __LINE__);     \
    fprintf(dout, " "fmt, ##args);                                                                  \
    fclose(dout);                                                                                   \
}while(0)/*DEMO_ERROR */
#else
#define PRT_ERR(fmt, args...)
#endif/* */

#if CGI_DEBUG
#define PRT_DBG(fmt, args...)           \
    { \
		FILE *dout; \
		dout = fopen("/tmp/demo_debug", "a"); \
        fprintf(dout, "===> %s():line[%d]:"fmt"\n", __func__, __LINE__, ##args); \
		fclose(dout); \
	}
#else
#define PRT_DBG(fmt, args...)
#endif/* CGI_DEBUG */

#if INOUT_DEBUG
#define FUN_IN(fmt, args...) \
	{ \
		FILE *dout; \
		dout = fopen("/tmp/demo_debug", "a"); \
        fprintf(dout, "===> %s():"fmt"\n", __func__, ##args); \
		fclose(dout); \
	}
#else /* INOUT_DEBUG */
#define  FUN_IN(fmt, args...)
#endif /* INOUT_DEBUG */

#if INOUT_DEBUG
#define FUN_OUT(fmt, args...) \
	{ \
		FILE *dout; \
		dout = fopen("/tmp/demo_debug", "a"); \
        fprintf(dout, "<=== %s():"fmt"\n", __func__, ##args); \
		fclose(dout); \
	}
#else /* INOUT_DEBUG */
#define FUN_OUT(fmt, args...)
#endif /* INOUT_DEBUG */

#endif//__include_h__
