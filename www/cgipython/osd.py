#!/usr/bin/env python

##############################################
#
# osd.py
#
# History:
#	2011/02/28 - [Jian Tang] Created this file
#
# Copyright (C) 2007 - 2011, Ambarella, Inc.
#
# All rights reserved. No Part of this file may be reproduced, stored
# in a retrieval system, or transmitted, in any form, or by any means,
# electronic, mechanical, photocopying, recording, or otherwise,
# without the prior consent of Ambarella, Inc.
#
##############################################

from pages import *
import os

class AmbaOSDPage(AmbaConfigPage):
	''' This page is OSD setting page '''

	def __init__(self,  menu, subMenu = None):
		super(AmbaOSDPage, self).__init__("osd", menu, subMenu);
		self.create_params()
		return

	def create_params(self):
		self.osd_params = {}
		for streamID in range(len(NUM)):
			self.osd_params[self.get_name(streamID, 'no_rotate')] = 0
			self.osd_params[self.get_name(streamID, 'bmp_enable')] = 0
			self.osd_params[self.get_name(streamID, 'time_enable')] = 0
			self.osd_params[self.get_name(streamID, 'text_enable')] = 0
			self.osd_params[self.get_name(streamID, 'text')] = 0
			self.osd_params[self.get_name(streamID, 'text_size')] = PP.ftsize['normal']
			self.osd_params[self.get_name(streamID, 'text_outline')] = 0
			self.osd_params[self.get_name(streamID, 'text_color')] = 0
			self.osd_params[self.get_name(streamID, 'text_bold')] = 0
			self.osd_params[self.get_name(streamID, 'text_italic')] = 0
			self.osd_params[self.get_name(streamID, 'text_startx')] = 0
			self.osd_params[self.get_name(streamID, 'text_starty')] = 0
			self.osd_params[self.get_name(streamID, 'text_boxw')] = 50
			self.osd_params[self.get_name(streamID, 'text_boxh')] = 50
		return

	def get_params(self):
		ret = self.process_PostData()
		if ret < 0 :
			print "1:set params failed"
		elif (ret == 0):
			print "0:set params succeeded"
		elif (ret == 1):
			self.get_section_param('OSD', self.osd_params)
			return 1
		else :
			print "1:unexpected error %d" % ret
		return 0

	def get_name(self, streamID, name):
		return 's%d_%s' % (streamID, name)

	def add_params(self):
		text = ''
		for streamID in range(len(NUM)):
			text += self.add_osd(streamID)
		text += '''<p align="center" >\n'''
		text += '''<input type="button" value="Apply" onclick = "javascript:setOSD()"/>&nbsp; &nbsp; \n'''
		text += '''<input type="button" value="Cancel" onclick = "javascript:showPage('osd')"/>\n'''
		text += '''</p>\n'''
		return text

	def add_osd(self, streamID):
		text = '<fieldset><legend>Stream %d</legend><br>\n' % streamID
		action = '''onchange="addOSD('no_rotate', %d)"''' % streamID
		name = self.get_name(streamID, 'no_rotate')
		text += '&nbsp; &nbsp;' + self.create_checkbox('No Rotate', name, \
				self.osd_params[name], action) + '&nbsp; &nbsp; '
		action = '''onchange="addOSD('bmp', %d)"''' % streamID
		name = self.get_name(streamID, 'bmp_enable')
		text += self.create_checkbox('Add BMP (8-bits, < 80K)', name, \
				self.osd_params[name], action) + '&nbsp; &nbsp; '
		action = '''onchange="addOSD('time', %d)"''' % streamID
		name = self.get_name(streamID, 'time_enable')
		text += self.create_checkbox('Add current time', name, \
				self.osd_params[name], action) + '<br><br>'
		text += self.add_expand_osd_text(streamID)
		text += '<br></fieldset><br>\n'
		return text

	def add_expand_osd_text(self, streamID):
		text = ''
		settings = ''
		headerID = 'text%d' % streamID
		expandID = 'osd_text%d' % streamID
		name = self.get_name(streamID, 'text_enable')
		enabled = ['', 'disabled'][self.osd_params[name] == 0]
		text += '''<div class="expandstyle"><span id="%s" onClick="expandOSDText('%s', '%s', 280, %d)">''' \
				% (headerID, headerID, expandID, streamID)
		text += '''<img src="../img/expand.gif" /></span>'''
		action = '''onclick="addOSD('text', %d)"''' % streamID
		text += self.create_checkbox('Add text string', name, \
				self.osd_params[name], action)
		text += '<br><br>\n'
		settings += self.add_osd_text(streamID, enabled)
		text += '''<div class="expandcontent" id="%s">%s</div>''' % (expandID, settings)
		text += '''</div>'''
		return text

	def add_osd_text(self, streamID, enabled):
		text = '<fieldset><legend>OSD Text</legend><br>'
		name = self.get_name(streamID, 'text')
		content = self.osd_params[name]
		if content[0] == '"' and content [-1] == '"' :
			content = content[1:-1]
		text += self.create_wide_text_entry('Content (32 character) : ', name, content, 32, enabled) + '<br><br>'
		name = self.get_name(streamID, 'text_size')
		options = [[PP.ftsize['smaller'], 'Smaller'], [PP.ftsize['small'], 'Small'], \
				[PP.ftsize['normal'], 'Middle'], [PP.ftsize['large'], 'Large'], \
				[PP.ftsize['larger'], 'Larger']]
		if self.osd_params[name] == 0:
			self.osd_params[name] = PP.ftsize['normal']
		text += self.create_select_label('Font size :', name, options, \
				self.osd_params[name], enabled) + '&nbsp; &nbsp; '
		name = self.get_name(streamID, 'text_color')
		options = [[PP.color['black'], 'Black'], [PP.color['red'], 'Red'], \
				[PP.color['blue'], 'Blue'], [PP.color['green'], 'Green'], \
				[PP.color['yellow'], 'Yellow'], [PP.color['magenta'], 'Magenta'], \
				[PP.color['cyan'], 'Cyan'], [PP.color['white'], 'White']]
		text += self.create_select_label('Color :', name, options, \
				self.osd_params[name], enabled) + '<br><br>'
		name = self.get_name(streamID, 'text_outline')
		options = []
		for i in range(len(NUM)):
			options.append([i, "%s" % i])
		text +=  '&nbsp; &nbsp; ' + self.create_select_label('Outline', name, options, \
				self.osd_params[name], enabled) + '&nbsp; &nbsp; '
		name = self.get_name(streamID, 'text_bold')
		text += self.create_checkbox('Bold', name, self.osd_params[name], enabled) + '&nbsp; &nbsp; '
		name = self.get_name(streamID, 'text_italic')
		text += self.create_checkbox('Italic', name, self.osd_params[name], enabled) + '<br><br>'
		text += self.add_osd_text_box(streamID, enabled);
		text += '<br></fieldset><br>'
		return text

	def add_osd_text_box(self, streamID, enabled):
		text = '<fieldset><legend>Text Box (0~100%)</legend><br>'
		name = self.get_name(streamID, 'text_startx')
		text += self.create_text_entry('Offset X : ', name, \
				self.osd_params[name], 2, enabled) + '&nbsp; &nbsp; '
		name = self.get_name(streamID, 'text_starty')
		text += self.create_text_entry('Y : ', name, \
				self.osd_params[name], 2, enabled) + '<br><br>'
		name = self.get_name(streamID, 'text_boxw')
		text += self.create_text_entry('Width : ', name, \
				self.osd_params[name], 2, enabled) + '&nbsp; &nbsp; '
		name = self.get_name(streamID, 'text_boxh')
		text += self.create_text_entry('Height : ', name, \
				self.osd_params[name], 2, enabled) + '<br>'
		text += '<br></fieldset>'
		return text

def main():
	support_cgi()
	page = AmbaOSDPage('OSD Setting')
	page.show_params()

	return

if __name__ == '__main__':
	main()

