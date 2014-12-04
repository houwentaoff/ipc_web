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
            <script type=\"text/javascript\" src=\"../js/AJAXInteraction.js\"></script>\
            <script type=\"text/javascript\" src=\"../js/page.js\"></script>\
            <script type=\"text/javascript\" src=\"../js/style.js\"></script>\
            <script type=\"text/javascript\" src=\"../js/jquery.js\"></script>\
            <script type=\"text/javascript\" src=\"../js/osd.js\"></script>\
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
                <a href=\"/cgi-bin/demo.cgi?page=1&stream=0\" title=\"enc\"  class=\"menu_item\">enc</a>\
                <a href=\"/cgi-bin/demo.cgi?page=3&stream=0\" title=\"osd\"  class=\"menu_item\">osd</a>\
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
				<a href=\"/cgi-bin/demo.cgi?page=0&stream=0\">流1</a>\
				<a href=\"/cgi-bin/demo.cgi?page=0&stream=1\">流2</a>\
				<a href=\"/cgi-bin/demo.cgi?page=0&stream=2\">流3</a>\
				<a href=\"/cgi-bin/demo.cgi?page=0&stream=3\">流4</a>\
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
                     %s   %s\
                </div>\
            </div>\ 
        </div>\
        <div class=\"enc_submit\">\
            <input name=\"button\" type=\"button\" class=\"apply\" id=\"button\"  onclick=\"javascript:setEnc()\" value=\"确认\"/> 			&nbsp;\
            <input name=\"button\" type=\"button\" class=\"cancel\" id=\"button\"  onclick=\"javascript:showPage(\'enc\')\" value=\"取消\"/>\
        </div>\
</div>\
";

const char * const osdcontent= "\
<div class=\"content\">\
        <div id=\"status\">&nbsp; </div>\
        <div class=\"stream_selector\">\
            <a href=\"/cgi-bin/view.cgi?page=0\">stream 1</a>\
            <a href=\"/cgi-bin/view.cgi?page=1\">stream 2</a>\
            <a href=\"/cgi-bin/view.cgi?page=2\">stream 3</a>\
            <a href=\"/cgi-bin/view.cgi?page=3\">stream 4</a>\
        </div>\
    	<div id=\"canvasDiv\"></div>	\n\
   <script type=\"text/javascript\"> $(document).ready(function() {\n\
    	 prepareCanvas();\
});</script>	\n\
        <div class=\"viewright\">\
            <div class=\"action\">	\
                <div id=\"s0_text\">\
                    <label> Content (32 character):</label><input type=\"text\" class=\"textinput\" value=\"%s\" maxlength=31  /></div>\
                <div id=\"s0_text_color\">\
                    <label> RGB :0x</label><input type=\"text\" class=\"textinput\" value=\"%s\" maxlength=10  /></div>\
                <div id=\"s0_text_size\">\
                    <label> 大小 :</label><input type=\"text\" class=\"textinput\" value=\"%d\" maxlength=5  /></div>\
                <div id=\"s0_text_startx\">\
                    <label>x :</label><input type=\"text\" class=\"textinput\" value=\"%d\" width=\"30\" maxlength=4  /></div>\
                <div id=\"s0_text_starty\">\
                    <label>y :</label> <input type=\"text\" class=\"textinput\" value=\"%d\" width=\"30\" maxlength=4  /></div>\
                <div id=\"s0_text_boxw\">\
                    <label><br />宽度 :</label><input type=\"text\" class=\"textinput\" value=\"%d\" width=\"30\" maxlength=4  /></div>\
                <div id=\"s0_text_boxh\">\
                    <label><br />高度 :</label><input type=\"text\" class=\"textinput\" value=\"%d\" width=\"30\" maxlength=4  /></div>	\
                \
                <div id=\"s0_time_enable\">\
                    <label>是否显示时间</label><input type=\"checkbox\" value=\"%d\" onchange=\"addOSD('time', 0)\" /></div>\
                \
                <div id=\"s0_bmp_enable\">\
                    <label>是否显示图片</label><input type=\"checkbox\" value=\"%d\" onchange=\"addOSD('bmp', 0)\" /></div>\
                \
                <div id=\"pm_enable\">\
                    <label>是否遮挡</label><input type=\"checkbox\" value=\"%d\" onchange=\"addOSD('pm', 0)\" /></div>	\
                \
                <div id=\"pm_x\">\
                    <label>x :</label><input type=\"text\" class=\"textinput\" value=\"%d\" width=\"30\" maxlength=4  /></div>\
                <div id=\"pm_y\">\
                    <label>y :</label> <input type=\"text\" class=\"textinput\" value=\"%d\" width=\"30\" maxlength=4  /></div>\
                <div id=\"pm_w\">\
                    <label><br />宽度 :</label><input type=\"text\" class=\"textinput\" value=\"%d\" width=\"30\" maxlength=4  /></div>\
                <div id=\"pm_h\">\
                    <label><br />高度 :</label><input type=\"text\" class=\"textinput\" value=\"%d\" width=\"30\" maxlength=4  /></div>	\
                                \
                <div id=\"pm_color\">\
                    <label> RGB :0x</label><input type=\"text\" class=\"textinput\" value=\"%s\" maxlength=10  /></div>						\
                <div></div>\
                <div>\
                    <p>&nbsp;</p>\
                    <input name=\"button\" type=\"button\" class=\"apply\" id=\"button\"  onclick=\"javascript:setOSD()\" value=\"确认\"/> 			&nbsp;\
                    <input name=\"button\" type=\"button\" class=\"cancel\" id=\"button\"  onclick=\"javascript:showPage('osd')\" value=\"取消\"/>\
                </div>\
\
                <br />\
            </div>\
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

