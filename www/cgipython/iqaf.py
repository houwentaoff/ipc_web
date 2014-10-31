#!/usr/bin/env python

##############################################
#
# iqaf.py
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

class AmbaIQAFPage(AmbaConfigPage):
	''' This page is Image Quality Auto Focus page '''

	def __init__(self, menu, subMenu):
		super(AmbaIQAFPage, self).__init__('iqaf', menu, subMenu);
		self.af_params = {
			'lens_type': 0,
			'af_mode': 0,
			'af_tile_mode': 0,
			'zm_dist': 0,
			'fs_dist': 0,
			'fs_near': 0,
			'fs_far': 100
		}

	def get_params(self):
		ret = self.process_PostData()
		if ret < 0:
			print "1:set params failed"
		elif ret == 0:
			print "0:set params succeeded"
		elif ret == 1:
			self.get_section_param('IQAF', self.af_params)
		else :
			print "1:unexpected error"
		return ret

	def add_params(self):
		text = ''
		text += self.add_focus()
		text += '''<p align="center">\n'''
		text += '''<input type="button" value="Apply" onclick = "javascript:setIQAF()"/>&nbsp; &nbsp; \n'''
		text += '''<input type="button" value="Cancel" onclick = "javascript:showPage('iqaf')"/>\n'''
		text += '''</p>\n'''
		return text

	def add_focus(self):
		text = '<fieldset><legend>Focus</legend><br>\n'
		options = [[PP.lens['fixed_focus'], 'Fixed focus lens'], \
				[PP.lens['tamron18x'], 'Tamron18X lens']]
		text += self.create_select_label('Lens Type :', 'lens_type', options, \
				self.af_params['lens_type']) + '&nbsp; \n'
		options = [[PP.af_mode['caf'], 'Continuous AF'], [PP.af_mode['saf'], 'Single AF'], \
				[PP.af_mode['man'], 'Manual Focus'], [PP.af_mode['calib'], 'Calibration']]
		text += self.create_select_label('Focus Mode :', 'af_mode', options, \
				self.af_params['af_mode']) + '<br><br>\n'
		options = [[0, ''], [], []]
		options = [[0, 'Wide']]
		for i in range(1, 31):
			options.append([i, '%d' % i])
		options.append([31, 'Tele'])
		text += self.create_select_label('Zoom Distance :', 'zm_dist', options, \
				self.af_params['zm_dist']) + '<br><br>\n'
		text += self.create_text_entry('Focus Range (50~50000 cm)  Near :', \
				'fs_near', self.af_params['fs_near'], 5) + '&nbsp; &nbsp; '
		text += self.create_text_entry('Far :', 'fs_far', self.af_params['fs_far'], 5) + '<br><br>\n'
		text += self.add_manual_focus()
#		text += self.create_slider_bar('Slider Bar', 'slider_bar', 20, (-100, 100), 'barBox')
		text += '<br><br></fieldset><br>\n'
		return text

	def add_manual_focus(self):
		text = '<fieldset><legend>Manual Setting</legend><br>\n'
		text += self.create_text_entry('Focus Distance (50~50000 cm) :', 'fs_dist', \
				self.af_params['fs_dist'], 5) + '<br><br>\n'
		text += '</fieldset><br>\n'
		return text


def main():
	support_cgi()
	page = AmbaIQAFPage('Image Quality', 'Auto Focus')
	page.show_params()
	return

if __name__ == '__main__':
	main()

