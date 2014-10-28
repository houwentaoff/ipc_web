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

const char* const video_html = "<div id=\"video\">\n\
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
	
const char * const head_html =  "<!DOCTYPE HTML PUBLIC  \"-//W3C//DTD HTML 4.0 Transitional//\
EN\"\"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n\
	<html>\n\
	<head>\n\
		<title>%s</title>\n\
		<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />\n\
		<script language=\"JavaScript\" src=\"../js/AJAXInteraction.js\"></script>\n\
		<script language=\"JavaScript\" src=\"../js/page.js\"></script>\n\
		<script language=\"JavaScript\" src=\"../js/style.js\"></script>\n\
		<link rel=\"stylesheet\" type=\"text/css\" href=\"../css/goke.css\"/>\n\
		<style type=\"text/css\"><!--\n\
		%s\n\
		--></style>\n\
	</head>\n";

const char * const body_html = "<body %s>";
const char * const foot_html = "<div id=\"footer\">\n\
			Copyright &copy 2014 GOKE Inc. All right reserved.\n\
                </div>\n\
	</body>\n\
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

