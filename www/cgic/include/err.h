/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Goke.
 *       Filename:  err.h
 *
 *    Description:  
 *         Others:
 *   
 *        Version:  1.0
 *        Date:  2014/8/26 19:48:31
 *       Revision:  none
 *       Compiler:  xxx-gcc
 *
 *         Author:  Sean Hou , houwentaoff@gmail.com
 *   Organization:  Goke
 *        History:   Created by housir
 *
 * =====================================================================================
 */
#ifndef __ERR_H__
#define __ERR_H__

typedef enum GK_ErrorCode_t{
GK_CGI_NO_ERROR=0,
GK_CGI_ERROR,/*return by cgiMain */
GK_CGI_ARGV_ERROR,/* Page argv or other argv*/
GK_MEM_ERROR,

}GK_ErrorCode_e;

#endif//__ERR_H__

