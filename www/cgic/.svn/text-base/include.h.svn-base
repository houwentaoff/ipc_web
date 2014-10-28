/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Goke.
 *       Filename:  include.h
 *
 *    Description:  
 *         Others:
 *   
 *        Version:  1.0
 *        Date:  2014/8/26 19:39:42
 *       Revision:  none
 *       Compiler:  xxx-gcc
 *
 *         Author:  Sean Hou , houwentaoff@gmail.com
 *   Organization:  Goke
 *        History:   Created by housir
 *
 * =====================================================================================
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

/* #####   DEBUG MACROS   ########################################################### */
#define     INOUT_DEBUG                          1  /*1 is open , o is close */
#define     DEMO_DEBUG                           1  /*1 is open , 0 is close */
#define     DEMO_ERROR                           1  /*1 is open , 0 is close */

/* #####   EXPORTED MACROS   ######################################################## */
#if CGICDEBUG
#define CGICDEBUGSTART \
	{ \
		FILE *dout; \
		dout = fopen("/tmp/demo_debug", "a"); \
	
#define CGICDEBUGEND \
		fclose(dout); \
	}
#else /* CGICDEBUG */
#define CGICDEBUGSTART
#define CGICDEBUGEND
#endif /* CGICDEBUG */

#if DEMO_ERROR
#define PRINT_ERR(fmt, args...)                                                                     \
do                                                                                                  \
{                                                                                                   \
    FILE *dout;                                                                                     \
    dout = fopen("/tmp/demo_debug", "a");                                                           \
    fprintf(dout, "\\033[5;41;32m [ERROR] ---> %s ():line[%d]:\\033[0m\n", __func__, __LINE__);     \
    fprintf(dout, " "fmt, ##args);                                                                  \
    fclose(dout);                                                                                   \
}while(0)/*DEMO_ERROR */
#else
#define PRINT_ERR(fmt, args...)
#endif/* */

#if DEMO_DEBUG
#define PRINT_DBG(fmt, args...)           \
    { \
		FILE *dout; \
		dout = fopen("/tmp/demo_debug", "a"); \
        fprintf(dout, "Sean ===> %s():"fmt"\n", __func__, ##args); \
		fclose(dout); \
	}
#else
#define PRINT_DBG(fmt, args...)
#endif/* DEMO_DEBUG */

#if INOUT_DEBUG
#define FUN_IN(fmt, args...) \
	{ \
		FILE *dout; \
		dout = fopen("/tmp/demo_debug", "a"); \
        fprintf(dout, "Sean ===> %s():"fmt"\n", __func__, ##args); \
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
        fprintf(dout, "Sean <=== %s():"fmt"\n", __func__, ##args); \
		fclose(dout); \
	}
#else /* INOUT_DEBUG */
#define FUN_OUT(fmt, args...)
#endif /* INOUT_DEBUG */

#endif//__include_h__
