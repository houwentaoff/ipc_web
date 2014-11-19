/*
 ****************************************************************************
 *
 ** \file      capture.c
 **
 ** \version   $Id: capture.c 2417 2014-11-19 09:11:29Z houwentao $
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

#include "cgic.h"

int cgiMain() {
	cgiWriteEnvironment("/tmp/capcgi.dat");
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<title>Captured</title>\n");
	fprintf(cgiOut, "<h1>Captured</h1>\n");
	fprintf(cgiOut, "Your form submission was captured for use in\n");
	fprintf(cgiOut, "debugging CGI code.\n");
	return (GK_CGI_NO_ERROR);
}

