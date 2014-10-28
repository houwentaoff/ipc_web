#!/bin/sh

streamid=$1
if [ -z "$1" ]; then
    streamid="0";
fi
echo "Content-type: text/html"
echo '
<!DOCTYPE HTML PUBLIC  "-//W3C//DTD HTML 4.0 Transitional//EN""-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html>
<head>
<title>GOKE IPCAM Live View</title>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />
<script language="JavaScript" src="../js/cs_AJAXInteraction.js"></script>
<script language="JavaScript" src="../js/cs_page.js"></script>
<script language="JavaScript" src="../js/cs_style.js"></script>
<link rel="stylesheet" type="text/css" href="../css/goke.css"/>
<style type="text/css">
<!--
#container, #top { width:1280px; }
#content { margin:0px; border-left:0px; border-right:0px; padding-left:0px; padding-right:0px }
#display { width:100%; height:780px; align:center; padding:0px; }
#videoforIE { margin:10px 0px 0px 0px;align:center;padding:0px;width:1280px;height:760px}
#action { margin:10px; width:100%; height:60px; align:left; }
.nav, .but { margin:3px; padding:5px; width:60px; vertical-align:middle; text-align:center;centerdisplay:inline; text-transform:capitalize; }
.but { width:auto; font-weight:bold; text-decoration:none; position:relative; }
.textbox { width:10%; text-align:center; }
.textinput { width:40px; }
.right { float:right; }
.nav1 {margin:3px; padding:5px; width:60px; vertical-align:middle; text-align:center;centerdisplay:inline; text-transform:capitalize; }

--></style>
</head>'

echo "<body onload=\"javascript:OnLoadActiveX(window.location.host, ${streamid}, 1, 0, 1);\" >"
echo '
<!-- <body onload="OnLoadActiveX('192.168.10.244', 0, 1, 0, 1);" > -->
<p class="style2">GOKE IPCam</p>
<div id="container" class="subcont">
<div id="wrap">
<div id="content">&nbsp;<label></label>
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp;
<a href="/cgi-bin/view.cgi?0">Main</a>
<a href="/cgi-bin/view.cgi?1">Second</a>
<a href="/cgi-bin/view.cgi?2">Third</a>
<a href="/cgi-bin/view.cgi?3">Fourth</a>
<div id="display">
<div id="videoforIE">
<OBJECT CLASSID="CLSID:3BCDAA6A-7306-42FF-B8CF-BE5D3534C1E4"codebase="http://"+window.location.host+"/activeX/GOKEWeb.cab#version=1,0,0,33" WIDTH=1280 HEIGHT=760align="absmiddle" ID="GOKEIPCmrWebPlugIn1">
<PARAM NAME="_Version" VALUE="65536">
<PARAM NAME="_ExtentX" VALUE="19045">
<PARAM NAME="_ExtentY" VALUE="11478">
<PARAM NAME="_StockProps" VALUE="0">
</OBJECT>
</div>
</div>
<div id="action"><input class="but" id="Play" type="button" value="Play" style=" " onclick="javascript:PlayActiveX()"/>
<input class="but" id="Stop" type="button" value="Stop"Stopstyle="" onclick="javascript:StopActiveX()"/>
<input name="button" type="button" class="but" id="Record" style="" onclick="javascript:Record()" value="Start Record"/>


</div>
</div>
<div class="clearingdiv">&nbsp; </div>
</div>

</div>
<div id="footer">
Copyright &copy 2014 GOKE Inc. All right reserved.
</div>
</body>
</html>
'

