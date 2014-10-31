#!/usr/bin/env python

##############################################
#
# pm.py
#
# History:
#	2010/05/28 - [Jian Tang] Created this file
#
# Copyright (C) 2007 - 2010, Ambarella, Inc.
#
# All rights reserved. No Part of this file may be reproduced, stored
# in a retrieval system, or transmitted, in any form, or by any means,
# electronic, mechanical, photocopying, recording, or otherwise,
# without the prior consent of Ambarella, Inc.
#
##############################################

from pages import *
import os

class AmbaPMPage(AmbaConfigPage):
	''' This page is Privacy Mask page '''

	videoHtml = '''
	<div id="video">
	<OBJECT CLASSID="clsid:9BE31822-FDAD-461B-AD51-BE1D1C159921" align="absmiddle" ID="vlc" events="True" style="width:510px;height:300px;position:absolute;display:block">
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
				width="510"
				height="300"
				toolbar="true"
				text="Waiting for video"
				name="vlc">
			</EMBED>
		</OBJECT>
	</div>
	'''
	videoHtmlforIE = '''
	<div id="videoforIE">
		<OBJECT CLASSID="CLSID:3BCDAA6A-7306-42FF-B8CF-BE5D3534C1E4" codebase="http://%s/activeX/ambaWeb.cab#version=%s" align="absmiddle" ID="AmbaIPCmrWebPlugIn1" style="width:510px;height:300px;position:absolute;display:block>
			<PARAM NAME="_Version" VALUE="65536">
			<PARAM NAME="_ExtentX" VALUE="19045">
			<PARAM NAME="_ExtentY" VALUE="11478">
			<PARAM NAME="_StockProps" VALUE="0">
		</OBJECT>
	</div>
	'''

	def __init__(self,  menu, subMenu = None):
		super(AmbaPMPage, self).__init__("pm", menu, subMenu);
		self.pm_params = {
			'pm_left': 0,
			'pm_top': 0,
			'pm_w': 0,
			'pm_h': 0,
			'pm_color': 0,
		}
		self.streamId = 0
		return

	def add_css_style(self):
		super(AmbaPMPage, self).add_css_style()
		self.css += '\n#content img { border: 1px solid #000000; }'

	def get_params(self):
		ret = self.process_PostData()
		if ret < 0 :
			print "1:set params failed"
		elif (ret == 0):
			print "0:set params succeeded"
		elif (ret == 1):
			self.get_section_param('PRIMASK', self.pm_params)
			return 1
		else :
			print "1:unexpected error %d" % ret
		return 0

	def add_params(self):
		text = ''
		text += '<fieldset><legend>Privacy Mask</legend><br>\n'
		text += self.add_canvas()
		text += self.add_mask()
		text += '''<p align="center" >\n'''
		text += '<input type="button" value="Add include" onclick = "javascript:addRemovePrivacyMask(0)" />&nbsp; &nbsp; \n'
		text += '<input type="button" value="Add exclude" onclick = "javascript:addRemovePrivacyMask(1)" />&nbsp; &nbsp; \n'
		text += '<input type="button" value="Remove" onclick = "javascript:clearPrivacyMask()" />\n'
		text += '''</p>\n'''
#		text += self.add_mask_list()
		text += '</fieldset><br>\n'
		return text

	def add_canvas(self):
		text = '<p align="center" class="style1">Main Encode Source Buffer</p>\n'
		text += '''<div id="pm_canvas" style="clear: both; position:relative; width:96%; height:300px; margin:0 auto">\n'''
#		text += '''<img id="pm_canvas" alt="privacy_mask" src="../img/gray.gif" width=510 height=300 />\n'''
		text += self.videoHtml
		text += self.videoHtmlforIE % (self.host, self.version)
		text += '''<div id="pm_mask">\n</div>\n'''
		text += '''</div>'''
		return text

	def add_mask(self):
		text = ''
		options = [[PP.color['black'], 'Black'], [PP.color['red'], 'Red'], \
				[PP.color['blue'], 'Blue'], [PP.color['green'], 'Green'], \
				[PP.color['yellow'], 'Yellow'], [PP.color['magenta'], 'Magenta'], \
				[PP.color['cyan'], 'Cyan'], [PP.color['white'], 'White']]
		text += '<br><br>\n' + self.create_select_label('Privacy mask color :', \
					'pm_color', options, self.pm_params['pm_color'])
		text += '<br><br><label>Privacy mask rectangle : </label><br><br>\n'
		text += self.create_text_entry('Left (0 ~ 100%) :', 'pm_left', self.pm_params['pm_left'], 2) + '&nbsp; &nbsp; \n'
		text += self.create_text_entry('Top (0 ~ 100%) :', 'pm_top', self.pm_params['pm_top'], 2) + '<br><br>\n'
		text += self.create_text_entry('Width (0 ~ 100%) :', 'pm_w', self.pm_params['pm_w'], 2) + '&nbsp; &nbsp; \n'
		text += self.create_text_entry('Height (0 ~ 100%) :', 'pm_h', self.pm_params['pm_h'], 2) + '<br><br>\n'
		return text

	def add_mask_list(self):
		text = ''
		text += '<br><br>&nbsp; Privacy Mask Rectangle List : <br>\n'
		text += '<table align="center" id="pm_mask_list" border=1 cellspacing=0 borderColor=#0cc width=90%>\n'
		text += '<tr><th width=18%>No.</th><th>Left</th><th>Top</th><th>Width</th><th>Height</th></tr><br>\n'
		text += '</table>'
		return text

	def add_body_JS(self):
		self.bodyJS += ''' onload="javascript: OnLoadActiveX('%s', %d, %d, %d, %d);" ''' % \
			(self.host, self.streamId, self.recvType, 0, 0)
		return


def main():
	support_cgi()
	page = AmbaPMPage('Privacy Mask')
	page.show_params()

	return

if __name__ == '__main__':
	main()

