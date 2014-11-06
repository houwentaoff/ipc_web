/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Goke.
 *       Filename:  base.c
 *
 *    Description:  
 *         Others:
 *   
 *        Version:  1.0
 *        Date:  2014/8/26 16:35:29
 *       Revision:  none
 *       Compiler:  xxx-gcc
 *
 *         Author:  Sean Hou , houwentaoff@gmail.com
 *   Organization:  Goke
 *        History:   Created by housir
 *
 * =====================================================================================
 */
//#include <base.h>
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
const char * const onload = "\
onload=\"OnLoadActiveX(\'192.168.103.47\', 0, 1, 0, 1);\
 ";

const char * const head_html =  "<!DOCTYPE HTML PUBLIC  \"-//W3C//DTD HTML 4.0 Transitional//EN\"\"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\
<html>\
    <head>\
        <meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\">\
        <title>liveview</title>\
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
const char * const nav="\
<div class=\"leftnav\">\
        <div class=\"menu_item\" id=\"fl_menu\">\
            <div class=\"menu\" style=\"none\">\
                <a href=\"/cgi-bin/demo.cgi?page=\"liveview\" title=\"liveview\" target=\"_blank\" class=\"menu_item\">liveview</a>\
                <a href=\"/cgi-bin/demo.cgi?page=\"3A\" title=\"3A\" target=\"_blank\" class=\"menu_item\">3A</a>\
                <a href=\"/cgi-bin/demo.cgi?page=\"vinvout\" title=\"vinvout\" target=\"_blank\" class=\"menu_item\">vinvout</a>\
                <a href=\"enc.html\" title=\"enc\" target=\"_blank\" class=\"menu_item\">enc</a>\
                <a href=\"osd.html\" title=\"osd\" target=\"_blank\" class=\"menu_item\">osd</a>\
            </div>\
        </div>\
</div>";
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
const char * const liveviewcontent="\
<div class=\"content\">\
%s\
        <div class=\"right\">\
            <div class=\"action\">\
                <p>\
                <input class=\"but\" id=\"Play\" type=\"button\" value=\"Play\"  onclick=\"javascript:PlayVideo()\"/>\
                <input class=\"but\" id=\"Stop\" type=\"button\" value=\"Stop\"  onclick=\"javascript:StopVideo()\"/>\
                </p>\
                <p>\
                <input class=\"but\" id=\"Record\" type=\"button\" value=\"Start Record\"  onclick=\"javascript:Record()\"/>\
                <input class=\"but\" id=\"ForceIdr\" type=\"button\" value=\"Force Idr\"  onclick=\"javascript:FlySet(0, 'ForceIdr')\"/>\
                </p>\
                <!--     		<label for=\"ChangeFr\"><br />\
Encode frame rate (1~30) :</label>\
<select id=\"ChangeFr\" onkeydown=\"javascript:FlySet(0, 'ChangeFr')\">\
<option value=8533333 >60</option>\
<option value=17066667 selected>30</option>\
<option value=20480000 >25</option>\
<option value=25600000 >20</option>\
<option value=34133333 >15</option>\
<option value=51200000 >10</option>\
<option value=85333333 >6</option>\
<option value=102400000 >5</option>\
<option value=128000000 >4</option>\
<option value=170666667 >3</option>\
<option value=256000000 >2</option>\
<option value=512000000 >1</option>\
</select>\
<label for=\"ChangeVBRMinBps\" <?cs if:brcmode==\"CBR\" ?><?cs var:\"style=display:none\"?> <?cs /if ?>></label>\
        <p>&nbsp;</p>\
        <label for=\"ChangeCBRAvgBps\">CBR average bitrate(kbps) :</label>\
<input type=\"text\" class=\"textinput\" id=\"ChangeCBRAvgBps\" value=\"4000\" maxlength=5 onkeypress=\"javascript:NumOnly()\" onkeydown=\"javascript:FlySet(0, 'ChangeCBRAvgBps')\" />\
-->\
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
                <p>\
                <input class=\"but\" id=\"Play\" type=\"button\" value=\"Play\"  onclick=\"javascript:PlayVideo()\"/>\
                <input class=\"but\" id=\"Stop\" type=\"button\" value=\"Stop\"  onclick=\"javascript:StopVideo()\"/>\
                </p>\
                <p>\
                <input class=\"but\" id=\"Record\" type=\"button\" value=\"Start Record\"  onclick=\"javascript:Record()\"/>\
                <input class=\"but\" id=\"ForceIdr\" type=\"button\" value=\"Force Idr\"  onclick=\"javascript:FlySet(0, 'ForceIdr')\"/>\
                </p>\
            </div>\
            <br />\
        </div>\
        <div class=\"vivout_right2\">\
			<div class=\"title\">输入设置</div>\
            <div class=\"action\">\
                <p>\
                <input name=\"button\" type=\"button\" class=\"but\" id=\"button\"  onclick=\"javascript:PlayVideo()\" value=\"Play\"/>\
                <input name=\"button2\" type=\"button\" class=\"but\" id=\"button2\"  onclick=\"javascript:StopVideo()\" value=\"Stop\"/>\
                </p>\
                <p>\
%s\
\
                <input name=\"button2\" type=\"button\" class=\"but\" id=\"button3\"  onclick=\"javascript:Record()\" value=\"Start Record\"/>\
                <input name=\"button2\" type=\"button\" class=\"but\" id=\"button4\"  onclick=\"javascript:FlySet(0, 'ForceIdr')\" value=\"Force Idr\"/>\
                </p>\
          </div>\
            <br />\
        </div>\
		<div class=\"vinvout_submit\">\
		    <input name=\"button\" type=\"button\" class=\"apply\" id=\"button\"  onclick=\"javascript:setCamBasic()\" value=\"确认\"/> \
			&nbsp;<input name=\"button\" type=\"button\" class=\"cancel\" id=\"button\"  onclick=\"javascript:showPage('csb')\" value=\"取消\"/>\
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
char* const const head_footHtml = "<!DOCTYPE HTML PUBLIC  \"-//W3C//DTD HTML 4.0 Transitional//\
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

