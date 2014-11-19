/*
****************************************************************************
*
** \file      route.c
**
** \version   $Id: route.c 2417 2014-11-19 09:11:29Z houwentao $
**
** \brief     截取ajx数值并返回text使网页局部更新
**
** \attention THIS SAMPLE CODE IS PROVIDED AS IS. GOFORTUNE SEMICONDUCTOR
**            ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR 
**            OMMISSIONS.
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
****************************************************************************
*/

#include "include.h"

int cgiMain() 
{
    //1.set value 2.update web page
    //waiting msg
    //cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<aa>cccccccccccccccccc<bb>\n");

    return (GK_CGI_NO_ERROR);
}


