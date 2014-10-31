#!/usr/bin/env python

##############################################
#
# csb.py
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

class AmbaCamBasicPage(AmbaConfigPage):
	''' This page is Camera basic setting page '''

	def __init__(self, menu, subMenu):
		super(AmbaCamBasicPage, self).__init__('csb', menu, subMenu);
		self.vinvout_params = {
			'vin_mode': 0,
			'vin_framerate' : 0,
			'vin_mirror' : -1,
			'vin_bayer' : -1,
			'vout_type' : PP.vout_type['cvbs'],
			'vout_mode' : PP.video_mode['480I']
		}
		return

	def process_PostData(self):
		fieldform = cgi.FieldStorage()

		req_cnt = 0
		vin_fps_changed = 0
		image_data = ''
		vv_changed = 0
		vv_data = ''
		if fieldform.getvalue('req_cnt') :
			req_cnt = int(fieldform.getvalue('req_cnt'))
			for i in range (0, req_cnt):
				sec = 'sec%d' % i
				data = 'data%d' % i
				sec_value = fieldform.getvalue(sec)
				data_value = fieldform.getvalue(data)
				if sec_value == "IMAGE" :
					vin_fps_changed = 1
					image_data = data_value
				else :
					vv_changed = 1
					vv_data = data_value
			if vin_fps_changed :
				data_value = "slow_shutter = 0\n"
				if (self.trans.send_set_request('REQ_SET_PARAM', 'IMAGE', data_value)) < 0 :
					return -1
			if vv_changed :
				if (self.trans.send_set_request('REQ_SET_PARAM', 'VINVOUT', vv_data)) < 0 :
					return -1
			else :
				return -1
			if vin_fps_changed :
				image_data += "slow_shutter = 1\n"
				if (self.trans.send_set_request('REQ_SET_PARAM', 'IMAGE', image_data)) < 0 :
					return -1
			return 0
		else :
			return 1

	def get_params(self):
		ret = self.process_PostData()
		if ret < 0 :
			print "1:set params failed"
		elif ret == 0 :
			print "0:set params succeeded"
		elif ret == 1 :
			self.get_section_param('VINVOUT', self.vinvout_params)
		else :
			 print "1:unexpected error"
		return ret

	def add_params(self):
		text = ''
		text += self.add_vin()
		text += self.add_vout()
		text += '''<p align="center">\n'''
		text += '''<input type="button" value="Apply" onclick = "javascript:setCamBasic()"/>&nbsp; &nbsp; \n'''
		text += '''<input type="button" value="Cancel" onclick = "javascript:showPage('csb')"/>\n'''
		text += '''</p>\n'''
		return text

	def add_vin(self):
		text = '<fieldset><legend>VIN Settings</legend><br>\n'
		options = [[PP.video_mode['auto'], 'Auto'], [PP.video_mode['1080P'], '1080P'], \
				[PP.video_mode['720P'], '720P']]
		text += self.create_select_label('Resolution :', 'vin_mode', options, self.vinvout_params['vin_mode'])
		options = [[PP.fps['auto'], 'Auto'], [PP.fps['5'], '5'], [PP.fps['6'], '6'], \
				[PP.fps['10'], '10'], [PP.fps['13'], '13'], [PP.fps['15'], '15'], \
				[PP.fps['25'], '25'], [PP.fps['29.97'], '29.97'], \
				[PP.fps['30'], '30'], [PP.fps['59.94'], '59.94'], [PP.fps['60'], '60']]
		text += self.create_select_label('Frame Rate (fps) :','vin_framerate', options, \
				self.vinvout_params['vin_framerate'])
		text += '<br><br>'
		options = [[4, 'Auto'], [0, '0'], [1, '1'], [2, '2'], [3, '3']]
		text += self.create_select_label('Mirror Pattern :', 'vin_mirror', options, self.vinvout_params['vin_mirror'])
		text += self.create_select_label('Bayer Pattern :', 'vin_bayer', options, self.vinvout_params['vin_bayer'])
		text += '<br><br></fieldset><br>\n'
		return text

	def add_vout(self):
		text = '<fieldset><legend>VOUT Settings</legend><br>\n'
		options = [[PP.vout_type['off'], 'OFF'], [PP.vout_type['cvbs'], 'CVBS'], \
				[PP.vout_type['hdmi'], 'HDMI']]
		action = '''onchange="setVoutMode(this.options[this.selectedIndex].value)"'''
		text += self.create_select_label('Type :', 'vout_type', options, \
				self.vinvout_params['vout_type'], action) + '&nbsp; &nbsp; '
		if self.vinvout_params['vout_type'] == PP.vout_type['hdmi']:
			options = [[PP.video_mode['480P'], '480P'], [PP.video_mode['576P'], '576P'], \
					[PP.video_mode['720P'], '720P'], [PP.video_mode['1080I'], '1080I'], \
					[PP.video_mode['1080P30'], '1080P30']]
		else:
			options = [[PP.video_mode['480I'], '480I'], [PP.video_mode['576I'], '576I']]
		text += self.create_select_label('Resolution :', 'vout_mode', options, \
				self.vinvout_params['vout_mode'])
		text += '<br><br></fieldset><br>\n'
		return text


def main():
	support_cgi()
	page = AmbaCamBasicPage('Camera Setting', 'Basic')
	page.show_params()
	return

if __name__ == '__main__':
	main()

