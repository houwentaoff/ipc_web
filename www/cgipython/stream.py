#!/usr/bin/env python

##############################################
#
# stream.py
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


class AmbaStreamPage(AmbaConfigPage):
	''' This page is Stream setting page '''

	def __init__(self,  menu, subMenu, streamId = 0):
		super(AmbaStreamPage, self).__init__("stream_%d" % streamId, menu, subMenu);
		self.streamId = streamId
		self.prefix = 's%d' % streamId
		self.stream_params = {
				self.get_name('id') : 0,
				self.get_name('type') : 0,
				self.get_name('source') : 0,
				self.get_name('flip_rotate') : 0,
				self.get_name('width') : 0,
				self.get_name('height') : 0,
				self.get_name('start_x') : 0,
				self.get_name('start_y') : 0,
				self.get_name('h264_id') : 0,
				self.get_name('M') : 0,
				self.get_name('N') : 0,
				self.get_name('idr_interval') : 0,
				self.get_name('brc') : 0,
				self.get_name('profile') : 0,
				self.get_name('gop_model') : 0,
				self.get_name('vbr_ness') : 0,
				self.get_name('min_vbr') : 0,
				self.get_name('max_vbr') : 0,
				self.get_name('average_bitrate') : 0,
				self.get_name('quality') : 0
		}
		return

	def get_name(self, name):
		return self.prefix + '_' + name

	def get_params(self):
		ret = self.process_PostData()
		if ret < 0 :
			print "1:set params failed"
		elif ret == 0 :
			print "0:set params succeeded"
		elif ret == 1 :
			self.get_section_param('STREAM%d' % self.streamId, self.stream_params)
		else :
			print "1:unexpected error"
		return ret

	def add_params(self):
		text = ''
		content = 'Note: All settings can only be configured when stream is not encoding.'
		text += self.create_em_text(content, 'style1') + '<br>'
		text += '<fieldset><legend>Format Setting</legend><br>\n'
#		text += self.add_format()
		text += self.add_h264()
		text += self.add_mjpeg()
		text += '<br></fieldset><br>\n'
		text += '''<p align="center" >\n'''
		text += '''<input type="button" value="Apply" onclick = "javascript:setStream(%d)"/>&nbsp; &nbsp; \n''' % (self.streamId)
		text += '''<input type="button" value="Cancel" onclick = "javascript:showPage('stream')"/>\n'''
		text += '''</p>\n'''
		return text

	def add_format(self):
		text = ''
		options = [[0, 'None'], [1, 'H.264'], [2, 'MJPEG']]
		text += self.create_radio_input('Type :', self.get_name('type'), options, \
				self.stream_params[self.get_name('type')]) + '<br><br>'
		options = [[0, 'Main'], [1, 'Secondary']]
		text += self.create_radio_input('Source Buffer :', self.get_name('source'), options, \
				self.stream_params[self.get_name('source')]) + '<br><br>'
		if self.stream_params[self.get_name('width')] == 0 or self.stream_params[self.get_name('height')] == 0:
			value = PP.create_res(1280, 720)
		else:
			value = PP.create_res(self.stream_params[self.get_name('width')], \
							self.stream_params[self.get_name('height')])
		options = []
		for i in sorted(PP.res.keys()):
			options.append([PP.res[i], i])
		text += self.create_select_label('Resolution :', self.get_name('resolution'), options, value) + '&nbsp; &nbsp; '
		options = [[0, 'Nomal'], [1, 'Horizontal Flip'], [2, 'Vertical Flip'], [3, 'Rotate 180 degree']]
		text += self.create_select_label('Flip & Rotate :', self.get_name('flip_rotate'), options, \
				self.stream_params[self.get_name('flip_rotate')]) + '<br>\n'
		text += '				&nbsp; &nbsp; &nbsp; Encode offset : '
		text += self.create_text_entry('x :', self.get_name('start_x'), self.stream_params[self.get_name('start_x')], 4)
		text += self.create_text_entry('y :', self.get_name('start_y'), self.stream_params[self.get_name('start_y')], 4)
		text += '<br><br>\n'
		return text

	def add_h264(self):
		text = '<fieldset><legend>H.264</legend><br>'
		text += self.create_text_entry('M (1-3) :', self.get_name('M'), \
				self.stream_params[self.get_name('M')], 2) + '&nbsp; &nbsp;'
		text += self.create_text_entry('N :', self.get_name('N'), \
				self.stream_params[self.get_name('N')], 4) + '&nbsp; &nbsp;'
		text += '<br><br>'
		text += self.create_text_entry('IDR interval :', self.get_name('idr_interval'), \
				self.stream_params[self.get_name('idr_interval')], 4) + '<br><br>'
		options = [[PP.brc['cbr'], 'CBR'], [PP.brc['vbr'], 'VBR']]
		text += self.create_radio_input('Bitrate Control :', self.get_name('brc'), options, \
				self.stream_params[self.get_name('brc')]) + '<br /><br />'
		options = [[PP.gopm['simple'], 'Simple'], [PP.gopm['adv'], 'Advanced']]
		text += self.create_radio_input('GOP Model :', self.get_name('gop_model'), options, \
				self.stream_params[self.get_name('gop_model')]) + '<br><br>'
		options = [[PP.profile['main'], 'Main'], [PP.profile['baseline'], 'Baseline']]
		text += self.create_radio_input('Profile :', self.get_name('profile'), options, \
				self.stream_params[self.get_name('profile')]) + '<br><br>'
		text += self.create_text_entry('VBRness (0 - 99) :', self.get_name('vbr_ness'), \
				self.stream_params[self.get_name('vbr_ness')], 4) + '<br><br>'
		text += self.create_text_entry('Min VBR factor (0 - 100) :', self.get_name('min_vbr'), \
				self.stream_params[self.get_name('min_vbr')], 4) + '<br><br>'
		text += self.create_text_entry('Max VBR factor (0 - 400) :', self.get_name('max_vbr'), \
				self.stream_params[self.get_name('max_vbr')], 4) + '<br><br>'
		text += self.create_text_entry('Average bitrate (bps) :', self.get_name('average_bitrate'), \
				self.stream_params[self.get_name('average_bitrate')], 20)
		text += '<br><br></fieldset><br>'
		return text

	def add_mjpeg(self):
		text = '<fieldset><legend>MJPEG</legend><br>'
		text += self.create_text_entry('Quality (0 - 100) :', self.get_name('quality'), \
				self.stream_params[self.get_name('quality')], 4)
		text += '<br><br></fieldset><br>'
		return text

	def add_body_JS(self):
		self.bodyJS = ''' onload="javascript: getData('stream', %d)" ''' % (self.streamId)
		return

def main():
	support_cgi()
	streamId = os.environ.get("streamId")
	if not streamId :
		streamId = 0
	else :
		streamId = int(streamId)
	page = AmbaStreamPage('Stream Setting', NUM[streamId], streamId)
	page.show_params()

	return

if __name__ == '__main__':
	main()

