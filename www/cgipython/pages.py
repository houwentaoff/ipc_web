#!/usr/bin/env python

##############################################
#
# pages.py
#
# History:
#	2010/02/28 - [Jian Tang] Created this file
#
# Copyright (C) 2007 - 2010, Ambarella, Inc.
#
# All rights reserved. No Part of this file may be reproduced, stored
# in a retrieval system, or transmitted, in any form, or by any means,
# electronic, mechanical, photocopying, recording, or otherwise,
# without the prior consent of Ambarella, Inc.
#
##############################################

import cgitb
cgitb.enable()

import cgi
import os
from util import *
from params import *

PP = PARAM()

NUM = ['Main', 'Secondary', 'Third', 'Fourth']

class AmbaPage(object):
	''' This is Base class of AMBA pages '''

	header = 'Content-Type: text/html\n\n'
	pageURL = '/cgi-bin/webdemo.cgi'

	pageHtml = '''<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
	<html>
	<head>
		<title>%s</title>
		<meta http-equiv="content-type" content="text/html; charset=utf-8" />
		<script language="JavaScript" src="../js/AJAXInteraction.js"></script>
		<script language="JavaScript" src="../js/page.js"></script>
		<script language="JavaScript" src="../js/style.js"></script>
		<link rel="stylesheet" type="text/css" href="../css/amba.css"/>
		<style type="text/css"><!--
		%s
		--></style>
	</head>
	<body%s>
		<div id="container" class="subcont">
			<div id="top">
				<table border="0" cellpadding="0" cellspacing="0" height="40">
					<tr>
						<td align="center" width="160"><img src="../img/logo1.gif" alt="logo" /></td>
						<td align="center" width="640">&nbsp; &nbsp;<p class="style2">Ambarella IPCam</p></td>
					</tr>
				</table>
			</div>
			%s
		</div>
		<div id="footer">
			Copyright &copy 2011 Ambarella Inc. All right reserved.
		</div>
	</body>
	</html>
	'''

	def __init__(self, name):
		self.name = name
		self.title = ''
		self.css = ''
		self.bodyJS = ''
		self.content = ''
		self.version = '1,0,0,33'
		self.host = os.environ.get("HOST")
		self.trans = AmbaTransfer()
		self.recvType = 1
		if 0 == os.system("ps | grep tcp_stream_server | grep -v grep"):
			self.recvType = 0
		return

	def create_em_text(self, content, style='', name=''):
		idName = [' id="%s"' % name, ''][name == '']
		classStyle = [' class="%s"' % style, ''][style == '']
		text = '<p%s%s>%s</p>\n' \
			% (classStyle, idName, content)
		return text

	def create_select_label(self, label, name, options, value, action=''):
		text = '<label for="%s">%s</label><select id="%s" %s>\n' % (name, label, name, action)
		for item in options:
			sel = [' ', ' selected'][item[0] == value]
			text += '<option value=%d%s>%s</option>\n' % (item[0], sel, item[1])
		text += '</select>\n'
		return text

	def create_input_entry(self, name, value, maxlen, act):
		inputType = ["text", "hidden"][act == 'hidden']
		action = [act, ''][act == 'hidden']
		text = '<input type="%s" class="textinput" id="%s" value="%s" maxlength=%d %s />\n' \
			% (inputType, name, value, maxlen, action)
		return text

	def create_text_entry(self, label, name, value, maxlen, ro=''):
		text = '<label for="%s" id="%s_l">%s</label>\n' % (name, name, label)
		text += '<input type="text" class="textinput" id="%s" value="%s" maxlength=%d %s />\n' \
			% (name, value, maxlen, ro)
		return text

	def create_wide_text_entry(self, label, name, value, maxlen, ro=''):
		text = '<label for="%s" id="%s_l">%s</label>\n' % (name, name, label)
		text += '<input type="text" class="widetextinput" id="%s" value="%s" maxlength=%d %s />\n' \
			% (name, value, maxlen, ro)
		return text

	def create_checkbox(self, label, name, value, action=''):
		checked = ['', 'checked'][value == 1]
		text = '<input type="checkbox" id="%s" value="%s" %s %s />\n' \
			% (name, value, action, checked)
		text += '<label for="%s" id="%s_l">%s</label>\n' % (name, name, label)
		return text

	def create_radio_input(self, label, name, options, value):
		text = '<label for="%s">%s</label>\n' % (name, label)
		text += '<input type="radio" id="%s" style="visibility:hidden" />' % (name)
		for item in options:
			check = [' ', ' checked'][item[0] == value]
			text += '<input type="radio" name="n_%s" value=%d%s/>%s &nbsp; &nbsp; \n' \
				% (name, item[0], check, item[1])
		return text

	def get_section_param(self, sectionName, paramData, extroInfo = ''):
		recvData = self.trans.send_get_request('REQ_GET_PARAM', sectionName, extroInfo)
		retv = False
		if recvData :
			for item in recvData:
				if item in paramData:
					paramData[item] = recvData[item]
			retv = True
		return retv

	def get_params(self):
		return

	def add_css_style(self):
		return

	def add_body_JS(self):
		return

	def add_content(self):
		self.get_params()
		return

	def create_page_html(self):
		self.add_css_style()
		self.add_body_JS()
		self.add_content()
		html = self.pageHtml % (self.title, self.css, self.bodyJS, self.content)
		return html


class AmbaVideoPage(AmbaPage):
	''' This is Ambarella IPCAM Live Vide Page '''

	contentHtml = '''
		<div id="sitename" class="setting">
			<div id="title"></div>
		</div>
		<div id="wrap">
			<div id="content">%s</div>
			<div class="clearingdiv">&nbsp; </div>
		</div>'''

	cssStyle = '''
	#container, #top { width:1280px; }
	#content { margin:0px; border-left:0px; border-right:0px; padding-left:0px; padding-right:0px }
	#display { width:100%; height:780px; align:center; padding:0px; }
	#video { margin:10px 0px 0px 0px; align:center;padding:0px; width:1280px; height:760px; position:absolute;display:block;overflow:auto}
	#videoforIE { margin:10px 0px 0px 0px;align:center;padding:0px;width:1280px;height:760px;position:absolute;display:block}
	#action { margin:10px; width:100%; height:60px; align:left;}
	.nav, .but { margin:3px; padding:5px; width:60px; vertical-align:middle; text-align:center; display:inline; text-transform:capitalize; }
	.but { width:auto; font-weight:bold; text-decoration:none; position:relative; }
	.textbox { width:10%; text-align:center; }
	.textinput { width:40px; }
	.right { float:right; }
	'''

	liveHtml = '''%s\n
	<div id="display">
		<div id="video">
			<OBJECT CLASSID="clsid:9BE31822-FDAD-461B-AD51-BE1D1C159921" WIDTH=%d HEIGHT=%d style="margin:%dpx %dpx %dpx %dpx" align="absmiddle" ID="vlc" events="True">
			<param name="MRL" value="" >
			<param name="ShowDisplay" value="True" >
			<param name="AutoLoop" value="False" >
			<param name="AutoPlay" value="False" >
			<param name="Volume" value="50" >
			<param name="toolbar" value="true" >
			<param name="StartTime" value="0" >
			<EMBED pluginspage="http://www.videolan.org"
				type="application/x-vlc-plugin"
				version="VideoLAN.VLCPlugin.2"
				width="%d"
				height="%d"
				style="margin:%d 0 %d 0"
				toolbar="true"
				text="Waiting for video"
				name="vlc">
			</EMBED>
			</OBJECT>
		</div>
	'''
	liveHtmlforIE = '''\n
		<div id="videoforIE">
			<OBJECT CLASSID="CLSID:3BCDAA6A-7306-42FF-B8CF-BE5D3534C1E4" codebase="http://%s/activeX/ambaWeb.cab#version=%s" WIDTH=1280 HEIGHT=760 align="absmiddle" ID="AmbaIPCmrWebPlugIn1">
			<PARAM NAME="_Version" VALUE="65536">
			<PARAM NAME="_ExtentX" VALUE="19045">
			<PARAM NAME="_ExtentY" VALUE="11478">
			<PARAM NAME="_StockProps" VALUE="0">
			</OBJECT>
		</div>
	</div>
	<div id="action">\n%s
	</div>
	'''

	def __init__(self, name):
		super(AmbaVideoPage, self).__init__(name)
		self.title = 'Ambarella IPCAM Live View'
		self.width = 0
		self.height = 0
		return

	def create_button(self, name, value, exprop, action):
		text = '		<input class="but" id="%s" type="button" value="%s" %s onclick="javascript:%s"/>\n' \
			 % (name, value, exprop, action)
		return text

	def create_select_label(self, label, name, options, value, action=''):
		text = '''<label for="%s">%s</label><select id="%s" onkeydown="javascript:%s">\n''' % (name, label, name, action)
		for item in options:
			sel = [' ', ' selected'][item[0] == value]
			text += '<option value=%d%s>%s</option>\n' % (item[0], sel, item[1])
		text += '</select>\n'
		return text

	def create_text_entry(self, label, name, value, maxlen, action):
		text = '&nbsp; <label for="%s">%s</label>\n' % (name, label)
		text += '<input type="text" class="textinput" id="%s" value="%s" maxlength=%d onkeypress="javascript:NumOnly()" onkeydown="javascript:%s" />\n' \
				% (name, value, maxlen, action) 
		return text

	def add_settings(self):
		return

	def add_controls(self):
		return

	def add_css_style(self):
		super(AmbaVideoPage, self).add_css_style()
		self.css += self.cssStyle
		return

        def add_body_JS(self):
	#	self.bodyJS = ''' onload="javascript: " ''' \
		return

	def process_PostData(self):
		ret = 0
		fieldform = cgi.FieldStorage()
		if len(fieldform) == 0 :
			return ret

		req_cnt = 0
		if fieldform.getvalue('req_cnt'):
			req_cnt = int(fieldform.getvalue('req_cnt'));
			for i in range (0, req_cnt):
				req = 'req%d' % i
				info = 'info%d' % i
				data = 'data%d' % i
				req_value = fieldform.getvalue(req)
				info_value =  int(fieldform.getvalue(info))
				data_value = int(fieldform.getvalue(data))
				if req_value and (info_value or data_value):
					ret = self.trans.send_fly_request(req_value, info_value, data_value)
					if ret != 0 :
						return -1
				else :
					ret = -1
					break
		return req_cnt

	def add_content(self):
		super(AmbaVideoPage, self).add_content()
		settingText = self.add_settings()
		controlText = self.add_controls()
		if (self.width <= 1280) & (self.height <= 720):
			self.content = self.contentHtml % (self.liveHtml % (settingText, \
			self.width, self.height, (760-self.height)/2, (1280-self.width)/2, (760-self.height)/2, (1280-self.width)/2,\
			self.width, self.height, (760-self.height)/2, (760-self.height)/2\
			)+self.liveHtmlforIE % (self.host, self.version, controlText))
		else:
			self.content = self.contentHtml % (self.liveHtml % (settingText, 1280, 720, 20, 0, 20, 0, 1280, 720, 20, 20)+self.liveHtmlforIE % (self.host, self.version, controlText))
		return


class AmbaLiveViewPage(AmbaVideoPage):
	''' This page is Privacy Mask page '''

	def __init__(self, streamId = 0, statSize = 0):
		super(AmbaLiveViewPage, self).__init__('liveview')
		self.title = 'Ambarella IPCAM Live View'
		self.streamId = streamId
		self.statSize = statSize
		self.create_params()
		return

	def create_params(self):
		self.live_params = {}
		self.live_params['encode_type'] = 0
		self.live_params['encode_fps'] = 30
		self.live_params['encode_width'] = 1280
		self.live_params['encode_height'] = 720
		self.live_params['brc_mode'] = 0
		self.live_params['cbr_avg_bps'] = 4000000
		self.live_params['vbr_min_bps'] = 1000000
		self.live_params['vbr_max_bps'] = 6000000
		return

	def get_params(self):
		ret = self.process_PostData()
		if ret < 0:
			print "1:set params failed"
		elif ret > 0:
			print "0:set params succeeded"
		elif ret == 0:
			return self.get_section_param('LIVE', self.live_params, self.streamId)
		else :
			print "1:unexpected error %d" % ret
		return 0

	def add_stream_resolution(self):
		self.width = self.live_params['encode_width']
		self.height = self.live_params['encode_height']
		return

	def add_settings(self):
		self.add_stream_resolution()
		text = ''
		text += '&nbsp; &nbsp; <a class="nav" href="/cgi-bin/webdemo.cgi?page=enc">Setting</a>\n'
		text += '&nbsp;<a class="nav" href="/cgi-bin/help.cgi?topic=live_view" target="_blank" >Help</a>\n'
		text += '&nbsp; &nbsp; &nbsp; &nbsp; <label>Live Stream &nbsp; '
		for item in range(len(NUM)):
			sel = ['', ' active'][item == self.streamId]
			text += '''<a class="nav%s" href="/cgi-bin/live_view.cgi?streamId=%d">%s</a>\n''' \
				% (sel, item, NUM[item])
		text += '</label>\n'
		text += '&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; '
		text += '    <input class="but" id="z" type="button" value="Z" style="visibility:hidden" />\n'
		#text += self.create_button('Stat', 'Hide Statistics', '', "ShowStat()")
		#text += '&nbsp; &nbsp; &nbsp; &nbsp;'
		#text += self.create_button('DPTZ', 'Hide Digital PTZ', 'hidden', "ShowDPTZ()") + '<br>\n'
		return text

	def add_controls(self):
		text = ''
		text += self.create_button('Play', 'Play', '', "PlayVideo()")
		text += self.create_button('Stop', 'Stop', '', "StopVideo()")
		text += self.create_button('Record', 'Start Record', '', "Record()")
		text += self.create_button('ForceIdr', 'Force Idr', '', \
			"FlySet(%d, 'ForceIdr')" % (self.streamId))
		options = []
		fpsList = [60, 30, 25, 20, 15, 10, 6, 5, 4, 3, 2, 1]
		for item in fpsList:
			item = '%s' % item
			options.append([PP.fps[item], item])
		text += self.create_select_label('Encode frame rate (1~30) :', 'ChangeFr', options, \
			self.live_params['encode_fps'], "FlySet(%d, 'ChangeFr')" % (self.streamId))
		if self.live_params['encode_type'] == 1:	# H264
			options = [[PP.brc['cbr'], 'CBR'], [PP.brc['vbr'], 'VBR'], \
					[PP.brc['cbrq'], 'CBR (keep quality)'], \
					[PP.brc['vbrq'], 'VBR (keep quality)']]
#			text += self.create_select_label("BRC:", "ChangeBRC", options, \
#				self.live_params['brc_mode'], "FlySet(%d, 'ChangeBRC')" % (self.streamId))
			if self.live_params['brc_mode'] & 0x1 :
				text += self.create_text_entry('VBR min bitrate(kbps) :', 'ChangeVBRMinBps', \
					self.live_params['vbr_min_bps'] / 1000, 5, \
					"FlySet(%d, 'ChangeVBRMinBps')" % (self.streamId))
				text += self.create_text_entry('VBR max bitrate(kbps) :', 'ChangeVBRMaxBps', \
					self.live_params['vbr_max_bps'] / 1000, 5, \
					"FlySet(%d, 'ChangeVBRMaxBps')" % (self.streamId))
			else :
				text += self.create_text_entry('CBR average bitrate(kbps) :', 'ChangeCBRAvgBps', \
					self.live_params['cbr_avg_bps'] / 1000, 5, \
					"FlySet(%d, 'ChangeCBRAvgBps')" % (self.streamId))
#			text += self.create_button('Apply', 'Apply', '', "ApplyFlySetting(%d)" % (self.streamId))
		return text

	def add_body_JS(self):
		self.bodyJS = ''' onload="javascript: OnLoadActiveX('%s', %d, %d, %d, %d);" ''' % \
			(self.host, self.streamId, self.recvType, self.statSize, 1)
		#self.bodyJS += ''' onload="javascript: OnClose();" '''
		return

	def show_page(self):
		print self.create_page_html()
		return


class AmbaConfigPage(AmbaPage):
	''' This is Ambarella IPCAM Config Page '''

	contentHtml = '''
			<div id="sitename" class="setting">
				<div id="title"></div>
			</div>
			<div id="wrap">
				<div id="menunav"><p>
				%s</p>
				<p>&nbsp; </p>
				</div>
				<div id="content">
					<div id="status">&nbsp; </div>
					%s
				</div>
				<div class="clearingdiv">&nbsp; </div>
			</div>
		'''

	cssStyle = '''
		#container, #top { width:800px; }
		#content { margin:0 20px 0 200px; }
	'''

	menuMap = {}
	pageMap = {}

	def __init__(self, name, activeMenu, activeSubMenu = None):
		super(AmbaConfigPage, self).__init__(name)
		self.title = 'Ambarella IPCAM Setting'
		self.activeMenu = activeMenu
		self.activeSubMenu = activeSubMenu
		self.add_menu_info()
		self.add_page_info()
		return

	def add_menu_info(self):
		if self.menuMap.has_key(self.activeMenu):
			subMenu = self.menuMap[self.activeMenu]
		else:
			subMenu = []
		if self.activeSubMenu and \
			self.activeSubMenu not in subMenu:
			subMenu.append(self.activeSubMenu)
		self.menuMap[self.activeMenu] = subMenu
		return

	def get_page_key(self, menu, subMenu = None):
		if subMenu:
			key = '%s/%s' % (menu, subMenu)
		else:
			key = menu
		return key

	def add_page_info(self):
		pageKey = self.get_page_key(self.activeMenu, self.activeSubMenu)
		if not self.pageMap.has_key(pageKey):
			self.pageMap[pageKey] = self.name
		return

	def create_menu_item(self, page, menuName, status = None):
		if status:
			menuType = ' %s' % status
		else:
			menuType = ''
		line = '				<a class="nav%s" href="%s?page=%s">%s</a><span class="hide"> | </span>\n' \
			% (menuType, self.pageURL, page, menuName)
		return line

	def create_sub_menu(self):
		subMenuString = ''
		if len(self.menuMap[self.activeMenu]) > 0:
			for sub in self.menuMap[self.activeMenu]:
				status = 'sub' + ['', ' active'][sub == self.activeSubMenu]
				pageKey = self.get_page_key(self.activeMenu, sub)
				page = self.pageMap[pageKey]
				subMenuString += self.create_menu_item(page, sub, status)
		return subMenuString

	def create_live_menu(self):
		menuString = '<a class="nav">Live View</a><span class="hide"> | </span>\n'
		for i in range(len(NUM)):
			menuString += '				<a class="nav sub active" href="/cgi-bin/live_view.cgi?streamId=%d">' \
							% (i)
			menuString += '%s</a><span class="hide"> | </span>\n' % NUM[i]
		return menuString

	def create_menu(self):
		menuString = self.create_live_menu()
		for name in sorted(self.menuMap.keys()):
			isActive = [False, True][name == self.activeMenu]
			if len(self.menuMap[name]) > 0:
				sub = self.menuMap[name][0]
			else:
				sub = None
			pageKey = self.get_page_key(name, sub)
			page = self.pageMap[pageKey]
			if isActive:
				menuString += self.create_menu_item(page, name, 'active')
				menuString += self.create_sub_menu()
			else:
				menuString += self.create_menu_item(page, name)
		self.menu = menuString
		return menuString

	def support_cgi(self):
		print self.header				# HTML is following
		print
		return

	def add_params(self):
		return ''

	def show_params(self):
		if self.get_params() != 1:
			return

		print '<div id="status">&nbsp; </div>\n'
		print self.add_params()
		return

	def create_slider_bar(self, label, name, value, sliderRange, outBox):
		smin, smax = sliderRange
		width = smax - smin
		SLIDERWIDTH = 350
		left = (value - smin) * SLIDERWIDTH / width
		text = '				<label>%s:</label>\n' % label
		action = '''<input type="text" class="textinput" id="%s" value="%s" maxlength=%d readonly="readonly" />''' % (outBox, value, 4)
		text += '''				%s''' % action + '<br><br>\n'
		text += '					<div class="left barholder"></div>\n'
		action = '''onclick="javascript:AddSubOneStep('%s', '%s', %d, %d, 1)"''' % (name, outBox, width, smin)
		text += '					<div class="left"><img src="../img/left.gif" %s />%d&nbsp; </div>\n' % (action, smin)
		action = '''onmouseover="javascript:MoveBar('%s', '%s', %d, %d)"''' % (name, outBox, width, smin)
		text += '					<div class="left slider">'
		text += '<div class="bar" id="%s" %s style="left: %dpx; "></div></div>\n' % (name, action, left)
		action = '''onclick="javascript:AddSubOneStep('%s', '%s', %d, %d, 0)"''' % (name, outBox, width, smin)
		text += '					<div class="left">&nbsp; %d<img src="../img/right.gif" %s /></div>\n' % (smax, action)
		return text

	def process_PostData(self):
		fieldform = cgi.FieldStorage()

		if fieldform.getvalue('req_cnt'):
			req_cnt = int(fieldform.getvalue('req_cnt'));
			for i in range (0, req_cnt):
				sec = 'sec%d' % i
				data = 'data%d' % i
				sec_value = fieldform.getvalue(sec)
				data_value =  fieldform.getvalue(data)
				if sec_value and data_value :
					if (self.trans.send_set_request('REQ_SET_PARAM', sec_value, data_value)) != 0 :
						return -1
				else :
					return -1
			return 0
		else :
			return 1

	''' These methods inherited from AmbaPage base class '''
	def add_css_style(self):
		super(AmbaConfigPage, self).add_css_style()
		self.css += self.cssStyle
		return

	def add_body_JS(self):
		self.bodyJS = ''' onload="javascript: getData('%s')" ''' \
					% (self.name)
		return

	def add_content(self):
		super(AmbaConfigPage, self).add_content()
		menu = self.create_menu()
		params = self.add_params()
		self.content += self.contentHtml % (menu, params)
		return

	''' This is auto test method '''
	def auto_test(self):
		self.support_cgi()
		print self.create_page_html()
		return



