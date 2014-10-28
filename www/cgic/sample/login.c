/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Goke C&S.
 *       Filename:  login.c
 *
 *    Description:  作用:登录 加密 cookies 的应用
 *         Others:
 *   
 *        Version:  1.0
 *        Date:  Sunday, August 24, 2014 03:46:34 HKT
 *       Revision:  none
 *       Compiler:  arm-gcc
 *
 *         Author:  Sean , houwentaoff@gmail.com
 *   Organization:  Goke
 *        History:  Sunday, August 24, 2014 Created by SeanHou
 *
 * =====================================================================================
 */
#if 0
#define SERVER_NAME cgiServerName
#endif
#if 1
#define SERVER_NAME "bwcpn"
#endif

#include <cgic.h>

void Cookies()
{
    char **array, **arrayStep;
    char cname[1024], cvalue[1024];
    fprintf(cgiOut, "Cookies Submitted On This Call, With Values (Many Browsers NEVER Submit Cookies):<p>\n");
    if (cgiCookies(&array) != cgiFormSuccess) {
        return;
    }
    arrayStep = array;
    fprintf(cgiOut, "<table border=1>\n");
    fprintf(cgiOut, "<tr><th>Cookie<th>Value</tr>\n");
    while (*arrayStep) {
        char value[1024];
        fprintf(cgiOut, "<tr>");
        fprintf(cgiOut, "<td>");
        cgiHtmlEscape(*arrayStep);
        fprintf(cgiOut, "<td>");
        cgiCookieString(*arrayStep, value, sizeof(value));
        cgiHtmlEscape(value);
        fprintf(cgiOut, "\n");
        arrayStep++;
    }
    fprintf(cgiOut, "</table>\n");
    cgiFormString("cname", cname, sizeof(cname));   
    cgiFormString("cvalue", cvalue, sizeof(cvalue));    
    if (strlen(cname)) {
        fprintf(cgiOut, "New Cookie Set On This Call:<p>\n");
        fprintf(cgiOut, "Name: ");  
        cgiHtmlEscape(cname);
        fprintf(cgiOut, "Value: "); 
        cgiHtmlEscape(cvalue);
        fprintf(cgiOut, "<p>\n");
        fprintf(cgiOut, "If your browser accepts cookies (many do not), this new cookie should appear in the above list the next time the form is submitted.<p>\n"); 
    }
    cgiStringArrayFree(array);
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
//      cgiHeaderCookieSetString(cname, cvalue,
//          86400, "/", SERVER_NAME);
    }
}
void ShowForm()
{
//  fprintf(cgiOut, "<!-- 2.0: multipart/form-data is required for file uploads. -->");
    fprintf(cgiOut, "<form method=\"POST\" enctype=\"multipart/form-data\" ");
    fprintf(cgiOut, "   action=\"");/*在 HTML5 中，action 属性不再是必需的。*/
    cgiValueEscape(cgiScriptName);
    fprintf(cgiOut, "\">\n");
   
    fprintf(cgiOut, "<p>Set a Cookie<p>\n");
    fprintf(cgiOut, "<input type=\"text\" name=\"cname\" value=\"\"> Cookie Name\n");
    fprintf(cgiOut, "<input type =\"password\" name=\"cvalue\" value=\"\"> Cookie Value<p>\n");
    fprintf(cgiOut, "<input type=\"submit\" name=\"testcgic\" value=\"Submit Request\">\n");
    fprintf(cgiOut, "</form>\n");

}
int cgiMain() {
    /* Set any new cookie requested. Must be done *before*
        outputting the content type. */
    CookieSet();
    /* Send the content type, letting the browser know this is HTML */
    cgiHeaderContentType("text/html");
    /* Top of the page */
    fprintf(cgiOut, "<HTML><HEAD>\n");
    fprintf(cgiOut, "<TITLE>cgic test</TITLE></HEAD>\n");
    fprintf(cgiOut, "<BODY><H1>cgic test</H1>\n");
    if ((cgiFormSubmitClicked("testcgic") == cgiFormSuccess))
    {
        Cookies();
        fprintf(cgiOut, "<hr>\n");
    }
    ShowForm();  
    /* Finish up the page */
    fprintf(cgiOut, "</BODY></HTML>\n");
    
    return 0;
}

