#!/usr/bin/env python

##############################################
#
# params.py
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

class ADDAFTER:
	''' This class simulate 'i++' operator '''

	def __init__(self, value = 0):
		self.value = value

	def adda(self):
		value = self.value
		self.value = value + 1
		return value
	

class PARAM:
	''' This class includes all pre-defined params '''
	
	def __init__(self):
		self.create_all_params()

	def create_all_params(self):
		self.add_request_id()
		self.add_video_fps()
		self.add_bitrate_control()
		self.add_gop_model()
		self.add_profile_type()
		self.add_video_mode()
		self.add_vout_type()
		self.add_enc_mode()
		self.add_enc_resolution()
		self.add_enc_high_mega_resolution()
		self.add_shutter_limit()
		self.add_flip_rotate()
		self.add_wb_control()
		self.add_lens_id()
		self.add_focus_mode()
		self.add_gain()
		self.add_color_type()
		self.add_osd_text_size()
		return

	def create_res(self, width, height):
		return (width << 16) + height

	def create_shutter(self, shutter_time):
		return shutter_time * 100

	def create_fr(self, rotate, vflip, hflip):
		return ((rotate << 2) | (vflip << 1) | (hflip))
	
	def add_request_id(self):
		self.req = {}
		cnt = ADDAFTER(3)
		self.req['getversion'] = cnt.adda()
		self.req['getinfo'] = cnt.adda()
		self.req['getcambasic'] = cnt.adda()
		self.req['setcambasic'] = cnt.adda()
		self.req['getcamadv'] = cnt.adda()
		self.req['setcamadv'] = cnt.adda()
		self.req['getenc'] = cnt.adda()
		self.req['setenc'] = cnt.adda()
		self.req['getmd'] = cnt.adda()
		self.req['setmd'] = cnt.adda()
		self.req['getpt'] = cnt.adda()
		self.req['setpt'] = cnt.adda()
		self.req['getpm'] = cnt.adda()
		self.req['setpm'] = cnt.adda()
		self.req['getrec'] = cnt.adda()
		self.req['setrec'] = cnt.adda()
		self.req['setforceidr'] = cnt.adda()

	def add_video_fps(self):
		self.fps = {}
		self.fps['auto'] = 0
		self.fps['1'] = 512000000
		self.fps['2'] = 256000000
		self.fps['3'] = 170666667
		self.fps['4'] = 128000000
		self.fps['5'] = 102400000
		self.fps['6'] = 85333333
		self.fps['10'] = 51200000
		self.fps['13'] = 39384615
		self.fps['15'] = 34133333
		self.fps['20'] = 25600000
		self.fps['24'] = 21333333
		self.fps['25'] = 20480000
		self.fps['29.97'] = 17083750
		self.fps['30'] = 17066667
		self.fps['50'] = 10240000
		self.fps['59.94'] = 8541875
		self.fps['60'] = 8533333

	def add_bitrate_control(self):
		self.brc = {}
		self.brc['cbr'] = 0
		self.brc['vbr'] = 1
		self.brc['cbrq'] = 2
		self.brc['vbrq'] = 3

	def add_gop_model(self):
		self.gopm = {}
		self.gopm['simple'] = 0
		self.gopm['adv'] = 1

	def add_profile_type(self):
		self.profile = {}
		self.profile['main'] = 0
		self.profile['baseline'] = 1

	def add_video_mode(self):
		self.video_mode = {}
		self.video_mode['auto'] = 0
		self.video_mode['640x480'] = 8
		self.video_mode['800x600'] = 9
		self.video_mode['1024x768'] = 10
		self.video_mode['1280x1024'] = 11
		self.video_mode['1600x1200'] = 12
		self.video_mode['1280x960'] = 16
		self.video_mode['480I'] = 0xFFF0
		self.video_mode['576I'] = 0xFFF1
		self.video_mode['480P'] = 0xFFF2
		self.video_mode['576P'] = 0xFFF3
		self.video_mode['720P'] = 0xFFF4
		self.video_mode['1080I'] = 0xFFF6
		self.video_mode['1080P'] = 0xFFF8
		self.video_mode['1080P30'] = 0xFFFD

	def add_vout_type(self):
		self.vout_type = {}
		self.vout_type['off'] = 0
		self.vout_type['cvbs'] = ((0 << 16) | (1 << 0))
		self.vout_type['hdmi'] = ((0 << 16) | (1 << 2))

	def add_enc_mode(self):
		self.enc_mode = {}
		self.enc_mode['high-fps'] = 0
		self.enc_mode['high-mp'] = 1
		self.enc_mode['low-delay'] = 2

	def add_enc_resolution(self):
		self.res = {}
		self.res['1920 x1080'] = self.create_res(1920, 1080)
#		self.res['1600 x1200'] = self.create_res(1600, 1200)
		self.res['1440 x1080'] = self.create_res(1440, 1080)
		self.res['1280 x1024'] = self.create_res(1280, 1024)
		self.res['1280 x 960'] = self.create_res(1280, 960)
		self.res['1280 x 720'] = self.create_res(1280, 720)
#		self.res['1024 x 768'] = self.create_res(1024, 768)
		self.res[' 800 x 600'] = self.create_res(800, 600)
		self.res[' 720 x 576'] = self.create_res(720, 576)
		self.res[' 720 x 480'] = self.create_res(720, 480)
		self.res[' 640 x 480'] = self.create_res(640, 480)
		self.res[' 352 x 288'] = self.create_res(352, 288)
		self.res[' 352 x 240'] = self.create_res(352, 240)
		self.res[' 320 x 240'] = self.create_res(320, 240)
		self.res[' 320 x 240'] = self.create_res(320, 240)
		self.res[' 176 x 144'] = self.create_res(176, 144)
		self.res[' 176 x 120'] = self.create_res(176, 120)
		self.res[' 160 x 120'] = self.create_res(160, 120)

	def add_enc_high_mega_resolution(self):
		self.highRes = {}
		self.highRes['2592x1944 (5.0M)'] = self.create_res(2592, 1944)
		self.highRes['2560x1440 (3.7M)'] = self.create_res(2560, 1440)
		self.highRes['2304x1296 (3.0M)'] = self.create_res(2304, 1296)
		self.highRes['2048x1536 (3.0M)'] = self.create_res(2048, 1536)

 	def add_shutter_limit(self):
		self.shutter = {}
		self.shutter['1/8000'] = 64000
		self.shutter['1/1024'] = 500000
		self.shutter['1/960'] = 533333
		self.shutter['1/480'] = 1066667
		self.shutter['1/240'] = 2133333
		self.shutter['1/120'] = 4266667
		self.shutter['1/100'] = 5120000
		self.shutter['1/60'] = 8533333
		self.shutter['1/50'] = 10240000
		self.shutter['1/30'] = 17066667
		self.shutter['1/25'] = 20480000
		self.shutter['1/15'] = 34133333
		self.shutter['1/7.5'] = 68266667

	def add_flip_rotate(self):
		self.fr = {}
		self.fr['normal'] = self.create_fr(0, 0, 0)
		self.fr['hflip'] = self.create_fr(0, 0, 1)
		self.fr['vflip'] = self.create_fr(0, 1, 0)
		self.fr['rotate90'] = self.create_fr(1, 0, 0)
		self.fr['rotate180'] = self.create_fr(0, 1, 1)
		self.fr['rotate270'] = self.create_fr(1, 1, 1)

	def add_wb_control(self):
		self.wb = {}
		cnt = ADDAFTER(0)
		self.wb['auto'] = cnt.adda()
		self.wb['hold'] = 100
		self.wb['Incandescent'] = cnt.adda()
		self.wb['D4000'] = cnt.adda()
		self.wb['D5000'] = cnt.adda()
		self.wb['Sunny'] = cnt.adda()
		self.wb['Cloudy'] = cnt.adda()
		self.wb['Flash'] = cnt.adda()
		self.wb['Fluorescent'] = cnt.adda()
		self.wb['Fluorescent High'] = cnt.adda()
		self.wb['Under Water'] = cnt.adda()
		self.wb['Custom'] = cnt.adda()

	def add_lens_id(self):
		self.lens = {}
		self.lens['fixed_focus'] = 0
		self.lens['others'] = 100
		self.lens['tamron18x'] = 101
		self.lens['jcd661'] = 102

	def add_focus_mode(self):
		self.af_mode = {}
		self.af_mode['caf'] = 0
		self.af_mode['saf'] = 1
		self.af_mode['man'] = 3
		self.af_mode['calib'] = 4
		self.af_mode['debug'] = 5

	def add_gain(self):
		self.gain = {}
		self.gain['auto'] = 0
		self.gain['0db'] = 0
		self.gain['3db'] = 3
		self.gain['6db'] = 6
		self.gain['9db'] = 9
		self.gain['12db'] = 12
		self.gain['15db'] = 15
		self.gain['18db'] = 18
		self.gain['24db'] = 24
		self.gain['30db'] = 30
		self.gain['36db'] = 36
		self.gain['42db'] = 42
		self.gain['48db'] = 48
		self.gain['54db'] = 54
		self.gain['60db'] = 60

	def add_focus_distance(self):
		self.fs_dist = {}

	def add_color_type(self):
		self.color = {}
		cnt = ADDAFTER(0)
		self.color['black'] = cnt.adda()
		self.color['red'] = cnt.adda()
		self.color['blue'] = cnt.adda()
		self.color['green'] = cnt.adda()
		self.color['yellow'] = cnt.adda()
		self.color['magenta'] = cnt.adda()
		self.color['cyan'] = cnt.adda()
		self.color['white'] = cnt.adda()

	def add_osd_text_size(self):
		self.ftsize = {}
		self.ftsize['smaller'] = 16
		self.ftsize['small'] = 24
		self.ftsize['normal'] = 32
		self.ftsize['large'] = 40
		self.ftsize['larger'] = 48

	def test(self):
		print '\nrequest list : \n', self.req
		print '\video fps : \n', self.fps
		print '\nbitrate control type : \n', self.brc
		print '\ngop model :\n', self.gopm
		print '\nprofile :\n', self.profile
		print '\nvideo mode : \n', self.video_mode
		print '\nvout type : \n', self.vout_type
		print '\nencode mode : \n', self.enc_mode
		print '\nencode resolution : \n', self.res
		print '\nhigh mega encode resolution : \n', self.highRes
		print '\nshutter time : \n', self.shutter
		print '\nwb control : \n', self.wb
		print '\nlens type : \n', self.lens
		print '\nfocus mode : \n', self.af_mode
		print '\ngain : \n', self.gain
		print '\ncolor type : \n', self.color
		print '\nosd text size : \n', self.ftsize

def main():
	sample = PARAM()
	sample.test()

if __name__ == '__main__':
	main()

