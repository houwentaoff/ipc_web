#!/usr/bin/env python

##############################################
#
# enc.py
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

from pages import *
import os

class AmbaEncPage(AmbaConfigPage):
	''' This page is Encode setting page '''

	def __init__(self,  menu, subMenu = None):
		super(AmbaEncPage, self).__init__("enc", menu, subMenu);
		self.create_params()
		return

	def create_params(self):
		self.stream_params = {}
		self.enc_params = {}
		for streamID in range(len(NUM)):
			self.stream_params[self.get_name(streamID, 'h264_id')] = 0
			self.stream_params[self.get_name(streamID, 'M')] = 0
			self.stream_params[self.get_name(streamID, 'N')] = 0
			self.stream_params[self.get_name(streamID, 'idr_interval')] = 0
			self.stream_params[self.get_name(streamID, 'gop_model')] = 0
			self.stream_params[self.get_name(streamID, 'profile')] = 0
			self.stream_params[self.get_name(streamID, 'brc')] = 0
			self.stream_params[self.get_name(streamID, 'cbr_avg_bps')] = 0
			self.stream_params[self.get_name(streamID, 'vbr_min_bps')] = 0
			self.stream_params[self.get_name(streamID, 'vbr_max_bps')] = 0
			self.stream_params[self.get_name(streamID, 'quality')] = 0
			self.enc_params[self.get_name(streamID, 'type')] = 0
			self.enc_params[self.get_name(streamID, 'enc_fps')] = 30
			self.enc_params[self.get_name(streamID, 'dptz')] = 0
			self.enc_params[self.get_name(streamID, 'flip_rotate')] = 0
		self.enc_params['enc_mode'] = 0,
		self.enc_params['s0_width'] = 1280
		self.enc_params['s0_height'] = 720
		self.enc_params['s1_width'] = 720
		self.enc_params['s1_height'] = 480
		self.enc_params['s2_width'] = 352
		self.enc_params['s2_height'] = 240
		self.enc_params['s3_width'] = 352
		self.enc_params['s3_height'] = 240
		return

	def process_PostData(self):
		ret = 0
		fieldform = cgi.FieldStorage()

		req_cnt = 0
		if fieldform.getvalue('req_cnt'):
			req_cnt = int(fieldform.getvalue('req_cnt'));
			for i in range (0, req_cnt):
				req = 'req%d' % i
				info = 'info%d' % i
				sec = 'sec%d' % i
				data = 'data%d' % i
				req_value = fieldform.getvalue(req)
				info_value =  fieldform.getvalue(info)
				sec_value = fieldform.getvalue(sec)
				data_value = fieldform.getvalue(data)
				if sec_value and data_value :
					ret = self.trans.send_set_request('REQ_SET_PARAM', sec_value, data_value)
					if ret != 0 :
						return -1;
				elif req_value and info_value :
					ret = self.trans.send_fly_request(req_value, int(info_value))
					if ret != 0 :
						return -1;
				else :
					ret = -1
					break
		return req_cnt

	def get_params(self):
		ret = self.process_PostData()
		if ret < 0 :
			print "1:set params failed"
		elif (ret > 0):
			print "0:set params succeeded"
		elif ret == 0 :
			self.get_section_param('ENCODE', self.enc_params)
			for stream in range(len(NUM)):
				self.get_section_param('STREAM%d' % stream, self.stream_params)
			return 1
		else :
			print "1:unexpected error %d" % ret
		return 0

	def get_name(self, streamID, name):
		return 's%d_%s' % (streamID, name)

	def add_params(self):
		text = ''
		content = '''Note: Stream resolutions should be in the descending order.'''
		text += self.create_em_text(content, 'style1') + '<br>\n'
		options = [[0, 'Normal mode'], [1, 'High Mega-pixel mode'], [2, 'Low delay mode']]
		action = '''onchange="setEncodeMode(this.options[this.selectedIndex].value)"'''
		text += self.create_select_label('Encode Mode :', 'enc_mode', options, \
				self.enc_params['enc_mode'], action) + '<br><br>\n'
		encMode = self.enc_params['enc_mode']
		for streamID in range(len(NUM)):
			if streamID > 0 and encMode != PP.enc_mode['high-fps']:
				enabled = 'disabled'
			else :
				enabled = ''
			text += self.add_encode_format(streamID, enabled, encMode)
		text += '''<p align="center" >\n'''
		text += '''<input type="button" value="Apply" onclick = "javascript:setEnc()"/>&nbsp; &nbsp; \n'''
		text += '''<input type="button" value="Cancel" onclick = "javascript:showPage('enc')"/>\n'''
		text += '''</p>\n'''
		return text

	def add_encode_format(self, streamID, enabled, encMode):
		text = '<fieldset><legend>Stream %d</legend><br>\n' % streamID
		options = [[0, 'OFF'], [1, 'H.264'], [2, 'MJPEG']]
		text += self.create_select_label('Type :', self.get_name(streamID, 'type'), options, \
				self.enc_params[self.get_name(streamID, 'type')], enabled) + '&nbsp; &nbsp; '
		fpsList = [60, 30, 25, 20, 15, 10, 6, 5, 4, 3, 2, 1]
		options = []
		for item in fpsList:
			item = '%s' % item
			options.append([PP.fps[item], item])
		text += self.create_select_label('Encode FPS :', self.get_name(streamID, 'enc_fps'), options, \
				self.enc_params[self.get_name(streamID, 'enc_fps')], enabled) + '&nbsp; &nbsp; '
		options = [[0, 'Disable'], [1, 'Enable']]
		if self.enc_params[self.get_name(streamID, 'width')] == 0 or \
			self.enc_params[self.get_name(streamID, 'height')] == 0:
			value = PP.create_res(1280, 720)
		else:
			value = PP.create_res(self.enc_params[self.get_name(streamID, 'width')], \
						self.enc_params[self.get_name(streamID, 'height')])
		if encMode == PP.enc_mode['low-delay'] :
			action = 'disabled'
		elif enabled == 'disabled' :
			action = 'disabled'
		else :
			action = ''
		text += self.create_select_label('DPTZ Type :', self.get_name(streamID, 'dptz'), options, \
			self.enc_params[self.get_name(streamID, 'dptz')], action)
		text += '<br><br>\n'
		options = []
		if encMode != PP.enc_mode['high-mp'] or streamID != 0 :
			for i in sorted(PP.res.keys(), reverse=True):
				options.append([PP.res[i], i])
		else :
			for i in sorted(PP.highRes.keys(), reverse=True):
				options.append([PP.highRes[i], i])
		action = '''onchange="setDPTZMode(this.options[this.selectedIndex].value, %d)" %s''' \
				% (streamID, enabled)
		text += self.create_select_label('Resolution :', self.get_name(streamID, 'resolution'), \
				options, value, action) + '&nbsp; &nbsp; '
		options = [[PP.fr['normal'], 'Normal'], [PP.fr['hflip'], 'Horizontal Flip'], \
				[PP.fr['vflip'], 'Vertical Flip'], [PP.fr['rotate90'], 'Rotate Clockwise 90'], \
				[PP.fr['rotate180'], 'Rotate 180'], [PP.fr['rotate270'], 'Rotate Clockwise 270']]
		text += self.create_select_label('Flip & Rotate :', self.get_name(streamID, 'flip_rotate'), \
				options, self.enc_params[self.get_name(streamID, 'flip_rotate')], enabled) + '<br><br>'
		text += self.add_expand_format(streamID, enabled)
		text += '</fieldset><br>\n'
		return text

	def add_expand_format(self, streamID, enabled):
		text = ''
		settings = ''
		headerID = 'stream%d' % streamID
		expandID = 'setting%d' % streamID
		text += '''<div class="expandstyle"><span id="%s" onClick="expandEncodeFormat('%s', '%s', 360, %d)">''' \
			% (headerID, headerID, expandID, streamID)
		text += '''<img src="../img/expand.gif" /></span>Stream settings :<br><br>\n'''
		settings += self.add_h264(streamID, enabled)
		settings += self.add_mjpeg(streamID, enabled)
		text += '''<div class="expandcontent" id="%s">%s</div>''' % (expandID, settings)
		text += '''</div>'''
		return text

	def add_h264(self, streamID, enabled):
		text = '<fieldset><legend>H.264</legend><br>'
		string = ['', '(1-3)'][streamID == 0]
		action = ["disabled", ""][streamID == 0]
		action += enabled
		text += self.create_text_entry('M %s :' % string, self.get_name(streamID, 'M'), \
				self.stream_params[self.get_name(streamID, 'M')], 1, action) + '&nbsp; &nbsp;'
		text += self.create_text_entry('N (1-255) :', self.get_name(streamID, 'N'), \
				self.stream_params[self.get_name(streamID, 'N')], 3, enabled) + '<br><br>'
		text += self.create_text_entry('IDR interval (1-100) :', self.get_name(streamID, 'idr_interval'), \
				self.stream_params[self.get_name(streamID, 'idr_interval')], 4, enabled) + '<br><br>'
#		Disable it since ucode does not support hierarchical GOP model
#		options = [[PP.gopm['simple'], 'Simple'], [PP.gopm['adv'], 'Advanced']]
#		text += self.create_select_label('GOP model :', self.get_name(streamID, 'gop_model'), options, \
#				self.stream_params[self.get_name(streamID, 'gop_model')], enabled) + '&nbsp; &nbsp; '
		options = [[PP.profile['main'], 'Main'], [PP.profile['baseline'], 'Baseline']]
		text += self.create_select_label('Profile :', self.get_name(streamID, 'profile'), options, \
				self.stream_params[self.get_name(streamID, 'profile')], enabled) + '<br><br>'
		options = [[PP.brc['cbr'], 'CBR'], [PP.brc['vbr'], 'VBR'], \
				[PP.brc['cbrq'], 'CBR (keep quality)'], \
				[PP.brc['vbrq'], 'VBR (keep quality)']]
		action = '''onchange="setBRCMode(this.options[this.selectedIndex].value, %d)" %s''' \
				% (streamID, enabled)
		brc = self.stream_params[self.get_name(streamID, 'brc')]
		text += self.create_select_label('Bitrate control :', self.get_name(streamID, 'brc'), \
				options, brc, action) + '<br><br>'
		action = ['', 'disabled'][brc & 0x1]
		if action == 'disabled' or enabled == 'disabled':
			action = 'disabled'
		text += self.create_text_entry('Average Bitrate  :', self.get_name(streamID, 'cbr_avg_bps'), \
				self.stream_params[self.get_name(streamID, 'cbr_avg_bps')], 10, action)
		text += '<br><br>'
		action = ['disabled', ''][brc & 0x1]
		if action == 'disabled' or enabled == 'disabled':
			action = 'disabled'
		text +=  self.create_text_entry('Min bitrate (bps) :', self.get_name(streamID, 'vbr_min_bps'), \
				self.stream_params[self.get_name(streamID, 'vbr_min_bps')], 10, action) + '&nbsp; &nbsp; '
		text +=  self.create_text_entry('Max bitrate (bps) :', self.get_name(streamID, 'vbr_max_bps'), \
				self.stream_params[self.get_name(streamID, 'vbr_max_bps')], 10, action)
		text += '<br></fieldset><br>'
		return text

	def add_mjpeg(self, streamID, enabled):
		text = '<fieldset><legend>MJPEG</legend><br>'
		text += self.create_text_entry('Quality (0 - 100) :', self.get_name(streamID, 'quality'), \
				self.stream_params[self.get_name(streamID, 'quality')], 4, enabled)
		text += '<br></fieldset><br>'
		return text


def main():
	support_cgi()
	page = AmbaEncPage('Encode Setting')
	page.show_params()

	return

if __name__ == '__main__':
	main()

