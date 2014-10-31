#!/usr/bin/env python

##############################################
#
# iqb.py
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

class AmbaIQBasicPage(AmbaConfigPage):
	''' This page is Image Quality basic setting page '''

	cssStyle = '''
	.textinput { width:40px; }
	'''

	def __init__(self, menu, subMenu):
		super(AmbaIQBasicPage, self).__init__('iqb', menu, subMenu);
		self.img_params = {
			'preference': 0,
			'dn_mode': 0,
			'exposure_mode': 0,
			'backlight_comp': 0,
			'dc_iris': 0,
			'dc_iris_duty': 0,
			'local_exposure': 0,
			'mctf_strength': 0,
			'shutter_min': PP.shutter['1/8000'],
			'shutter_max': PP.shutter['1/30'],
			'max_gain': PP.gain['36db'],
			'ae_target_ratio': 100,
			'vin_fps': PP.fps['29.97'],
			'saturation': 20,
			'brightness': 40,
			'hue': 60,
			'contrast': 80,
			'sharpness': 100,
			'wbc': 0
		}

	def get_params(self):
		ret = self.process_PostData()
		if ret < 0:
			print "1:set params failed"
		elif ret == 0:
			print "0:set params succeeded"
		elif ret == 1:
			self.get_section_param('IMAGE', self.img_params)
		else :
			print "1:unexpected error"
		return ret

	def add_params(self):
		text = ''
		text += self.add_preference()
		text += self.add_basic()
		text += self.add_ae()
		text += self.add_image()
		text += self.add_awb()
		text += '''<p align="center">\n'''
		text += '''<input type="button" value="Apply" onclick = "javascript:setIQBasic()"/>&nbsp; &nbsp; \n'''
		text += '''<input type="button" value="Cancel" onclick = "javascript:showPaeg('iqb')"/>\n'''
		text += '''</p>\n'''
		return text

	def add_preference(self):
		text = '<fieldset><legend>Preference</legend><br>\n'
		options = [[0, 'Normal'], [1, 'Low Light'], [2, 'Traffic']]
		text += self.create_select_label('Preference :', 'preference', options, self.img_params['preference'], 'disabled')
		text += '<br><br></fieldset><br>\n'
		return text

	def add_basic(self):
		text = '<fieldset><legend>Basic Settings</legend><br>\n'
		options = [[0, 'Off'], [1, 'On']]
		text += self.create_select_label('Day Night Mode :', 'dn_mode', options, \
				self.img_params['dn_mode']) + '&nbsp; &nbsp; '
		text += self.create_text_entry('3D De-noise Filter (0 ~ 255) :', 'mctf_strength', \
				self.img_params['mctf_strength'], 3) + '<br><br>'
		options = [[0, 'Off'], [1, 'Auto'], [128, 'Manual Weak'], [192, 'Manual Medium'], \
				[256, 'Manual Strong']]
		text += self.create_select_label('Local Exposure Mode :', 'local_exposure', options, \
				self.img_params['local_exposure']) + '&nbsp; &nbsp; '
		options = [[0, 'Off'], [1, 'On']]
		text += self.create_select_label('Back-light Compensation :', 'backlight_comp', \
				options, self.img_params['backlight_comp']) + '<br><br>'
		text += '</fieldset><br>\n'
		return text

	def add_ae(self):
		text = '<fieldset><legend>Exposure Settings</legend><br>\n'
		options = [[0, 'Anti-flicker 50Hz'], [1, 'Anti-flicker 60Hz'], [2, 'Auto'], [3, 'Hold']]
		text += self.create_select_label('Exposure Control :', 'exposure_mode', \
				options, self.img_params['exposure_mode']) + '<br><br>'
		text += self.create_text_entry('Exposure Target Factor (25% ~ 400%) :', 'ae_target_ratio', \
				self.img_params['ae_target_ratio'], 3) + '<br><br>'
		options = [[0, 'Off'], [1, 'On']]
		action = '''onchange="setIRISDuty(this.options[this.selectedIndex].value)"'''
		text += self.create_select_label('DC Iris Mode :', 'dc_iris', options, \
				self.img_params['dc_iris'], action) + '&nbsp; &nbsp; '
		action = ['', 'disabled'][self.img_params['dc_iris'] == 0]
		text += self.create_text_entry('Balance Duty (1 ~ 999) :', 'dc_iris_duty', \
				self.img_params['dc_iris_duty'], 3, action) + '<br><br>'
		text += '&nbsp; Shutter Time Limit (seconds): '
		options = [[PP.shutter['1/8000'], '1/8000'], [PP.shutter['1/1024'], '1/1024'], \
				[PP.shutter['1/960'], '1/960'], [PP.shutter['1/480'], '1/480'], \
				[PP.shutter['1/240'], '1/240'], [PP.shutter['1/120'], '1/120'],  \
				[PP.shutter['1/100'], '1/100'], [PP.shutter['1/60'], '1/60'], \
				[PP.shutter['1/50'], '1/50'], [PP.shutter['1/30'], '1/30'], [PP.shutter['1/25'], '1/25']]
		action = '''onchange="setShutterLimit(this.options[this.selectedIndex].value)"'''
		text += self.create_select_label('Min', 'shutter_min', options, self.img_params['shutter_min'], action)
		options.append([PP.shutter['1/15'], '1/15'])
		options.append([PP.shutter['1/7.5'], '1/7.5'])
		text += self.create_select_label('Max', 'shutter_max', options, self.img_params['shutter_max'])
		text += '<br><br>'
		options = [[PP.gain['30db'], '30db'], [PP.gain['36db'], '36db (default)'], \
				[PP.gain['42db'], '42db'], [PP.gain['48db'], '48db'], \
				[PP.gain['54db'], '54db'], [PP.gain['60db'], '60db']]
		text += self.create_select_label('Sensor Max Gain :', 'max_gain', options, \
				self.img_params['max_gain']) + '<br><br>'
		text += '</fieldset><br>\n'
		return text

	def add_image(self):
		text = '<fieldset><legend>Image Property</legend><br>\n'
		text += self.create_text_entry('Saturation (0 ~ 255) :', 'saturation', self.img_params['saturation'], 3) + '<br><br>'
#		text += self.create_slider_bar('Saturation', 'satBar', self.img_params['saturation'], (0, 255), 'saturation') + '<br><br>\n'
		text += self.create_text_entry('Brightness (-255 ~ 255) :', 'brightness', self.img_params['brightness'], 4) + '<br><br>'
#		text += self.create_text_entry('Hue (-15 ~ 15) :', 'hue', self.img_params['hue'], 3) + '<br><br>'
		text += self.create_text_entry('Contrast (0 ~ 128) :', 'contrast', self.img_params['contrast'], 3) + '<br><br>'
		text += self.create_text_entry('De-noise / Sharpness (0 ~ 255) :', 'sharpness', self.img_params['sharpness'], 4)
		text += '<br><br></fieldset><br>\n'
		return text

	def add_awb(self):
		text = '<fieldset><legend>White Balance Setting</legend><br>\n'
		options = [[PP.wb['auto'], 'Auto'], [PP.wb['hold'], 'Hold'], \
				[PP.wb['Incandescent'], 'Incandescent'], \
				[PP.wb['D4000'], 'D4000'], [PP.wb['D5000'], 'D5000'], \
				[PP.wb['Sunny'], 'Sunny'], [PP.wb['Cloudy'], 'Cloudy'], \
				[PP.wb['Flash'], 'Flash'], [PP.wb['Fluorescent'], 'Fluorescent'], \
				[PP.wb['Fluorescent High'], 'Fluorescent High'], \
				[PP.wb['Under Water'], 'Under Water'], \
				[PP.wb['Custom'], 'Custom']]
		text +=self.create_select_label('White Balance Control :', 'wbc', options, self.img_params['wbc'])
		text += '<br><br></fieldset><br>\n'
		return text

def main():
	support_cgi()
	page = AmbaIQBasicPage('Image Quality', 'Basic')
	page.show_params()
	return

if __name__ == '__main__':
	main()

