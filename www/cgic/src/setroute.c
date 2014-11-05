/*
 ****************************************************************************
 *
 ** \file      setroute.c
 **
 ** \version   $Id$
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
#if 0
#define SERVER_NAME cgiServerName
#endif
#if 1
#define SERVER_NAME "bwcpn"
#endif

#include <include.h>

int cgiMain() {
    /* Send the content type, letting the browser know this is HTML */
    cgiHeaderContentType("text/html");

    AmbaCamBasicPage_process_PostData(); 

    return (0);
}
