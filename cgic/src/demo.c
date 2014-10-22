/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Goke.
 *       Filename:  demo.c
 *
 *    Description:  ipc web的一个例子
 *         Others:
 *   
 *        Version:  1.0
 *        Date:  2014/8/26 16:45:58
 *       Revision:  none
 *       Compiler:  xxx-gcc
 *
 *         Author:  Sean Hou , houwentaoff@gmail.com
 *   Organization:  Goke
 *        History:   Created by housir
 *
 * =====================================================================================
 */
#if 0
#define SERVER_NAME cgiServerName
#endif
#if 1
#define SERVER_NAME "bwcpn"
#endif

#include <include.h>

 int (*page_list[])()={
     NULL,
     view_page,
     enc_page,
     pm_page,
     osd_page,
     sys_page,

 };


static unsigned int page_value = ENCPAGE;/*默认的怎么用*/

static GK_ErrorCode_e switchPage ()
{
//	char* browser;
    const char *page_inf[]={
        "",
        "view_page",
        "enc_page",
        "pm_page",
        "osd_page",
        "sys_page",
    };
    int page_index;

//	if (cgiFormIntegerBounded("page", &page_index, VIEWPAGE, sizeof(page_list)/sizeof(char *), 0) != cgiFormSuccess)
	if (cgiFormIntegerBounded("page", &page_index, VIEWPAGE, PAGE_NUM-1, 0) != cgiFormSuccess)
    {
        PRINT_ERR("==>%s:line[%d], page_index[%d]\n", __func__, __LINE__, page_index);
        return (GK_CGI_ARGV_ERROR);/*防止绕过密码直接操作,需要通过首页登陆才能进入*/
    }
    page_value = page_index;	
//	browser = getenv("HTTP_USER_AGENT");
    fprintf(cgiOut, head_html, page_inf[page_index], "test page");
    PRINT_DBG ("page_index[%d]\nhead_html[%s]\nfun[0x%x]", page_index, head_html, (unsigned int)(page_list[page_value]));
    (page_list[page_value])();
    fprintf(cgiOut, "</body>%s", foot_html);

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

