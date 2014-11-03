/*
****************************************************************************
*
** \file      base_page.c
**
** \version   $Id$
**
** \brief     
**
** \attention THIS SAMPLE CODE IS PROVIDED AS IS. GOFORTUNE SEMICONDUCTOR
**            ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR 
**            OMMISSIONS.
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
****************************************************************************
*/
#include "include.h"



int AmbaCamBasicPage_process_PostData () 
{
	int vin_fps_changed = 0;
	int vv_changed = 0;
	AmbaTransfer transfer;
	Message msg;
	transfer_init(&transfer);

	Message image_data;
	Message vv_data;

	int i = 0;
	int req_cnt = get_value(postInfo, "req_cnt");
	if ((strstr(postInfo, "req_cnt") != NULL) \
		&& (strstr(postInfo, "sec") != NULL)\
		&& (strstr(postInfo, "data") != NULL)) {
		char* string_buffer = NULL;
		string_buffer = (char*)malloc(MSG_INFO_LEN);
		if (string_buffer == NULL) {
			LOG_MESSG("CamBasic post data error");
			return -1;
		}
		memset(string_buffer, 0, MSG_INFO_LEN);
		url_decode(string_buffer, postInfo, strlen(postInfo));
		for(i = 0; i < req_cnt; i++) {

			memset(&msg, 0, sizeof(Message));

			parse_postSec(string_buffer,&msg ,i);
			parse_postData(string_buffer,&msg ,i);

			if (strcmp(msg.section_Name, "IMAGE") == 0) {
				memset(&image_data, 0, sizeof(Message));
				vin_fps_changed = 1;
				strncat(image_data.section_Name, msg.section_Name, strlen(msg.section_Name));
				strncat(image_data.msg, msg.msg, strlen(msg.msg));

			}else if (strcmp(msg.section_Name, "VINVOUT") == 0){
				memset(&vv_data, 0, sizeof(Message));
				vv_changed = 1;
				strncat(vv_data.section_Name, msg.section_Name, strlen(msg.section_Name));
				strncat(vv_data.msg, msg.msg, strlen(msg.msg));

			}
		}
		free(string_buffer);
		string_buffer = NULL;

		if (vin_fps_changed) {
			memset(&msg, 0, sizeof(Message));
			strcat(msg.section_Name, "IMAGE");
			strcat(msg.msg, "slow_shutter = 0");

			if ((transfer.send_set_request(REQ_SET_PARAM, IMAGE, msg) < 0)) {
				return -1;
			}
		}
		if (vv_changed) {
			if ((transfer.send_set_request(REQ_SET_PARAM, VINVOUT, vv_data)) < 0) {
				return -1;
			}
		}
		else {
			return -1;
		}
		if (vin_fps_changed) {
			strcat(image_data.msg, "slow_shutter = 1");
			if ((transfer.send_set_request(REQ_SET_PARAM, IMAGE, image_data)) < 0) {
				return -1;
			}
		}
	return 0;
	}
	else {
		return 1;
	}
}

