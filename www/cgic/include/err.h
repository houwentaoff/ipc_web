/*
 ****************************************************************************
 *
 ** \file      err.h
 **
 ** \version   $Id: err.h 2451 2014-11-25 09:59:20Z houwentao $
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

#ifndef __ERR_H__
#define __ERR_H__

typedef enum GK_ErrorCode_t{
GK_CGI_NO_ERROR=0,
GK_CGI_ERROR,/*return by cgiMain */
GK_CGI_ARGV_ERROR,/* Page argv or other argv*/
GK_MEM_ERROR,
GK_MEM_OVERFLOW,
}GK_ErrorCode_e;

#endif//__ERR_H__

