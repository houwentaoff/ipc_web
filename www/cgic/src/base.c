/*
 ****************************************************************************
 *
 ** \file      base.c
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

#include <base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#if 1
const char* const video_html = "<div id=\"video\">\n \
	<object classid=\"clsid:9BE31822-FDAD-461B-AD51-BE1D1C159921\"\n\
			width=\"1280\"\n\
			height=\"720\"\n\
			id=\"vlc\"\n\
			events=\"True\">\n\
			<param name=\"MRL\" value=\"\" >\n\
			<param name=\"ShowDisplay\" value=\"True\" >\n\
			<param name=\"AutoLoop\" value=\"False\" >\n\
			<param name=\"AutoPlay\" value=\"False\" >\n\
			<param name=\"Volume\" value=\"50\" >\n\
			<param name=\"toolbar\" value=\"true\" >\n\
			<param name=\"StartTime\" value=\"0\" >\n\
			<embed pluginspage=\"http://www.videolan.org\"\n\
				type=\"application/x-vlc-plugin\"\n\
				version=\"VideoLAN.VLCPlugin.2\"\n\
				width=\"1280\"\n\
				height=\"720\"\n\
				toolbar=\"true\"\n\
				text=\"Waiting for video\"\n\
				name=\"vlc\">\n\
			</embed>\n\
		</object>\n\
	</div>\n";
#endif
const char * const onload = "onload=\"javascript:OnLoadActiveX(window.location.host, %d, 1, 0, 1); \"";

const char * const head_html =  "<!DOCTYPE HTML PUBLIC  \"-//W3C//DTD HTML 4.0 Transitional//EN\"\"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\
<html>\
    <head>\
        <meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\">\
        <title>%s</title>\
            <link href=\"../css/goke_1.css\" rel=\"stylesheet\" type=\"text/css\" />\
            <script language=\"JavaScript\" src=\"../js/AJAXInteraction.js\"></script>\
            <script language=\"JavaScript\" src=\"../js/page.js\"></script>\
            <script language=\"JavaScript\" src=\"../js/style.js\"></script>\
    </head>\
    <body %s>\
        <div class=\"container\">\
            <div class=\"header\">\
                <div class=\"logo\">\
                    <img src=\"../img/goke.png\" name=\"banner\" width=\"181\" height=\"78\" id=\"banner\" />\
                </div>\
                <div class=\"title\">\
                    <h1 align=\"center\">网 络 摄 像 机 产 品 演 示</h1>\
                    <p align=\"center\">IPC Product demonstration</p>\
                </div>\
            </div>";
//common
#if 0  //tmp
const char * const nav="\
<div class=\"leftnav\">\
        <div class=\"menu_item\" id=\"fl_menu\">\
            <div class=\"menu\" style=\"none\">\
                <a href=\"/cgi-bin/demo.cgi?page=0&stream=0\" title=\"liveview\" class=\"menu_item\">liveview</a>\
            </div>\
        </div>\
</div>";
#else
const char * const nav="\
<div class=\"leftnav\">\
        <div class=\"menu_item\" id=\"fl_menu\">\
            <div class=\"menu\" style=\"none\">\
                <a href=\"/cgi-bin/demo.cgi?page=0&stream=0\" title=\"liveview\" class=\"menu_item\">liveview</a>\
                <a href=\"/cgi-bin/demo.cgi?page=2\" title=\"3A\"  class=\"menu_item\">3A</a>\
                <a href=\"/cgi-bin/demo.cgi?page=5\" title=\"vinvout\" class=\"menu_item\">vinvout</a>\
                <a href=\"/cgi-bin/demo.cgi?page=1\" title=\"enc\"  class=\"menu_item\">enc</a>\
                <a href=\"osd.html\" title=\"osd\"  class=\"menu_item\">osd</a>\
            </div>\
        </div>\
</div>";
#endif
const char * const activeX="\
<div class=\"activeX\" >\
    <object classid=\"CLSID:3BCDAA6A-7306-42FF-B8CF-BE5D3534C1E4\" codebase=\"http://\"+window.location.host+\"/activex/ambaWeb.cab#version=1,0,0,33\" width=\"800\" height=\"541\"		align=\"left\" id=\"GOKEIPCmrWebPlugIn1\">\
        <param name=\"_Version\" value=\"65536\" />\
        <param name=\"_ExtentX\" value=\"19045\" />\
        <param name=\"_ExtentY\" value=\"11478\" />\
        <param name=\"_StockProps\" value=\"0\" />\
        <embed src=\"65536\" width=\"800\" height=\"541\" align=\"left\" _version=\"65536\" _extentx=\"19045\" _extenty=\"11478\" _stockprops=\"0\"></embed>\
    </object>\
</div> \
";

//menu 1 2 3 4 5
//menu 1
const char * const liveviewcontent="   \
<div class=\"content\">\
            <div id=\"stream_id\" class=\"%d\"></div>\
			<div class=\"stream_selector\">\
				<a href=\"/cgi-bin/demo.cgi?page=0&stream=0\">stream1</a>\
				<a href=\"/cgi-bin/demo.cgi?page=0&stream=1\">stream2</a>\
				<a href=\"/cgi-bin/demo.cgi?page=0&stream=2\">stream3</a>\
				<a href=\"/cgi-bin/demo.cgi?page=0&stream=3\">stream4</a>\
			</div>\
        <div class=\"activeX\" >\
            <object classid=\"CLSID:3BCDAA6A-7306-42FF-B8CF-BE5D3534C1E4\" codebase=\"http://\"+window.location.host+\"/activex/ambaWeb.cab#version=1,0,0,33\" width=\"800\" height=\"541\"		align=\"left\" id=\"GOKEIPCmrWebPlugIn1\">\
                <param name=\"_Version\" value=\"65536\" />\
                <param name=\"_ExtentX\" value=\"19045\" />\
                <param name=\"_ExtentY\" value=\"11478\" />\
                <param name=\"_StockProps\" value=\"0\" />\
                <embed src=\"65536\" width=\"800\" height=\"541\" align=\"left\" _version=\"65536\" _extentx=\"19045\" _extenty=\"11478\" _stockprops=\"0\"></embed>\
            </object>\
        </div> \
        <div id=\"status\">&nbsp; </div>\
        <div class=\"viewright\">\
            <div class=\"action\">	\
                <p>\
                <input class=\"but\" id=\"Play\" type=\"button\" value=\"Play\"  onclick=\"javascript:PlayActiveX()\"/>\
                <input class=\"but\" id=\"Stop\" type=\"button\" value=\"Stop\"  onclick=\"javascript:StopActiveX()\"/>\
                </p>\
                <p>\
                <input class=\"but\" id=\"Record\" type=\"button\" value=\"Start Record\"  onclick=\"javascript:Record()\"/>\
                </p>\
        <label for=\"ChangeCBRAvgBps\">码率(kbps) :</label>\
<input type=\"text\" class=\"textinput\" id=\"ChangeCBRAvgBps\" value=\"%d\" maxlength=5 onkeypress=\"javascript:NumOnly()\" onkeydown=\"javascript:FlySet(document.getElementById('stream_id').className, \'ChangeCBRAvgBps\')\" />\
            </div>\
            <br />\
        </div>\
  </div>\
";
//menu2 
const char * const vivocontent= "\
<div class=\"content\">\
    <div id=\"status\">&nbsp; </div>\
        <div class=\"vinvout_right\">\
			<div class=\"title\">输出设置</div>\
            <div class=\"action\">		\
%s\
            </div>\
        </div>\
        <div class=\"vivout_right2\">\
			<div class=\"title\">输入设置</div>\
            <div class=\"action\">\
%s\
            </div>\
        </div>\
		<div class=\"vinvout_submit\">\
		    <input name=\"button\" type=\"button\" class=\"apply\" id=\"button\"  onclick=\"javascript:setCamBasic()\" value=\"确认\"/> \
			&nbsp;<input name=\"button\" type=\"button\" class=\"cancel\" id=\"button\"  onclick=\"javascript:showPage('csb')\" value=\"取消\"/>\
	  </div>\
    </div>\
 ";
const char * const enccontent= "\
<div class=\"content\">\
        <div id=\"status\">&nbsp; </div>\
        <div class=\"enc_right1\">\
            <div class=\"title\">\
            %s\
            </div>\
            <div class=\"action\">\
            %s\
                <div id=\"enc_h264\">\
                    <label  class=\"title\">H264</label>\
                    <script type=\"text/javascript\">\
                        function change(x)\
                        {\
                            var value = document.getElementById(x).value;\
                            document.getElementById(\"_\"+x).innerHTML = value;\
                        }\
                    </script>\
                    <div id=\"__pointa\">\
                          <label></label><label id=\"_pointa\">1</label><input id=\"pointa\" type=\"range\"  min=\"0\" max=\"255\" value=\"100\" onchange=\"change(this.id)\">\
                    </div>\
                    <div id=\"__pointb\">\
                         <label id=\"_pointb\">2</label><input type=\"range\" id=\"pointb\" min=\"0\" max=\"255\" value=\"100\" onchange=\"change(this.id)\">\
                    </div>\
                    <div id=\"__pointc\">\
                          <label id=\"_pointc\">3</label><input type=\"range\" id=\"pointc\" min=\"0\" max=\"255\" value=\"100\" onchange=\"change(this.id)\">\
                    </div>\
                    <div id=\"__pointd\">\
                          <label id=\"_pointd\">4</label><input type=\"range\" id=\"pointd\" min=\"0\" max=\"255\" value=\"100\" onchange=\"change(this.id)\">	\
                    </div>\
                    <div id=\"__pointe\">\
                          <label id=\"_pointe\">5</label><input type=\"range\" id=\"pointe\" min=\"0\" max=\"255\" value=\"100\" onchange=\"change(this.id)\">\
                    </div>\
                    <div id=\"__pointf\">\
                          <label id=\"_pointe\">6</label><input type=\"range\" id=\"pointe\" min=\"0\" max=\"255\" value=\"100\" onchange=\"change(this.id)\">\
                    </div>\
                </div>\
            </div>\ 
        </div>\
        <div class=\"enc_submit\">\
            <input name=\"button\" type=\"button\" class=\"apply\" id=\"button\"  onclick=\"javascript:setCamBasic()\" value=\"确认\"/> 			&nbsp;\
            <input name=\"button\" type=\"button\" class=\"cancel\" id=\"button\"  onclick=\"javascript:showPage(\'csb\')\" value=\"取消\"/>\
        </div>\
</div>\
";
const char * const body_html = "<body %s>";
const char * const foot_html = "\
<div class=\"footer\">\
        <div >\
            <p>&nbsp;</p>\
            <p>公司地址：长沙市长星路389号 联系电话：0731-88218855 官方网站:www.gokemicro.com </p>\
            <p>国科微电子有限公司 版权所有</p>\
        </div>\
    </div>\
</div>\
</body>\
</html>";

//3 str need to insert.
const char*  const head_footHtml = "<!DOCTYPE HTML PUBLIC  \"-//W3C//DTD HTML 4.0 Transitional//\
EN\"\"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n\
	<html>\n\
	<head>\n\
		<title>%s</title>\n\
		<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />\n\
		<script language=\"JavaScript\" src=\"../js/AJAXInteraction.js\"></script>\n\
		<script language=\"JavaScript\" src=\"../js/CVerpage.js\"></script>\n\
		<script language=\"JavaScript\" src=\"../js/style.js\"></script>\n\
		<link rel=\"stylesheet\" type=\"text/css\" href=\"../css/goke.css\"/>\n\
		<style type=\"text/css\"><!--\n\
		%s\n\
		--></style>\n\
	</head>\n\
	<body%s>\n\
		<div id=\"container\" class=\"subcont\">\n\
			<div id=\"top\">\n\
				<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" height=\"40\">\n\
					<tr>\n\
						<td align=\"center\" width=\"160\"><img src=\"../img/logo1.gif\" \
						alt=\"logo\" /></td>\n\
						<td align=\"center\" width=\"640\">&nbsp; &nbsp;<p class=\"style2\
						\">GOKE IPCam</p></td>\n\
					</tr>\n\
				</table>\n\
			</div>\n\
			%s\n\
		</div>\n\
		<div id=\"footer\">\n\
			Copyright &copy 2014 GOKE Inc. All right reserved.\n\
		</div>\n\
	</body>\n\
	</html>";

