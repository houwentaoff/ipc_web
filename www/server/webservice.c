/*
****************************************************************************
*
** \file      webservice.c
**
** \version   $Id: webservice.c 2482 2014-12-04 07:45:14Z houwentao $
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

#include <getopt.h>
#include <sched.h>
#include <basetypes.h>

#include <sys/signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/ipc.h>

#include <sys/msg.h>
#include "webservice.h"
#include "video.h"
#include "ctlserver.h"

#define INOUT_DEBUG           1  	/*1 is open , o is close */
#define APP_DEBUG             1     /*1 is open , 0 is close */
#define APP_ERR               1     /*1 is open , 0 is close */

#define	MAX_ITEMS_TO_PARSE		(3*1000)

#if INOUT_DEBUG
#define FUN_IN(fmt, args...)            printf("[webservice] ===> %s()"fmt"\n", __func__, ##args)/*  */
#define FUN_OUT(fmt, args...)           printf("[webservice] <=== %s()"fmt"\n", __func__, ##args) /*  */
#else
#define FUN_IN(fmt, args...)
#define FUN_OUT(fmt, args...)
#endif

#if APP_DEBUG
#define PRT_DBG(fmt, args...)         printf("[webservice] %s():line[%d]:"fmt"", __func__, __LINE__, ##args)/*  */
#else
#define PRT_DBG(fmt, args...)
#endif

#if APP_ERR
#define PRT_ERR(fmt, args...)                                                               \
do                                                                                          \
{                                                                                           \
    printf("[webservice] \033[5;41;32m [ERROR] ---> %s ():line[%d]:\033[0m", __func__, __LINE__);      \
    printf(" "fmt, ##args);                                                                 \
}while(0)    /* */
#else
#define PRT_ERR(fmt, args...)
#endif

static u8						g_buffer[BUFFER_SIZE];
static gk_vin_mode			vin_map;
static gk_vout_mode			vout_map;
static gk_encode_stream     stream_map[ENCODE_STREAM_NUM];
static osd_param_t				osd_map[ENCODE_STREAM_NUM];
static privacy_mask_t		pm_map;
static u8						g_mw_pm_enable = 0;
static osd_param_t				g_mw_osd[ENCODE_STREAM_NUM];

static int sockfd = -1;
static int sockfd2 = -1;

extern GONVIF_Encode_Cmd g_stEncodeInfo[4];
extern int g_slIPCMsgID;
extern IPC_XML_TYPE g_emIpcXmlType;
extern GADI_SYS_HandleT vinHandle;
extern GADI_SYS_HandleT voutHandle;
extern GADI_SYS_HandleT vencHandle;
static   GADI_SYS_HandleT osdHandle;

static int set_vinvout_param(char * section_name);
static int get_vinvout_param(char * section_name, u32 info);
static int get_stream_param(char * section_name, u32 info);
static int set_stream_param(char * section_name);
static int get_encode_param(char * section_name, u32 info);
static int set_encode_param(char * section_name);
static int set_osd_param(char * section_name);
static int set_pm_param(char * section_name);

static Mapping VinVoutMap[] = {
	{"vin_enable",			&vin_map.enable,				MAP_TO_U32,	0.0,		0,		0.0,		0.0,	false},
	{"vin_mode",			&vin_map.mode,				MAP_TO_U32,	AMBA_VIDEO_MODE_AUTO,		0,		0.0,		0.0,	false},
	{"vin_framerate",		&vin_map.frame_rate,			MAP_TO_U32,	AMBA_VIDEO_FPS_29_97,		0,		0.0,		0.0,	false},
//	{"vin_mirror",			&vin_map.mirror_mode.mirror_pattern,	MAP_TO_U32,	0.0,		0,		0.0,		0.0,	},
//	{"vin_bayer",			&vin_map.mirror_mode.bayer_pattern,	MAP_TO_U32,	0.0,		0,		0.0,		0.0,	},

	{"vout_type",			&vout_map.type,			MAP_TO_U32,	AMBA_VOUT_SINK_TYPE_CVBS,		0,		0.0,		0.0,	false},
	{"vout_mode",			&vout_map.mode,			MAP_TO_U32,	AMBA_VIDEO_MODE_480I,		0,		0.0,		0.0,	false},

	{NULL,			NULL,						-1,	0.0,					0,	0.0,	0.0,		0},
};


static Mapping Stream0[] = {
	{"s0_h264_id",			&stream_map[0].h264Conf.streamId,				MAP_TO_U32,     0.0,		0,		0.0,		0.0,	false},
	{"s0_M",				&stream_map[0].h264Conf.gopM,					MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
	{"s0_N",				&stream_map[0].h264Conf.gopN,					MAP_TO_U8,	    30.0,		0,		0.0,		0.0,	false},
	{"s0_idr_interval",		&stream_map[0].h264Conf.idrInterval,		MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
//	{"s0_gop_model",		&stream_map[0].h264.gop_model,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s0_profile",			&stream_map[0].h264.profile,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s0_brc",				&stream_map[0].h264.brc_mode,	        MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
	{"s0_cbr_avg_bps",		&stream_map[0].h264Conf.cbrAvgBps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	false},
//	{"s0_vbr_min_bps",		&stream_map[0].h264.vbr_min_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
//	{"s0_vbr_max_bps",		&stream_map[0].h264.vbr_max_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
	{"s0_quality",			&stream_map[0].mjpegConf.quality,			MAP_TO_U8,	    88.0,	    0,		0.0,		0.0,	false},
	{NULL,			        NULL,						            -1,	            0.0,		0,	    0.0,	    0.0,	false},
};

static Mapping Stream1[] = {
	{"s1_h264_id",			&stream_map[1].h264Conf.streamId,				MAP_TO_U32,	    1.0,		0,		0.0,		0.0,	false},
	{"s1_M",				&stream_map[1].h264Conf.gopM,					MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
	{"s1_N",				&stream_map[1].h264Conf.gopN,					MAP_TO_U8,	    30.0,		0,		0.0,		0.0,	false},
	{"s1_idr_interval",		&stream_map[1].h264Conf.idrInterval,		MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
//	{"s1_gop_model",		&stream_map[1].h264.gop_model,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s1_profile",			&stream_map[1].h264.profile,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s1_brc",				&stream_map[1].h264.brc_mode,	        MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
	{"s1_cbr_avg_bps",		&stream_map[1].h264Conf.cbrAvgBps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	false},
//	{"s1_vbr_min_bps",		&stream_map[1].h264.vbr_min_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
//	{"s1_vbr_max_bps",		&stream_map[1].h264.vbr_max_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
	{"s1_quality",			&stream_map[1].mjpegConf.quality,			MAP_TO_U8,	    88.0,	    0,		0.0,		0.0,	},
	{NULL,			        NULL,						            -1,	            0.0,		0,	    0.0,	    0.0,	false},
};

static Mapping Stream2[] = {
	{"s2_h264_id",			&stream_map[2].h264Conf.streamId,				MAP_TO_U32,	    2.0,		0,		0.0,		0.0,	false},
	{"s2_M",				&stream_map[2].h264Conf.gopM,					MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
	{"s2_N",				&stream_map[2].h264Conf.gopN,					MAP_TO_U8,	    30.0,		0,		0.0,		0.0,	false},
	{"s2_idr_interval",		&stream_map[2].h264Conf.idrInterval,		MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
//	{"s2_gop_model",		&stream_map[2].h264.gop_model,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s2_profile",			&stream_map[2].h264.profile,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s2_brc",				&stream_map[2].h264.brc_mode,	        MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
	{"s2_cbr_avg_bps",		&stream_map[2].h264Conf.cbrAvgBps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	false},
//	{"s2_vbr_min_bps",		&stream_map[2].h264.vbr_min_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
//	{"s2_vbr_max_bps",		&stream_map[2].h264.vbr_max_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
	{"s2_quality",			&stream_map[2].mjpegConf.quality,			MAP_TO_U8,	    88.0,	    0,		0.0,		0.0,	false},
	{NULL,			        NULL,						            -1,	            0.0,	    0,	    0.0,        0.0,    false},
};

static Mapping Stream3[] = {
	{"s3_h264_id",			&stream_map[3].h264Conf.streamId,				MAP_TO_U32,	    3.0,		0,		0.0,		0.0,	false},
	{"s3_M",				&stream_map[3].h264Conf.gopM,					MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
	{"s3_N",				&stream_map[3].h264Conf.gopN,					MAP_TO_U8,	    30.0,		0,		0.0,		0.0,	false},
	{"s3_idr_interval",		&stream_map[3].h264Conf.idrInterval,		MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
//	{"s3_gop_model",		&stream_map[3].h264.gop_model,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s3_profile",			&stream_map[3].h264.profile,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s3_brc",				&stream_map[3].h264.brc_mode,	        MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
	{"s3_cbr_avg_bps",		&stream_map[3].h264Conf.cbrAvgBps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	false},
//	{"s3_vbr_min_bps",		&stream_map[3].h264.vbr_min_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
//	{"s3_vbr_max_bps",		&stream_map[3].h264.vbr_max_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
	{"s3_quality",			&stream_map[3].mjpegConf.quality,			MAP_TO_U8,	    88.0,	    0,		0.0,		0.0,	false},
	{NULL,			        NULL,						            -1,	            0.0,	    0,	    0.0,	    0.0,    false},
};

static Mapping EncodeMap[] = {
//	{"enc_mode",			&encode_mode,							MAP_TO_U32,	0.0,		0,		0.0,		0.0,	},

	{"s0_dptz",			&stream_map[0].dptz,							MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s0_type",			&stream_map[0].streamFormat.encode_type,			MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s0_flip_rotate",		&stream_map[0].streamFormat.flip_rotate,			MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s0_width",			&stream_map[0].streamFormat.encode_width,		MAP_TO_U16,	0.0,		0,		0.0,		0.0,	false},
	{"s0_height",			&stream_map[0].streamFormat.encode_height,		MAP_TO_U16,	0.0,		0,		0.0,		0.0,	false},
	{"s0_enc_fps",		&stream_map[0].streamFormat.encode_fps,			MAP_TO_U32,	30.0,		0,		0.0,		0.0,	false},

	{"s1_dptz",			&stream_map[1].dptz,							MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s1_type",			&stream_map[1].streamFormat.encode_type,			MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s1_flip_rotate",		&stream_map[1].streamFormat.flip_rotate,			MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s1_width",			&stream_map[1].streamFormat.encode_width,		MAP_TO_U16,	0.0,		0,		0.0,		0.0,	false},
	{"s1_height",			&stream_map[1].streamFormat.encode_height,		MAP_TO_U16,	0.0,		0,		0.0,		0.0,	false},
	{"s1_enc_fps",		&stream_map[1].streamFormat.encode_fps,			MAP_TO_U32,	30.0,		0,		0.0,		0.0,	false},

	{"s2_dptz",			&stream_map[2].dptz,							MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s2_type",			&stream_map[2].streamFormat.encode_type,			MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s2_flip_rotate",		&stream_map[2].streamFormat.flip_rotate,			MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s2_width",			&stream_map[2].streamFormat.encode_width,		MAP_TO_U16,	0.0,		0,		0.0,		0.0,	false},
	{"s2_height",			&stream_map[2].streamFormat.encode_height,		MAP_TO_U16,	0.0,		0,		0.0,		0.0,	false},
	{"s2_enc_fps",		&stream_map[2].streamFormat.encode_fps,			MAP_TO_U32,	30.0,		0,		0.0,		0.0,	false},

	{"s3_dptz",			&stream_map[3].dptz,							MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s3_type",			&stream_map[3].streamFormat.encode_type,			MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s3_flip_rotate",		&stream_map[3].streamFormat.flip_rotate,			MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s3_width",			&stream_map[3].streamFormat.encode_width,		MAP_TO_U16,	0.0,		0,		0.0,		0.0,	false},
	{"s3_height",			&stream_map[3].streamFormat.encode_height,		MAP_TO_U16,	0.0,		0,		0.0,		0.0,	false},
	{"s3_enc_fps",		&stream_map[3].streamFormat.encode_fps,			MAP_TO_U32,	30.0,		0,		0.0,		0.0,	false},

	{NULL,			        NULL,						            -1,	            0.0,	    0,	    0.0,	    0.0,    },
};
static Mapping OSDMap[] = {
	{"s0_bmp_enable",		&osd_map[0].bmp_enable,		MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s0_time_enable",		&osd_map[0].time_enable,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s0_text_enable",		&osd_map[0].text_enable,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s0_no_rotate",		&osd_map[0].no_rotate,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s0_text",				&osd_map[0].text.str,		MAP_TO_STRING,	0.0,		0,		0.0,		0.0,		OSD_TEXT_LENGTH,	false},
	{"s0_text_type",		&osd_map[0].text.type,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		5,	false},
	{"s0_text_size",		&osd_map[0].text.size,	MAP_TO_U8,	32.0,		MIN_MAX_LIMIT,		0.0,		100,	false},
	{"s0_text_outline",		&osd_map[0].text.outline,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		10.0,	false},
	{"s0_text_color",		&osd_map[0].text.color,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		COLOR_TYPE_NUM,false},
	{"s0_text_bold",		&osd_map[0].text.bold,	MAP_TO_S32,	0.0,		NO_LIMIT,			0.0,		0.0,	false},
	{"s0_text_italic",		&osd_map[0].text.italic,	MAP_TO_S32,	0.0,		NO_LIMIT,			0.0,		0.0,	false},
	{"s0_text_startx",		&osd_map[0].text.start_x,	MAP_TO_U16,	0.0,		MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"s0_text_starty",		&osd_map[0].text.start_y,	MAP_TO_U16,	0.0,		MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"s0_text_boxw",		&osd_map[0].text.box_w,	MAP_TO_U16,	50,0,	MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"s0_text_boxh",		&osd_map[0].text.box_h,	MAP_TO_U16,	50,0,	MIN_MAX_LIMIT,		0.0,		100.0,	false},

	{"s1_bmp_enable",		&osd_map[1].bmp_enable,		MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s1_time_enable",		&osd_map[1].time_enable,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s1_text_enable",		&osd_map[1].text_enable,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s1_no_rotate",		&osd_map[1].no_rotate,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s1_text",				&osd_map[1].text.str,		MAP_TO_STRING,	0.0,		0,		0.0,		0.0,		OSD_TEXT_LENGTH,	false},
	{"s1_text_type",		&osd_map[1].text.type,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		5,	false},
	{"s1_text_size",		&osd_map[1].text.size,	MAP_TO_U8,	32.0,		MIN_MAX_LIMIT,		0.0,		100,	false},
	{"s1_text_outline",		&osd_map[1].text.outline,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		10.0,	false},
	{"s1_text_color",		&osd_map[1].text.color,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		COLOR_TYPE_NUM,false},
	{"s1_text_bold",		&osd_map[1].text.bold,	MAP_TO_S32,	0.0,		NO_LIMIT,			0.0,		0.0,	false},
	{"s1_text_italic",		&osd_map[1].text.italic,	MAP_TO_S32,	0.0,		NO_LIMIT,			0.0,		0.0,	false},
	{"s1_text_startx",		&osd_map[1].text.start_x,	MAP_TO_U16,	0.0,		MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"s1_text_starty",		&osd_map[1].text.start_y,	MAP_TO_U16,	0.0,		MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"s1_text_boxw",		&osd_map[1].text.box_w,	MAP_TO_U16,	50,0,	MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"s1_text_boxh",		&osd_map[1].text.box_h,	MAP_TO_U16,	50,0,	MIN_MAX_LIMIT,		0.0,		100.0,	false},

	{"s2_bmp_enable",		&osd_map[2].bmp_enable,		MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s2_time_enable",		&osd_map[2].time_enable,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s2_text_enable",		&osd_map[2].text_enable,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s2_no_rotate",		&osd_map[2].no_rotate,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s2_text",				&osd_map[2].text.str,		MAP_TO_STRING,	0.0,		0,		0.0,		0.0,		OSD_TEXT_LENGTH,	false},
	{"s2_text_type",		&osd_map[2].text.type,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		5,	false},
	{"s2_text_size",		&osd_map[2].text.size,	MAP_TO_U8,	32.0,		MIN_MAX_LIMIT,		0.0,		100,	false},
	{"s2_text_outline",		&osd_map[2].text.outline,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		10.0,	false},
	{"s2_text_color",		&osd_map[2].text.color,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		COLOR_TYPE_NUM,false},
	{"s2_text_bold",		&osd_map[2].text.bold,	MAP_TO_S32,	0.0,		NO_LIMIT,			0.0,		0.0,	false},
	{"s2_text_italic",		&osd_map[2].text.italic,	MAP_TO_S32,	0.0,		NO_LIMIT,			0.0,		0.0,	false},
	{"s2_text_startx",		&osd_map[2].text.start_x,	MAP_TO_U16,	0.0,		MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"s2_text_starty",		&osd_map[2].text.start_y,	MAP_TO_U16,	0.0,		MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"s2_text_boxw",		&osd_map[2].text.box_w,	MAP_TO_U16,	50,0,	MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"s2_text_boxh",		&osd_map[2].text.box_h,	MAP_TO_U16,	50,0,	MIN_MAX_LIMIT,		0.0,		100.0,	false},

	{"s3_bmp_enable",		&osd_map[3].bmp_enable,		MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s3_time_enable",		&osd_map[3].time_enable,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s3_text_enable",		&osd_map[3].text_enable,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s3_no_rotate",		&osd_map[3].no_rotate,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s3_text",				&osd_map[3].text.str,		MAP_TO_STRING,	0.0,		0,		0.0,		0.0,		OSD_TEXT_LENGTH,	false},
	{"s3_text_type",		&osd_map[3].text.type,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		5,	false},
	{"s3_text_size",		&osd_map[3].text.size,	MAP_TO_U8,	32.0,		MIN_MAX_LIMIT,		0.0,		100,	false},
	{"s3_text_outline",		&osd_map[3].text.outline,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		10.0,	false},
	{"s3_text_color",		&osd_map[3].text.color,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		COLOR_TYPE_NUM,false},
	{"s3_text_bold",		&osd_map[3].text.bold,	MAP_TO_S32,	0.0,		NO_LIMIT,			0.0,		0.0,	false},
	{"s3_text_italic",		&osd_map[3].text.italic,	MAP_TO_S32,	0.0,		NO_LIMIT,			0.0,		0.0,	false},
	{"s3_text_startx",		&osd_map[3].text.start_x,	MAP_TO_U16,	0.0,		MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"s3_text_starty",		&osd_map[3].text.start_y,	MAP_TO_U16,	0.0,		MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"s3_text_boxw",		&osd_map[3].text.box_w,	MAP_TO_U16,	50,0,	MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"s3_text_boxh",		&osd_map[3].text.box_h,	MAP_TO_U16,	50,0,	MIN_MAX_LIMIT,		0.0,		100.0,	false},

	{NULL,			NULL,						-1,	0.0,					0,	0.0,	0.0,		0},

};
static Mapping PMMap[] = {
	{"pm_x",			&pm_map.x,		MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"pm_y",			&pm_map.y,		MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"pm_w",				&pm_map.width,		MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"pm_h",				&pm_map.height,		MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		100.0,	false},
	{"pm_color",			&pm_map.color,		MAP_TO_U32,	0.0,		MIN_MAX_LIMIT,		0.0,		
COLOR_TYPE_NUM,	false},
	{"pm_action",			&pm_map.action,		MAP_TO_U32,	0.0,		MIN_MAX_LIMIT,		0.0,		
1,	false},

	{NULL,			NULL,						-1,	0.0,					0,	0.0,	0.0,		false},
};
static Section Params[] = {
	{"VINVOUT",		VinVoutMap,		get_vinvout_param,		set_vinvout_param},
	{"ENCODE",		EncodeMap,		get_encode_param,		set_encode_param},
	{"STREAM0",		Stream0,		get_stream_param,		set_stream_param},
	{"STREAM1",		Stream1,		get_stream_param,		set_stream_param},
	{"STREAM2",		Stream2,		get_stream_param,		set_stream_param},
	{"STREAM3",		Stream3,		get_stream_param,		set_stream_param},
	{"PRIMASK",		PMMap,			get_func_null,		set_pm_param},
	{"OSD",			OSDMap,			get_func_null,		set_osd_param},
//	{"DPTZ",		DPTZMap,		get_dptz_param,		set_dptz_param},
//	{"LIVE",		LIVEMap,		get_live_param,		set_func_null},
	{NULL,			NULL,			NULL,				NULL}
};

int send_fly_request(enum CAMCONTROL_CMDTYPE cmdtype, const unsigned char datalen,unsigned char *pCmdData, unsigned char revdatalen, unsigned char *pRevData)
{
    FUN_IN();
    int retVal;
    int g_cltSockFd;
	MSG_INFO_t  stMsgUserInfo;
	memset(&stMsgUserInfo, 0, sizeof(stMsgUserInfo));

	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(client_addr));

	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(CAMCONTROL_SERVER_PORT);
	client_addr.sin_addr.s_addr = inet_addr(HOST);
    g_cltSockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (g_cltSockFd == -1)
    {
		return -1;
	}
    /* connect to server */
    retVal = connect(g_cltSockFd, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
	if (retVal == -1)
    {
        close(g_cltSockFd);
        printf("connect() socket connect faild!\n");
		return -1;
	}

	stMsgUserInfo.ucCmdType = cmdtype;
	stMsgUserInfo.ucDataLen = datalen;
	if(pCmdData != NULL){
		memcpy(stMsgUserInfo.ucCmdData, pCmdData, datalen);
	}

	/*send msg*/
	if(send(g_cltSockFd,& stMsgUserInfo, sizeof(MSG_INFO_t), 0) == -1)
    {
		perror("msgsnd failed !\n");
        close(g_cltSockFd);
		return -2;
	}

	/*receive action*/
	memset(&stMsgUserInfo, 0, sizeof(stMsgUserInfo));
	if(recv(g_cltSockFd, &stMsgUserInfo, sizeof(MSG_INFO_t), MSG_WAITALL) == -1)
    {
        close(g_cltSockFd);
		perror("msgrcv failed !\n");
		return -1;
	}
	if(stMsgUserInfo.AckValue != 0)
    {
//		pthread_mutex_unlock(&g_msgMutex);
        close(g_cltSockFd);
		return -2;
	}

	if(revdatalen != stMsgUserInfo.ucDataLen)
    {
		PRT_ERR("AckDataLen:%d,revdatalen:%d\n",stMsgUserInfo.ucDataLen,revdatalen);
        close(g_cltSockFd);
//		pthread_mutex_unlock(&g_msgMutex);
		return -3;
	}
	if(stMsgUserInfo.ucDataLen != 0)
    {
		if(pRevData != NULL)
        {
			memcpy(pRevData, stMsgUserInfo.ucCmdData, stMsgUserInfo.ucDataLen);
		}

	}
    close(g_cltSockFd);
    FUN_OUT();
    return (0);
}

static int check_params(Mapping * Map)
{
	int i = 0;

    FUN_IN();

	while (Map[i].TokenName != NULL) {
		switch (Map[i].Type) {
		case MAP_TO_U32:
			if (Map[i].ParamLimits == MIN_MAX_LIMIT) {
				if ((*(u32 *)(Map[i].Address) < (u32)(int)(Map[i].MinLimit)) ||
					(*(u32 *)(Map[i].Address) > (u32)(int)(Map[i].MaxLimit))) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be in [%d, %d] range.\n"
						"Use default value [%d]\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].MaxLimit, (int)Map[i].Default);
					*(u32 *)(Map[i].Address) = (u32)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MIN_LIMIT) {
				if (*(u32 *)(Map[i].Address) < (u32)(int)(Map[i].MinLimit)) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be larger than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].Default);
					*(u32 *)(Map[i].Address) = (u32)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MAX_LIMIT) {
				if (*(u32 *)(Map[i].Address) > (u32)(int)(Map[i].MaxLimit)) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be smaller than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MaxLimit,
						(int)Map[i].Default);
					*(u32 *)(Map[i].Address) = (u32)(int)(Map[i].Default);
				}
			}
			break;

		case MAP_TO_U16:
			if (Map[i].ParamLimits == MIN_MAX_LIMIT) {
				if ((*(u16 *)(Map[i].Address) < (u16)(int)(Map[i].MinLimit)) ||
					(*(u16 *)(Map[i].Address) > (u16)(int)(Map[i].MaxLimit))) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be in [%d, %d] range.\n"
						"Use default value [%d]\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].MaxLimit, (int)Map[i].Default);
					*(u16 *)(Map[i].Address) = (u16)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MIN_LIMIT) {
				if (*(u16 *)(Map[i].Address) < (u16)(int)(Map[i].MinLimit)) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be larger than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].Default);
					*(u16 *)(Map[i].Address) = (u16)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MAX_LIMIT) {
				if (*(u16 *)(Map[i].Address) > (u16)(int)(Map[i].MaxLimit)) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be smaller than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MaxLimit,
						(int)Map[i].Default);
					*(u16 *)(Map[i].Address) = (u16)(int)(Map[i].Default);
				}
			}
			break;

		case MAP_TO_U8:
			if (Map[i].ParamLimits == MIN_MAX_LIMIT) {
				if ((*(u8 *)(Map[i].Address) < (u8)(int)(Map[i].MinLimit)) ||
					(*(u8 *)(Map[i].Address) > (u8)(int)(Map[i].MaxLimit))) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be in [%d, %d] range.\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].MaxLimit, (int)Map[i].Default);
					*(u8 *)(Map[i].Address) = (u8)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MIN_LIMIT) {
				if (*(u8 *)(Map[i].Address) < (u8)(int)(Map[i].MinLimit)) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be larger than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].Default);
					*(u8 *)(Map[i].Address) = (u8)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MAX_LIMIT) {
				if (*(u8 *)(Map[i].Address) > (u8)(int)(Map[i].MaxLimit)) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be smaller than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MaxLimit,
						(int)Map[i].Default);
					*(u8 *)(Map[i].Address) = (u8)(int)(Map[i].Default);
				}
			}
			break;

		case MAP_TO_S32:
			if (Map[i].ParamLimits == MIN_MAX_LIMIT) {
				if ((*(int *)(Map[i].Address) < (int)(Map[i].MinLimit)) ||
					(*(int *)(Map[i].Address) > (int)(Map[i].MaxLimit))) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be in [%d, %d] range.\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].MaxLimit, (int)Map[i].Default);
					*(int *)(Map[i].Address) = (int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MIN_LIMIT) {
				if (*(int *)(Map[i].Address) < (int)(Map[i].MinLimit)) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be larger than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].Default);
					*(int *)(Map[i].Address) = (int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MAX_LIMIT) {
				if (*(int *)(Map[i].Address) > (int)(Map[i].MaxLimit)) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be smaller than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MaxLimit,
						(int)Map[i].Default);
					*(int *)(Map[i].Address) = (int)(Map[i].Default);
				}
			}
			break;

		case MAP_TO_S16:
			if (Map[i].ParamLimits == MIN_MAX_LIMIT) {
				if ((*(s16 *)(Map[i].Address) < (s16)(int)(Map[i].MinLimit)) ||
					(*(s16 *)(Map[i].Address) > (s16)(int)(Map[i].MaxLimit))) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be in [%d, %d] range.\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].MaxLimit, (int)Map[i].Default);
					*(s16 *)(Map[i].Address) = (s16)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MIN_LIMIT) {
				if (*(s16 *)(Map[i].Address) < (s16)(int)(Map[i].MinLimit)) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be larger than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].Default);
					*(s16 *)(Map[i].Address) = (s16)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MAX_LIMIT) {
				if (*(s16 *)(Map[i].Address) > (s16)(int)(Map[i].MaxLimit)) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be smaller than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MaxLimit,
						(int)Map[i].Default);
					*(s16 *)(Map[i].Address) = (s16)(int)(Map[i].Default);
				}
			}
			break;

		case MAP_TO_S8:
			if (Map[i].ParamLimits == MIN_MAX_LIMIT) {
				if ((*(s8 *)(Map[i].Address) < (s8)(int)(Map[i].MinLimit)) ||
					(*(s8 *)(Map[i].Address) > (s8)(int)(Map[i].MaxLimit))) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be in [%d, %d] range.\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].MaxLimit, (int)Map[i].Default);
					*(s8 *)(Map[i].Address) = (s8)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MIN_LIMIT) {
				if (*(s8 *)(Map[i].Address) < (s8)(int)(Map[i].MinLimit)) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be larger than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].Default);
					*(s8 *)(Map[i].Address) = (s8)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MAX_LIMIT) {
				if (*(s8 *)(Map[i].Address) > (s8)(int)(Map[i].MaxLimit)) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be smaller than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MaxLimit,
						(int)Map[i].Default);
					*(s8 *)(Map[i].Address) = (s8)(int)(Map[i].Default);
				}
			}
			break;

		case MAP_TO_DOUBLE:
			if (Map[i].ParamLimits == MIN_MAX_LIMIT) {
				if ((*(double *)(Map[i].Address) < (Map[i].MinLimit)) ||
					(*(double *)(Map[i].Address) > (Map[i].MaxLimit))) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be in [%.2lf, %.2lf] range.\n"
						"Use default value [%.2lf].\n",
						Map[i].TokenName, Map[i].MinLimit,
						Map[i].MaxLimit, Map[i].Default);
					*(double *)(Map[i].Address) = (Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MIN_LIMIT) {
				if (*(double *)(Map[i].Address) < (Map[i].MinLimit)) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be larger than [%.2lf].\n"
						"Use default value [%.2lf].\n",
						Map[i].TokenName, Map[i].MinLimit,
						Map[i].Default);
					*(double *)(Map[i].Address) = (Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MAX_LIMIT) {
				if (*(double *)(Map[i].Address) > (Map[i].MaxLimit)) {
					PRT_ERR("Error in input parameter %s."
						"check configuration file.\n"
						"Value should be smaller than [%.2lf].\n"
						"Use default value [%.2lf].\n",
						Map[i].TokenName, Map[i].MaxLimit,
						Map[i].Default);
					*(double *)(Map[i].Address) = (Map[i].Default);
				}
			}
			break;

		default:			// string
			break;
		}
		++i;
	}

    FUN_OUT();

	return 0;
}

static int update_params(Mapping * Map)
{
    FUN_IN();
	check_params(Map);
//	memcpy(pInp, &cfg, sizeof(InputParameters));
	FUN_OUT();
	return 0;
}

static int parse_items(char **items, char *buf, int bufsize)
{
	int item = 0;
	int InString = 0, InItem = 0;
	char *p = buf;
	char *bufend = &buf[bufsize];

	while (p < bufend) {
		switch (*p) {
		case 13:
			++p;
			break;

		case '#':					// Found comment
			*p = '\0';
			while ((*p != '\n') && (p < bufend))
				++p;
			InString = 0;
			InItem = 0;
			break;

		case '\n':
			InItem = 0;
			InString = 0;
			*p++ = '\0';
			break;

		case ' ':
		case '\t':					// Skip whitespace, leave state unchanged
			if (InString) {
				++p;
			} else {
				*p++ = '\0';
				InItem = 0;
			}
			break;

		case '"':					// Begin/End of String
			*p++ = '\0';
			if (!InString) {
				items[item++] = p;
				InItem = ~InItem;
			} else {
				InItem = 0;
			}
			InString = ~InString;	// Toggle
			break;

		default:
			if (!InItem) {
				items[item++] = p;
				InItem = ~InItem;
			}
			++p;
			break;
		}
	}

	return (item - 1);
}


static int parse_name_to_map_index(Mapping * Map, char * s)
{
	int i = 0;

	while (NULL != Map[i].TokenName) {
		if (0 == strcmp(Map[i].TokenName, s))
			return i;
		else
			++i;
	}

	return -1;
}

static void parse_content(Mapping * Map, char * buf, int bufsize)
{
	char		* items[MAX_ITEMS_TO_PARSE] = {NULL};
	int		MapIndex;
	int		item = 0;
	int		IntContent;
	u32		U32Content;
	double	DoubleContent;
	char		msg[256];
	int i;

    FUN_IN("buf[%s]bufsize[%d]vin_map.frame_rate[%d]\n", buf, bufsize, vin_map.frame_rate);
	// Stage one: Generate an argc/argv-type list in items[], without comments and whitespace.
	// This is context insensitive and could be done most easily with lex(1).
	item = parse_items(items, buf, bufsize);

    for (i=0;i<=item;i++)
    {
        PRT_DBG("item[%d]items [%d][%s]\n", item, i, items[i]);
    }

	memset(msg, 0, sizeof(msg));
	// Stage two: interpret the Value, context sensitive.
	for (i = 0; i < item; i += 3) {
		if (0 > (MapIndex = parse_name_to_map_index(Map, items[i]))) {
			PRT_ERR("\n\tParsing error in config file: Parameter Name [%s] not recognized.\n", items[i]);
			continue;
		}
		if (strcmp("=", items[i+1])) {
			PRT_ERR(" Parsing error in config file: '=' expected as the second token in each line.\n");
			exit(0);
		}

		// Now interpret the Value, context sensitive...
		switch (Map[MapIndex].Type) {
		case MAP_TO_U32:						// Numerical, unsigned integer (u32)
			if (1 != sscanf(items[i+2], "%u", &U32Content)) {
				PRT_ERR("Parsing error: Expected numerical value for Parameter [%s], found [%s].\n",
					items[i], items[i+2]);
				exit(0);
			}
            if (* (u32 *) (Map[MapIndex].Address) != U32Content){
			* (u32 *) (Map[MapIndex].Address) = U32Content;
                Map[MapIndex].Update = true;
            }

			sprintf(msg, "%s.", msg);
			break;

		case MAP_TO_U16:						// Numerical, unsigned integer (u16)
			if (1 != sscanf(items[i+2], "%u", &U32Content)) {
				PRT_ERR("Parsing error: Expected numerical value for Parameter [%s], found [%s].\n",
					items[i], items[i+2]);
				exit(0);
			}
            if (* (u16 *) (Map[MapIndex].Address) != U32Content){
			* (u16 *) (Map[MapIndex].Address) = U32Content;
                Map[MapIndex].Update = true;
            }
			sprintf(msg, "%s.", msg);
			break;

		case MAP_TO_U8:							// Numerical, unsigned integer (u8)
			if (1 != sscanf(items[i+2], "%u", &U32Content)) {
				PRT_ERR("Parsing error: Expected numerical value for Parameter [%s], found [%s].\n",
					items[i], items[i+2]);
				exit(0);
			}
			if (* (u8 *) (Map[MapIndex].Address) != U32Content){
			* (u8 *) (Map[MapIndex].Address) = U32Content;
                Map[MapIndex].Update = true;
            }
			sprintf(msg, "%s.", msg);
			break;

		case MAP_TO_S32:						// Numerical, signed integer
			if (1 != sscanf(items[i+2], "%d", &IntContent)) {
				PRT_ERR("Parsing error: Expected numerical value for Parameter [%s], found [%s].\n",
					items[i], items[i+2]);
				exit(0);
			}
			if (* (int *) (Map[MapIndex].Address) != IntContent){
			* (int *) (Map[MapIndex].Address) = IntContent;
                Map[MapIndex].Update = true;
            }
			sprintf(msg, "%s.", msg);
			break;

		case MAP_TO_S16:						// Numerical, signed short
			if (1 != sscanf(items[i+2], "%d", &IntContent)) {
				PRT_ERR("Parsing error: Expected numerical value for Parameter [%s], found [%s].\n",
					items[i], items[i+2]);
				exit(0);
			}
			if (* (s16 *) (Map[MapIndex].Address) != IntContent){
			* (s16 *) (Map[MapIndex].Address) = IntContent;
                Map[MapIndex].Update = true;
            }
			sprintf(msg, "%s.", msg);
			break;

		case MAP_TO_S8:						// Numerical, signed char
			if (1 != sscanf(items[i+2], "%d", &IntContent)) {
				PRT_ERR("Parsing error: Expected numerical value for Parameter [%s], found [%s].\n",
					items[i], items[i+2]);
				exit(0);
			}
			if (* (s8 *) (Map[MapIndex].Address) != IntContent){
			* (s8 *) (Map[MapIndex].Address) = IntContent;
                Map[MapIndex].Update = true;
            }
			sprintf(msg, "%s.", msg);
			break;

		case MAP_TO_DOUBLE:					// Numerical, double
			if (1 != sscanf(items[i+2], "%lf", &DoubleContent)) {
				PRT_ERR("Parsing error: Expected numerical value for Parameter [%s], found [%s].\n",
					items[i], items[i+2]);
				exit(0);
			}
			if (* (double *) (Map[MapIndex].Address) != DoubleContent){
			* (double *) (Map[MapIndex].Address) = DoubleContent;
                Map[MapIndex].Update = true;
            }
			sprintf(msg, "%s.", msg);
			break;

        case MAP_TO_STRING:						// String  attention:no Update
			memset((char *) Map[MapIndex].Address, 0, Map[MapIndex].StringLengthLimit);
			if (NULL != items[i+2]) {
				strncpy((char *) Map[MapIndex].Address, items[i+2], Map[MapIndex].StringLengthLimit - 1);
			} else {
				memset((char *) Map[MapIndex].Address, 0, Map[MapIndex].StringLengthLimit);
			}
			sprintf(msg, "%s.", msg);
			break;

		default:
			PRT_ERR("parse_content Unknown value type in the map definition!\n");
			exit(0);
		}
	}
	PRT_DBG("Parse parameters : msg[%s]\n", msg);
    FUN_OUT("vin_map.frame_rate[%d]\n", vin_map.frame_rate);
}
static int load_default_params(Mapping * Map)
{
	int i = 0;

	while (NULL != Map[i].TokenName) {
		switch (Map[i].Type) {
		case MAP_TO_U32:
			* (u32 *) (Map[i].Address) = (u32) ((int) Map[i].Default);
			break;
		case MAP_TO_U16:
			* (u16 *) (Map[i].Address) = (u16) ((int) Map[i].Default);
			break;
		case MAP_TO_U8:
			* (u8 *) (Map[i].Address) = (u8) ((int) Map[i].Default);
			break;
		case MAP_TO_S32:
			* (int *) (Map[i].Address) = (int) Map[i].Default;
			break;
		case MAP_TO_S16:
			* (s16 *) (Map[i].Address) = (s16) ((int) Map[i].Default);
			break;
		case MAP_TO_S8:
			* (s8 *) (Map[i].Address) = (s8) ((int) Map[i].Default);
			break;
		case MAP_TO_DOUBLE:
			* (double *) (Map[i].Address) = Map[i].Default;
			break;
		case MAP_TO_STRING:
			* (char *) (Map[i].Address) = '\0';
			break;
		default:
			PRT_ERR("load_default_params Unknown value type in the map definition!\n");
			exit(0);
		}
		++i;
	}

	return 0;
}

static int input_params(Mapping *Map, const char * content)
{
    FUN_IN();
	if (NULL == content) {
		load_default_params(Map);
	} else {
		char *pContent = NULL;
		u32 contentSize = 0;
		contentSize = strlen(content);
		pContent = (char *) malloc(contentSize + 1);
		strncpy(pContent, content, contentSize);
		pContent[contentSize] = 0;
		parse_content(Map, pContent, contentSize);
		update_params(Map);
		free(pContent);
	}
    FUN_OUT();
	return 0;
}

int output_params(Mapping * Map, char ** pContentOut)
{
	char * content = *pContentOut;
	int	i = 0;
	u32	unsigned_value;
	int	signed_value;

	content[0] = '\0';
	while (NULL != Map[i].TokenName) {
		sprintf(content, "%s%s", content, Map[i].TokenName);
		switch (Map[i].Type) {
		case MAP_TO_U32:
			unsigned_value = * (u32 *) (Map[i].Address);
			sprintf(content, "%s = %u\n", content, unsigned_value);
			break;
		case MAP_TO_U16:
			unsigned_value = * (u16 *) (Map[i].Address);
			sprintf(content, "%s = %u\n", content, unsigned_value);
			break;
		case MAP_TO_U8:
			unsigned_value = * (u8 *) (Map[i].Address);
			sprintf(content, "%s = %u\n", content, unsigned_value);
			break;
		case MAP_TO_S32:
			signed_value = * (int *) (Map[i].Address);
			sprintf(content, "%s = %d\n", content, signed_value);
			break;
		case MAP_TO_S16:
			signed_value = * (s16 *) (Map[i].Address);
			sprintf(content, "%s = %d\n", content, signed_value);
			break;
		case MAP_TO_S8:
			signed_value = * (s8 *) (Map[i].Address);
			sprintf(content, "%s = %d\n", content, signed_value);
			break;
		case MAP_TO_DOUBLE:
			sprintf(content, "%s = %.2lf\n", content, * (double *) (Map[i].Address));
			break;
		case MAP_TO_STRING:
			sprintf(content, "%s = \"%s\"\n", content, (char *) (Map[i].Address));
			break;
		default:
			sprintf(content, "%s : [%d] unknown value type.\n", content, Map[i].Type);
			PRT_ERR("Output Parameters Unknown value type in the map definition !!!\n");
			break;
		}
		++i;
	}
	return 0;
}

int send_text(u8 *pBuffer, u32 size)
{
    if (size == 8)
    {
        FUN_IN("ACK.result[%d]ACK.info[%d]\n", ((Ack *)pBuffer)->result, ((Ack *)pBuffer)->info);
    }
    else
    {
        FUN_IN("not ACK content[%s]size[%d]\n", pBuffer, size);
    }
	int retv = send(sockfd2, pBuffer, size, MSG_NOSIGNAL);
	if ((u32)retv != size) {
		PRT_ERR("send() returns %d.\n", retv);
		return -1;
	}
    FUN_OUT();
	return 0;
}

int receive_text(u8 *pBuffer, u32 size)
{
    FUN_IN();
	int retv = recv(sockfd2, pBuffer, size, MSG_WAITALL);
	if (retv <= 0) {
		if (retv == 0) {
			PRT_DBG("Port [%d] closed\n\n", ENCODE_SERVER_PORT);
			return -2;
		}
		PRT_ERR("recv() returns %d.", retv);
		return -1;
	}

    if (size == 8)
    {
        FUN_OUT("ACK.result[%d]ACK.info[%d]\n", ((Ack *)pBuffer)->result, ((Ack *)pBuffer)->info);
    }
    else
    {
        FUN_OUT("not ACK content[%s]size[%d]\n", pBuffer, size);
    }
	return retv;
}
static int get_encode_param(char * section_name, u32 info)
{
	int streamID = info, retv = 0;

    PRT_DBG("Section [%s] setting:streamID[%d]\n", section_name, streamID);
    stream_map[streamID].dptz                       = 0;
    stream_map[streamID].streamFormat.flip_rotate   = 0;
    
    stream_map[streamID].streamFormat.encode_type   = 
    g_stEncodeInfo[streamID].encode_type;

    stream_map[streamID].streamFormat.encode_fps    = 
    512000000/g_stEncodeInfo[streamID].framerate;

    stream_map[streamID].streamFormat.encode_width  = 
    g_stEncodeInfo[streamID].encode_width;

    stream_map[streamID].streamFormat.encode_height = 
    g_stEncodeInfo[streamID].encode_height;
#if 0
//h264
    stream_map[streamID].h264Conf.gopM              = 
    g_stEncodeInfo[streamID].h264.gopM;
    stream_map[streamID].h264Conf.gopN              = 
    g_stEncodeInfo[streamID].h264.gopN;
    stream_map[streamID].h264Conf.idrInterval       = 
    g_stEncodeInfo[streamID].h264.idrInterval;
//mpeg 
    stream_map[streamID].mjpegConf.quality          = 
    g_stEncodeInfo[streamID].mpeg.quality;
    FUN_OUT("\
            dptz             [%d]\n\
            flipRotate       [%d]\n\
            fps              [%d]\n\
            resolution     [%d X %d]\n\
            h264 M           [%d]\n\
            h264 N           [%d]\n\
            h264 IDR         [%d]\n\
            mjpeg qa         [%d]\n\
            ", 
            stream_map[streamID].dptz, stream_map[streamID].streamFormat.flip_rotate,   
            stream_map[streamID].streamFormat.encode_fps,
            stream_map[streamID].streamFormat.encode_width,
            stream_map[streamID].streamFormat.encode_height,
            stream_map[streamID].h264Conf.gopM,
            stream_map[streamID].h264Conf.gopN,
            stream_map[streamID].h264Conf.idrInterval,
            stream_map[streamID].mjpegConf.quality
    );
#endif       
    return retv;
}
static int get_vinvout_param(char * section_name, u32 info)
{
	int streamID = info, retv = 0;
    FUN_IN();
	PRT_DBG("Section [%s] setting:\n", section_name);

    /*-----------------------------------------------------------------------------
     *  remenber to convert frame_rate  [] -> 1-60
     *-----------------------------------------------------------------------------*/
    vin_map.frame_rate = 512000000/g_stEncodeInfo[streamID].framerate;
    vin_map.mode       = 0;//g_stEncodeInfo[streamID].framerate;
    vout_map.type = 0;//g_stEncodeInfo[streamID].framerate;
    vout_map.mode = 0;//g_stEncodeInfo[streamID].framerate;
    FUN_OUT("\
            vin_frame_rate [%d]\n\
            vin_mode       [%d]\n\
            vout_type      [%d]\n\
            vout_mode      [%d]\n\
            ", vin_map.frame_rate, vin_map.mode, vout_map.type, vout_map.mode);
	return retv;
}

static int show_date(int stream, int areaId, int enable)
{
	GADI_VENC_StreamFormatT formatPar;
    static GADI_OSD_TextParamsT textPar =
    {
    	.planeId   = 0,
    	.areaId    = 0,
    	.enable    = 1,
    	.type	   = 1,
    	.size	   = 32,
    	.color	   = 0,//black
    	.outline   = 1,
    	.bold	   = 0,
    	.italic    = 0,
    	.startX    = 0,
    	.startY    = 0,
    	.boxWidth  = 50,
    	.boxHeight = 50,
    };
	int retVal = 0;
    
    textPar.planeId = stream;
    textPar.areaId = areaId;
    textPar.enable = enable;
    
    formatPar.streamId = textPar.planeId;

//	retVal =  video_get_stream_format(&formatPar);
//	if(retVal != GADI_OK)
//	{
//		GADI_ERROR("get stream format error\n");
//		return retVal;
//	}

    gadi_sys_get_date(textPar.textStr);
	textPar.textLen = strlen(textPar.textStr);

    if (!osdHandle)
    {   
        retVal = gadi_osd_init();
	    osdHandle = gadi_osd_open(&retVal);
    }
	if(retVal != GADI_OK)
	{
		PRT_ERR("gadi_osd_open error\n");
        return (-1);
	}

	retVal = gadi_osd_show_date(osdHandle, &textPar, 1920, 1080);

	return retVal;

}
static int osd_set_text(GADI_OSD_TextParamsT* textParam)
{
	int retVal = 0;
#if 0
	GADI_VENC_StreamFormatT formatPar;

	formatPar.streamId = textParam->planeId;

	retVal =  video_get_stream_format(&formatPar);
	if(retVal != GADI_OK)
	{
		PRT_ERR("get stream format error\n");
		return retVal;
	}
#endif

	//strcpy(textParam.textStr, string);
	textParam->textLen = strlen(textParam->textStr);
    PRT_DBG("\n"
            "======text param======\n"
            "planeId    :       %d\n"
            "areaId     :       %d\n"
            "enable     :       %d\n"
            "textStr    :       %s\n"
            "textLen    :       %d\n"
            "type       :       %d\n"
            "size       :       %d\n"
            "color      :       %d\n"
            "outline    :       %d\n"
            "bold       :       %d\n"
            "italic     :       %d\n"
            "startX     :       %d\n"
            "startY     :       %d\n"
            "boxWidth   :       %d\n"
            "boxHeight  :       %d\n",
            textParam->planeId, textParam->areaId,
            textParam->enable, textParam->textStr,
            textParam->textLen, textParam->type,
            textParam->size, textParam->color,
            textParam->outline, textParam->bold,
            textParam->italic, textParam->startX,
            textParam->startY, textParam->boxWidth,
            textParam->boxHeight
            );

	retVal = gadi_osd_show_text(osdHandle, textParam, 1920, 1080);

	return retVal;

}
static int set_pm_param(char * section_name)
{
	FUN_IN("Section [%s] setting:\n", section_name);
	privacy_mask_t pm;
	pm.unit = 0;
	pm.width = pm_map.width;
	pm.height = pm_map.height;
	pm.x = pm_map.x;
	pm.y = pm_map.y;
	pm.action = pm_map.action;
	pm.color = pm_map.color;
#if 0
	if (mw_add_privacy_mask(&pm) < 0) {
		APP_ERROR("mw_add_privacy_mask");
		return -1;
	} else {
		if (pm_map.action == MW_PM_REMOVE_ALL) {
			if (pm_remove_all_mask() < 0){
				APP_ERROR("pm_remove_all_mask");
				return -1;
			}
			if (mw_enable_privacy_mask(0) < 0) {
				APP_ERROR("Failed to clear privacy mask!\n");
				return -1;
			}
		} else {
			if (pm_add_one_mask(&pm) < 0) {
				APP_ERROR("pm_add_one_mask");
				return -1;
			}
			if (mw_enable_privacy_mask(1) < 0) {
				APP_ERROR("Failed to draw privacy mask!\n");
				return -1;
			}
		}
	}
	g_mw_pm_enable = (pm_map.action != MW_PM_REMOVE_ALL);
#endif
	return 0;
}
static int set_osd_param(char * section_name)
{
	PRT_DBG("Section osd[%s] setting:\n", section_name);
	int i;
    GADI_OSD_TextParamsT textParam;

 
    GADI_ERR retVal = GADI_OK;


    if (!osdHandle)
    {   
        retVal = gadi_osd_init();
	    osdHandle = gadi_osd_open(&retVal);
    }
	if(retVal != GADI_OK)
	{
		PRT_ERR("gadi_osd_open error\n");
        return (-1);
	}

	for (i = 0; i < 1; ++i) {

        /*-----------------------------------------------------------------------------
         *  bmp
         *-----------------------------------------------------------------------------*/
#if 0
		if (osd_map[i].bmp_enable != g_mw_osd[i].bmp_enable) {
			PRT_DBG("Enable bmp osd, add Ambarella log file.\n");
			bmp.stream = i;
			bmp.area = 0;
			bmp.enable = osd_map[i].bmp_enable;
			bmp.offset_x = 100;
			bmp.offset_y = 100;
			bmp.bmp_filename = "/usr/local/bin/Ambarella-256x128-8bit.bmp";
			if (mw_osd_set_bmp(&bmp) < 0) {
				PRT_DBG("Add osd bmp failed !\n");
			} else {
				g_mw_osd[i].bmp_enable = osd_map[i].bmp_enable;
			}
		}
#endif
        /*-----------------------------------------------------------------------------
         *  time
         *-----------------------------------------------------------------------------*/
		if (osd_map[i].time_enable != g_mw_osd[i].time_enable) {
			PRT_DBG("Enable time string osd, add black\n");
			if (show_date(i, 1, osd_map[i].time_enable) < 0) {
				PRT_ERR("Add osd current time failed !\n");
				return -1;
			} else {
				g_mw_osd[i].time_enable = osd_map[i].time_enable;
			}
		}

        /*-----------------------------------------------------------------------------
         *  text
         *-----------------------------------------------------------------------------*/
		if (1) {
			osd_map[i].text.length = strlen(osd_map[i].text.str);
			osd_map[i].text.stream = i;
			PRT_DBG("Enable osd text string [%s], length [%d].\n",
				osd_map[i].text.str, osd_map[i].text.length);

            textParam.planeId   = osd_map[i].text.stream;
            textParam.areaId    = 2;//0:bmp 1:time 2:show text
            textParam.enable    = 1;//osd_map[i].text_enable;
            textParam.textLen   = osd_map[i].text.length;
            strncpy(textParam.textStr, osd_map[i].text.str, textParam.textLen+1);
            textParam.type      = osd_map[i].text.type;
            textParam.size      = osd_map[i].text.size;
            textParam.color     = osd_map[i].text.color;
            textParam.outline   = osd_map[i].text.outline;
            textParam.bold      = osd_map[i].text.bold;
            textParam.italic    = osd_map[i].text.italic;
            textParam.startX    = osd_map[i].text.start_x;
            textParam.startY    = osd_map[i].text.start_y;
            textParam.boxWidth  = osd_map[i].text.box_w;
            textParam.boxHeight = osd_map[i].text.box_h;

			if (osd_set_text(&textParam) < 0) {
				PRT_ERR("Add osd text string [%s] failed !\n", osd_map[i].text.str);
                return (-1);
			} else {
                PRT_DBG("Add osd text string [%s] successed!\n", osd_map[i].text.str);
			}
		}
	}

 //   retVal = gadi_osd_close(osdHandle);
 //   osdHandle = NULL;
 //   if(retVal != GADI_OK)
//	{
//		PRT_ERR("gadi_osd_close error\n");
//        return (-1);
//	}
	return 0;
}
static int set_encode_param(char * section_name)
{
	int streamID = 0;
    u16 width=0,height=0;
    int retVal=0;
    int i=0;

	PRT_DBG("Section [%s] setting:\n", section_name);
    PRT_DBG("\n"
            "s0_dptz             [%d]\n"
            "s0_type             [%d]\n"
            "s0_flip_rotate      [%d]\n"
            "s0_enc_fps          [%d]\n"
            "s0_resolution    [%d X %d]\n"
            ,  stream_map[streamID].dptz,  stream_map[streamID].streamFormat.encode_type,
               stream_map[streamID].streamFormat.flip_rotate,  stream_map[streamID].streamFormat.encode_fps,
               stream_map[streamID].streamFormat.encode_width, stream_map[streamID].streamFormat.encode_height);


    /*-----------------------------------------------------------------------------
     *  set enc_fps
     *-----------------------------------------------------------------------------*/
    for (i=5; i<sizeof(EncodeMap)/sizeof(Mapping); i+=6)
    {
       streamID = (i+6)/6 - 1;
        if (EncodeMap[i].Update)
        {
            retVal = gapp_video_set_framerate(streamID, 512000000/stream_map[streamID].streamFormat.encode_fps);
            if(retVal != GADI_OK){
                break;
            }
            g_stEncodeInfo[streamID].framerate = 512000000/stream_map[streamID].streamFormat.encode_fps;
            g_emIpcXmlType = IPC_XML_ENCODE;
            EncodeMap[i].Update = false;
        }
    }

    /*-----------------------------------------------------------------------------
     *  set resolution
     *-----------------------------------------------------------------------------*/
    for (i=3; i<sizeof(EncodeMap)/sizeof(Mapping); i+=6)
    {
        streamID = (i+6)/6 - 1;
        if (EncodeMap[i].Update)
        {
            retVal = gapp_video_set_resolution(streamID, stream_map[streamID].streamFormat.encode_width, stream_map[streamID].streamFormat.encode_height);
            if(retVal != GADI_OK){
                break;
            }
            g_stEncodeInfo[streamID].encode_width = stream_map[streamID].streamFormat.encode_width;
            g_stEncodeInfo[streamID].encode_height= stream_map[streamID].streamFormat.encode_height;
            g_emIpcXmlType = IPC_XML_ENCODE;
            EncodeMap[i].Update = false;
        }
    }

     /*-----------------------------------------------------------------------------
      *  set flip_rotate : adi not implementationed
      *-----------------------------------------------------------------------------*/
    for (i=2; i<sizeof(EncodeMap)/sizeof(Mapping); i+=6)
    {
        streamID = (i+6)/6 - 1;
        if (EncodeMap[i].Update)
        {
            PRT_ERR("adi not implementationed\n");
#if 0
            retVal = gapp_video_set_framerate(streamID, 512000000/stream_map[streamID].streamFormat.encode_fps);
            if(retVal != GADI_OK){
                break;
            }
            g_stEncodeInfo[streamID].framerate = pstEncCmd->framerate;
            g_emIpcXmlType = IPC_XML_ENCODE;
#endif
            EncodeMap[i].Update = false;
        }
    }
    /* save xml single */
    if(retVal == GADI_OK)
    {
        raise(XMLSAVE);
    }   
#if 0
    ......
#endif
        return 0;
}

static int set_vinvout_param(char * section_name)
{
    int streamId = 0, retv = 0;
    GONVIF_Encode_Cmd stEncCmd;

    FUN_IN("Section [%s] setting:\n", section_name);
    /*-----------------------------------------------------------------------------
     *  remenber to convert frame_rate  [] -> 1-60
     *-----------------------------------------------------------------------------*/
    //    g_stEncodeInfo[streamId].framerate = vin_map.frame_rate;
    PRT_DBG("\n\
            vin_frame_rate [%d]\n\
            vin_mode       [%d]\n\
            vout_type      [%d]\n\
            vout_mode      [%d]\n\
            ", vin_map.frame_rate, vin_map.mode, vout_map.type, vout_map.mode);

    memset(&stEncCmd, 0, sizeof(stEncCmd));
    stEncCmd.streamid = streamId;
    stEncCmd.framerate = 512000000/vin_map.frame_rate;
    //set FRAMERATE
    if (0 != send_fly_request(MEDIA_SET_FRAMERATE, sizeof(GONVIF_Encode_Cmd), (unsigned char *)&stEncCmd, 0, NULL))
    {
        PRT_ERR("set FRAMERATE error!\n");
    }
#if 0 //need to replace
    if (mw_disable_stream(STREAMS_MASK) < 0) {
        PRT_ERR("Cannot stop encoding streams!");
        return -1;
    }......

    update_encode_info();
#endif

    return retv;
}

static int get_stream_param(char *section_name, u32 info)
{
    int streamId = 0, retv = 0;
    FUN_IN();
    PRT_DBG("Section [%s] setting:\n", section_name);

     streamId = atoi(&section_name[6]);
    stream_map[streamId].h264Conf.cbrAvgBps =
        g_stEncodeInfo[streamId].bitrate*1000;
#if 0
    //h264
    stream_map[streamID].h264Conf.gopM              = 
        g_stEncodeInfo[streamID].h264.gopM;
    stream_map[streamID].h264Conf.gopN              = 
        g_stEncodeInfo[streamID].h264.gopN;
    stream_map[streamID].h264Conf.idrInterval       = 
        g_stEncodeInfo[streamID].h264.idrInterval;
    //mpeg 
    stream_map[streamID].mjpegConf.quality          = 
        g_stEncodeInfo[streamID].mpeg.quality;
    FUN_OUT("\
            dptz             [%d]\n\
            flipRotate       [%d]\n\
            fps              [%d]\n\
            resolution     [%d X %d]\n\
            h264 M           [%d]\n\
            h264 N           [%d]\n\
            h264 IDR         [%d]\n\
            mjpeg qa         [%d]\n\
            ", 
            stream_map[streamID].dptz, stream_map[streamID].streamFormat.flip_rotate,   
            stream_map[streamID].streamFormat.encode_fps,
            stream_map[streamID].streamFormat.encode_width,
            stream_map[streamID].streamFormat.encode_height,
            stream_map[streamID].h264Conf.gopM,
            stream_map[streamID].h264Conf.gopN,
            stream_map[streamID].h264Conf.idrInterval,
            stream_map[streamID].mjpegConf.quality
    );
 #endif
    //stream_map[streamId].h264Conf.gopM = 
    //stream_map[streamId].h264Conf.gopN = 
    //stream_map[streamId].h264Conf.idrInterval = 
    //stream_map[streamId].mjpegConf.quality = 
    FUN_OUT("cbrAvgBps [%d]\n", stream_map[streamId].h264Conf.cbrAvgBps);

	return retv;
}

static int set_stream_param(char *section_name)
{
    int streamId = 0; 
    int retVal = -1;
    int h264_update = false;
    int i=0;

	FUN_IN("Section [%s] setting:\n", section_name);

    streamId = atoi(&section_name[6]);
    FUN_OUT("cbrAvgBps [%d]\n", stream_map[streamId].h264Conf.cbrAvgBps);

    /*-----------------------------------------------------------------------------
     *  set bitrate
     *-----------------------------------------------------------------------------*/
    retVal = gapp_video_set_bitrate(streamId, stream_map[streamId].h264Conf.cbrAvgBps);
    if(retVal != GADI_OK){
        PRT_ERR("send_bitrate: error!\n");
    }
    g_stEncodeInfo[streamId].bitrate = stream_map[streamId].h264Conf.cbrAvgBps;

#if 1
    /*-----------------------------------------------------------------------------
     *  H264 set M N idrInterval 
     *-----------------------------------------------------------------------------*/
    for (i=1; i<=5 ;i++)
    {
        if (Params[streamId + 2].map[i].Update)
        {
            h264_update = true;
        }
    }
    if (true == h264_update)
    {
        if (!vencHandle)
        {
            PRT_ERR("vencHandle NULL\n");
            return (-1);
        }
        if(gadi_venc_set_h264_config(vencHandle, &stream_map[streamId].h264Conf) < 0)
        {
            PRT_ERR("gadi_venc_set_h264_config error, steamId=%d\n", streamId);
            return (-1);
        }
    }
#endif
    /* save xml single */
    if(retVal == GADI_OK)
    {
        raise(XMLSAVE);
    }
	return retVal;
}



static Section * find_section(char * name)
{
	int i = 0;
	while (NULL != Params[i].name) {
		if (strcmp(name, Params[i].name) == 0)
			break;
		++i;
	}
	if (NULL == Params[i].name)
		return NULL;
	else
		return &Params[i];
}

static int do_get_param(Request *req)
{
	u8 *name = g_buffer;
	char *content = (char *)g_buffer;
	int retv = 0;
	Section * section;
	Ack ack;

    memset(g_buffer, 0, BUFFER_SIZE);
	// Get section name
	retv = receive_text(name, req->dataSize);
	name[req->dataSize] = 0;
	PRT_DBG("Section Name is : %s\n", (char *)name);

	section = find_section((char *)name);
	if (NULL == section) {
		ack.result = -1;
		ack.info = -1;
		PRT_DBG("Section [%s] is not found!\n", (char *)name);
		return send_text((u8 *)&ack, sizeof(ack));
	} else {
        /*-----------------------------------------------------------------------------
         *  1.from board to map
         *  2.from map to cgi
         *-----------------------------------------------------------------------------*/
		retv = (*section->get)(section->name, req->info);
		retv = output_params(section->map, &content);
		PRT_DBG("content[%s]\n", content);
		ack.result = retv;
		ack.info = strlen(content);
		send_text((u8 *)&ack, sizeof(ack));
		return send_text((u8 *)content, ack.info);
	}
}

static int do_set_param(Request *req)
{
	u8 *name = g_buffer;
	char *content = (char *)g_buffer;
	int retv = 0;
	Section * section;
	Ack ack;

    memset(g_buffer, 0, BUFFER_SIZE);
	retv = receive_text(name, req->dataSize);
	name[req->dataSize] = 0;
	PRT_DBG("Section Name is : %s\n", (char *)name);
	section = find_section((char *)name);
	ack.info = ack.result = (NULL == section) ? -1 : 0;
	retv = send_text((u8 *)&ack, sizeof(ack));
	if (ack.result == -1) {
		PRT_DBG("Section [%s] is not found!\n", (char *)name);
		return -1;
	}

	// Receive parameter settings
	retv = receive_text((u8 *)content, req->info);
	content[req->info] = 0;
	if (0) {
		PRT_DBG("CGI Setting is : content[%s]\n", content);
	} else {
		PRT_DBG("CGI Parameter Setting is : content[%s]\n", content);

        /*-----------------------------------------------------------------------------
         *  1.from cgi to map
         *  2.from map to board
         *-----------------------------------------------------------------------------*/
		retv = input_params(section->map, content);
		retv = (*section->set)(section->name);
        PRT_DBG("ACK.result[%d] ack.info[%d]\n", ack.result, ack.info);
		ack.result = ack.info = retv;
	}
	retv = send_text((u8 *)&ack, sizeof(ack));
	return retv;
}
static int do_change_br(Request *req)
{
	int retv = 0, stream_id = 0;
	Ack ack;
    char buf[BUFFER_SIZE]="\0";

    FUN_IN("req.id[%d]req.info[%d]req.dataSize[%d]\n", req->id, req->info, req->dataSize);

    stream_id = (req->info >> STREAM_ID_OFFSET);
    stream_map[stream_id].h264Conf.cbrAvgBps = (req->info & ~(0xf <<
    STREAM_ID_OFFSET))/1000;

    sprintf(buf, "STREAM%d", stream_id);
    PRT_DBG("before set_stream_param\n");
    set_stream_param(buf);
    PRT_DBG("after set_stream_param\n");

#if 0
	mw_bitrate_range br;
	stream = (req->info >> STREAM_ID_OFFSET);
	br.id = (1 << stream);
	br.min_bps = req->info & ~(0x3 << STREAM_ID_OFFSET);
	br.max_bps = req->dataSize;
	retv = mw_change_br(&br);
	APP_INFO("[do_change_br] stream %d, bitrate range [%d, %d].\n",
		stream, br.min_bps, br.max_bps);
	if (retv == 0) {
		if (g_mw_stream[stream].h264.brc_mode & 0x1) {
			g_mw_stream[stream].h264.vbr_min_bps = br.min_bps;
			g_mw_stream[stream].h264.vbr_max_bps = br.max_bps;
			stream_map[stream].h264.vbr_min_bps = br.min_bps;
			stream_map[stream].h264.vbr_max_bps = br.max_bps;
		} else {
			g_mw_stream[stream].h264.cbr_avg_bps = br.min_bps;
			stream_map[stream].h264.cbr_avg_bps = br.min_bps;
		}
	}
#endif
	ack.result = retv;
	ack.info = 0;
	return send_text((u8 *)&ack, sizeof(ack));
}

static int process_request(Request *req)
{
	PRT_DBG("Process Request ID [%d]\n", (int)req->id);
	switch (req->id) {
	case REQ_SET_FORCEIDR:
//		do_force_idr(req);
		break;

	case REQ_STREAM_START:
//		do_stream_start(req);
		break;

	case REQ_STREAM_STOP:
//		do_stream_stop(req);
		break;

	case REQ_CHANGE_BR:
		do_change_br(req);
		break;

	case REQ_CHANGE_FR:
//		do_change_fr(req);
		break;

	case REQ_GET_PARAM:
		do_get_param(req);
		break;

	case REQ_SET_PARAM:
		do_set_param(req);
		break;

	default:
		PRT_ERR("Unknown request id [%d]\n", (int)req->id);
		break;
	}
	return 0;
}
static int connect_server(void)
{
	struct sockaddr_in client_addr;

	assert(sockfd2 <= 0);
	socklen_t length = sizeof(client_addr);

	PRT_DBG("Listen to %d\n", ENCODE_SERVER_PORT);
	if ((sockfd2 = accept(sockfd, (struct sockaddr *)&client_addr, &length)) < 0) {
		PRT_ERR("accept failed %d\n", sockfd2);
		return -1;
	}
	PRT_DBG("Port [%d] connected!\n", ENCODE_SERVER_PORT);

	return 0;
}

static int disconnect_server(void)
{
	assert(sockfd2 > 0);
	close(sockfd2);
	sockfd2 = -1;
	return 0;
}



static void main_loop(void)
{
    FUN_IN();
	while (1) {
		if (connect_server() < 0)
			break;

		while (1) {
			Request req;
			if (receive_text((u8 *)&req, sizeof(req)) < 0)
				break;

			process_request(&req);
		}

		disconnect_server();
	}
    FUN_OUT();
}

static int start_server(void)
{
	int flag = 1;
	struct sockaddr_in server_addr;

	assert(sockfd <= 0);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		PRT_ERR("socket failed %d !", sockfd);
		return -1;
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0) {
		PRT_ERR("setsockopt");
		return -1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(ENCODE_SERVER_PORT);
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		PRT_ERR("bind");
		return -1;
	}
	if (listen(sockfd, 10) < 0) {
		PRT_ERR("listen");
		return -1;
	}
	return 0;
}

static int create_server(void)
{

	if (start_server() < 0) {
		PRT_ERR("start_server");
		return -1;
	}
	PRT_DBG(" [Done] Create web_service\n");
	return 0;
}
static int load_default_configs()
{
	char string[10 * 1024], *content;

	content = string;
	input_params(VinVoutMap, NULL);
	output_params(VinVoutMap, &content);

	input_params(EncodeMap, NULL);
	output_params(EncodeMap, &content);
#if 0
	for (i = 0; i < 4; ++i) {
		input_params(StreamMap[i], NULL);
		output_params(StreamMap[i], &content);
		memcpy(&g_mw_stream[i].format, &stream_map[i].format,
			sizeof(mw_encode_format));
		memcpy(&g_mw_stream[i].h264, &stream_map[i].h264,
			sizeof(mw_h264_config));
		memcpy(&g_mw_stream[i].mjpeg, &stream_map[i].mjpeg,
			sizeof(mw_jpeg_config));
	}
#endif
	input_params(OSDMap, NULL);
	output_params(OSDMap, &content);

//	input_params(ACTIONMap, NULL);
//	output_params(ACTIONMap, &content);
    
    return 0;
}
void * webserver_process(void * argv)
{
    load_default_configs();
	if (create_server() < 0) {
		PRT_ERR("create_server");
		return (NULL);

	}
    while (1) {
        main_loop();
    }

    return (NULL);
}
