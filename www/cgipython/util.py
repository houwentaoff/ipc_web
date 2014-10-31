#!/usr/bin/env python

##############################################
#
# util.py
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

import struct, socket, re

req_format = '3I'
ack_format = '2I'


def support_cgi():
	print 'Content-Type: text/html'
	print


class AmbaPack(object):
	''' This is the class to build or parse pack for communicating with server '''

	def pack_req(self, Id, info, dataSize = 0) :
		pack = struct.pack(req_format, Id, info, dataSize)
		return pack

	def parse_ack(self, pack) :
		ack = dict( zip( ('result', 'info'), (struct.unpack(ack_format, pack)) ) )
		return ack

	def parse_data(self, pack) :
		data = {}
		lines = re.split('\n', pack)
		for line in lines:
			pairs = re.split(' = ',line)
			if len(pairs) == 2 :
				if pairs[1].find('"') >= 0:
					data.update({ pairs[0] : pairs[1] })
				else :
					data.update({ pairs[0] : int(pairs[1]) })
		return data


class AmbaSocket(object):
	''' This class encapsulate the socket class '''

	IP = ''		# Symbolic name meaning the local host
	PORT = 20000

	def __init__(self, host = 0, port = 0):
		if host == 0:
			host = self.IP
		if port == 0:
			port = self.PORT
		self.host = host
		self.port = port
		self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		return

	def __del__(self):
		self.s.close()
		return

	def connect_host(self):
		return self.s.connect_ex((self.host, self.port))

	def send_stream(self, byteStream):
		self.s.send(byteStream)
		return

	def recv_stream(self, size):
		byteStream = self.s.recv(size, socket.MSG_WAITALL)
		return byteStream


class AmbaTransfer(object):
	''' This is the class to transfer info between web server and client '''

	ENCODE_PORT = 20000
	IMAGE_PORT = 20002

	RequestId = {
		'REQ_SET_FORCEIDR' : 17,
		'REQ_GET_VIDEO_PORT' : 18,
		'REQ_STREAM_START' : 19,
		'REQ_STREAM_STOP' : 20,
		'REQ_CHANGE_BR' : 21,
		'REQ_CHANGE_FR' : 22,
		'REQ_CHANGE_BRC' : 23,
		'REQ_GET_PARAM' : 100,
		'REQ_SET_PARAM' : 101,
		'REQ_AAA_CONTROL' : 102
	}

	SectionPort = {
		'LIVE' : ENCODE_PORT,
		'DPTZ' : ENCODE_PORT,
		'ENCODE' : ENCODE_PORT,
		'PRIMASK' : ENCODE_PORT,
		'OSD' : ENCODE_PORT,
		'VINVOUT' : ENCODE_PORT,
		'STREAM0' : ENCODE_PORT,
		'STREAM1' : ENCODE_PORT,
		'STREAM2' : ENCODE_PORT,
		'STREAM3' : ENCODE_PORT,
		'IMAGE' : IMAGE_PORT,
		'IQAF' : IMAGE_PORT,
	}

	def __init__(self):
		self.pack = AmbaPack()
		return

	def recv_ack(self, conn):
		size = struct.calcsize(ack_format)
		stream = conn.recv_stream(size)
		return self.pack.parse_ack(stream)

	def send_get_request(self, Request, Section, Info = ''):
		if not ( Request in self.RequestId or Section in self.SectionPort.keys() ):
			return
		data = {}
		conn = AmbaSocket(port = self.SectionPort[Section])
		req = self.pack.pack_req(self.RequestId[Request], Info, len(Section))
		if conn.connect_host() == 0 :
			conn.send_stream(req + Section)
			# wait ack
			ack_size = struct.calcsize(ack_format)
			recvBuf = conn.recv_stream(ack_size)
			ack = self.pack.parse_ack(recvBuf)
			# receive data
			if ack["result"] == 0 and ack["info"] > 0 :
				recvBuf = conn.recv_stream(ack["info"])
				data = self.pack.parse_data(recvBuf)
		return data

	def send_set_request(self, Request, Section, Info=''):
		if not ( Request in self.RequestId or Section in self.SectionPort.keys() ):
			return
		conn = AmbaSocket(port = self.SectionPort[Section])
		req = self.pack.pack_req(self.RequestId[Request], len(Info), len(Section))
		if conn.connect_host() == 0 :
			conn.send_stream(req + Section)
			# wait ack
			ack_size = struct.calcsize(ack_format)
			recvBuf = conn.recv_stream(ack_size)
			ack = self.pack.parse_ack(recvBuf)
			# send data
			if ack["result"] == 0 and Info != '':
				conn.send_stream(Info)
				#wait ack
				recvBuf = conn.recv_stream(ack_size)
				ack = self.pack.parse_ack(recvBuf)
			return ack["result"]
		else :
			return 1

	def send_fly_request(self, Request, info, data = 0):
		if not ( Request in self.RequestId ):
			return
		conn = AmbaSocket()
		req = self.pack.pack_req(self.RequestId[Request], info, data)
		if conn.connect_host() == 0 :
			conn.send_stream(req)
			# wait ack
			ack_size = struct.calcsize(ack_format)
			recvBuf = conn.recv_stream(ack_size)
			ack = self.pack.parse_ack(recvBuf)
		return ack["result"]


def main():
	trans = AmbaTransfer()
	for section in trans.SectionName:
		data = trans.send_get_request('REQ_GET_PARAM', section)
		print 'Section : %s \nData : \n%s\n' % (section, data)
	return

if __name__ == '__main__':
	main()

