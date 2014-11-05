/*
****************************************************************************
*
** \file      demo.c
**
** \version   $Id$
**
** \brief     ipc_web example
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

PageMapping pagemap[] = {
    {"view_page", view_page, NULL, NULL},
    {"enc_page", enc_page, NULL, NULL},
    {"pm_page", pm_page, NULL, NULL},
    {"osd_page", osd_page, NULL, NULL},
    {"sys_page", sys_page, NULL, NULL},
    {"vivo_page", vivo_page, NULL, NULL},
    {NULL, NULL, NULL, NULL},
};

#if 0
 int (*page_list[])()={
     NULL,
     view_page,
     enc_page,
     pm_page,
     osd_page,
     sys_page,
     vivo_page,

 };
#endif

static unsigned int page_value = ENCPAGE;/*默认的怎么用*/

static GK_ErrorCode_e switchPage ()
{
//	char* browser;
	int page_index;

	if (cgiFormIntegerBounded("page", &page_index, VIEWPAGE, PAGE_NUM-1, 0) != cgiFormSuccess)
    {
        PRT_ERR("index[%d]\n", page_index);
        return (GK_CGI_ARGV_ERROR);/*防止绕过密码直接操作,需要通过首页登陆才能进入*/
    }
    page_value = page_index;	
//	browser = getenv("HTTP_USER_AGENT");
//    fprintf(cgiOut, head_html, page_inf[page_index], "test page");
//    PRT_DBG ("page_index[%d]\nhead_html[%s]\nfun[0x%x]", page_index, head_html, (unsigned int)(page_list[page_value]));
//    (page_list[page_value])();
    pagemap[page_value].page();
 //   fprintf(cgiOut, "</body>%s", foot_html);

    FUN_OUT();

	return (0);
}


void CookieSet()
{
	char cname[1024];
	char cvalue[1024];
	/* Must set cookies BEFORE calling cgiHeaderContentType */
	cgiFormString("cname", cname, sizeof(cname));	
	cgiFormString("cvalue", cvalue, sizeof(cvalue));	
	if (strlen(cname)) {
		/* Cookie lives for one day (or until browser chooses
			to get rid of it, which may be immediately),
			and applies only to this script on this site. */	
		cgiHeaderCookieSetString(cname, cvalue,
			86400, cgiScriptName, SERVER_NAME);
//		cgiHeaderCookieSetString(cname, cvalue,
//			86400, "/", SERVER_NAME);
	}
}

int cgiMain() {
	/* Set any new cookie requested. Must be done *before*
		outputting the content type. */
//	CookieSet();
	/* Send the content type, letting the browser know this is HTML */
	cgiHeaderContentType("text/html");
	/* Top of the page */
//	fprintf(cgiOut, "<HTML><HEAD>\n");
//	fprintf(cgiOut, "<TITLE>cgic test</TITLE></HEAD>\n");
//	fprintf(cgiOut, "<BODY><H1>cgic test</H1>\n");
//	if ((cgiFormSubmitClicked("testcgic") == cgiFormSuccess))
//    {
//        Cookies();
//		fprintf(cgiOut, "<hr>\n");
//    }
//    ShowForm();  
    if (GK_CGI_NO_ERROR != switchPage())
    {
        return (GK_CGI_ERROR);
    }
	/* Finish up the page */
//	fprintf(cgiOut, "</BODY></HTML>\n");
    
	return (0);
}

