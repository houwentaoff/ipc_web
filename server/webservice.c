/*
****************************************************************************
*
** \file      webservice.c
**
** \version   $Id: webservice.c 2593 2015-01-16 08:57:28Z houwentao $
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
#include "adi_sys.h"
#include "adi_pm.h"
#include "adi_isp.h"
//#include "adi_venc.h"


//*****************************************************************************
//*****************************************************************************
//** Local Defines
//*****************************************************************************
//*****************************************************************************
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

#define CUR_STREAM_NUM          (2)

//*****************************************************************************
//*****************************************************************************
//** Local Data
//*****************************************************************************
//*****************************************************************************
static u8						g_buffer[BUFFER_SIZE];
static gk_vin_mode			vin_map;
static gk_vout_mode			vout_map;
static gk_encode_stream     stream_map[ENCODE_STREAM_NUM];
static osd_param_t				osd_map[ENCODE_STREAM_NUM];
static live_view_param_t		live_map;
static privacy_mask_t		pm_map;
//static u8						g_mw_pm_enable = 0;
static osd_param_t				g_mw_osd[ENCODE_STREAM_NUM];
/*image _3a page*/
static basic_iq_params			g_mw_basic_iq;
static mw_image_param		g_mw_image;
static mw_awb_param			g_mw_awb;
static mw_af_param			g_mw_af;
static u32						g_mw_encode_mode = HIGH_FRAMERATE;

static basic_iq_params		iq_map;
static mw_image_param	image_map;
static mw_awb_param		awb_map;
static mw_af_param		af_map;

static int sockfd = -1;
static int sockfd2 = -1;

extern GONVIF_Encode_Cmd g_stEncodeInfo[4];
extern int g_slIPCMsgID;
extern IPC_XML_TYPE g_emIpcXmlType;
extern GADI_SYS_HandleT vinHandle;
extern GADI_SYS_HandleT voutHandle;
extern GADI_SYS_HandleT vencHandle;

extern GADI_SYS_HandleT ispHandle;

extern GADI_ISP_OpenParamsT openParams;
extern GADI_ISP_ConfigureParamsT configureParams;

extern GADI_ISP_ImageAttrT imageAttr;
//imageAttr: sharpness, saturation, brightness, hue, contrast, mctfStrength, localExpMode, localExpCurveTable, backLightCompEnable, dayNightEnable,extraBlc
extern GADI_ISP_AE_AttrT aeAttr;
extern GADI_ISP_ME_AttrT meAttr;

extern GADI_ISP_AwbAttrT awbAttr;
extern GADI_ISP_MwbAttrT mwbAttr;

extern GADI_ERR gadi_venc_get_h264_config(GADI_SYS_HandleT handle,
                                               void* configPar);
extern GADI_ERR gadi_venc_set_bitrate(GADI_SYS_HandleT handle, void* brPar);
extern GADI_ERR gadi_venc_set_h264_config(GADI_SYS_HandleT handle,
                                               void* configPar);
static   GADI_SYS_HandleT osdHandle;
static GADI_SYS_HandleT pmHandle;
//*****************************************************************************
//*****************************************************************************
//** Local Functions Declaration
//*****************************************************************************
//*****************************************************************************
static int set_vinvout_param(char * section_name);
static int get_vinvout_param(char * section_name, u32 info);
static int get_stream_param(char * section_name, u32 info);
static int set_stream_param(char * section_name);
static int get_encode_param(char * section_name, u32 info);
static int set_encode_param(char * section_name);
static int set_osd_param(char * section_name);
static int set_pm_param(char * section_name);
static int get_live_param(char * section_name, u32 info);
static int set_image_param(char * section_name);
static int get_image_param(char * section_name, u32 info);

static mapping_t VinVoutMap[] = {
	{"vin_enable",			&vin_map.enable,				MAP_TO_U32,	0.0,		0,		0.0,		0.0,	false},
	{"vin_mode",			&vin_map.mode,				MAP_TO_U32,	VIDEO_MODE_AUTO,		0,		0.0,		0.0,	false},
	{"vin_framerate",		&vin_map.frame_rate,			MAP_TO_U32,	VIDEO_FPS_29_97,		0,		0.0,		0.0,	false},
//	{"vin_mirror",			&vin_map.mirror_mode.mirror_pattern,	MAP_TO_U32,	0.0,		0,		0.0,		0.0,	},
//	{"vin_bayer",			&vin_map.mirror_mode.bayer_pattern,	MAP_TO_U32,	0.0,		0,		0.0,		0.0,	},

	{"vout_type",			&vout_map.type,			MAP_TO_U32,	VOUT_SINK_TYPE_CVBS,		0,		0.0,		0.0,	false},
	{"vout_mode",			&vout_map.mode,			MAP_TO_U32,	VIDEO_MODE_480I,		0,		0.0,		0.0,	false},

	{NULL,			NULL,						-1,	0.0,					0,	0.0,	0.0,		0},
};


static mapping_t Stream0[] = {
	{"s0_h264_id",			&stream_map[0].h264Conf.streamId,				MAP_TO_U32,     0.0,		0,		0.0,		0.0,	false},
	{"s0_M",				&stream_map[0].h264Conf.gopM,					MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
	{"s0_N",				&stream_map[0].h264Conf.gopN,					MAP_TO_U8,	    30.0,		0,		0.0,		0.0,	false},
	{"s0_idr_interval",		&stream_map[0].h264Conf.idrInterval,		MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
//	{"s0_gop_model",		&stream_map[0].h264.gop_model,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s0_profile",			&stream_map[0].h264.profile,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s0_brc",				&stream_map[0].h264.brc_mode,	        MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
	{"s0_cbr_avg_bps",		&stream_map[0].h264Conf.cbrAvgBps,		MAP_TO_U32,	    4000000.0,		0,		0.0,		0.0,	false},
//	{"s0_vbr_min_bps",		&stream_map[0].h264.vbr_min_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
//	{"s0_vbr_max_bps",		&stream_map[0].h264.vbr_max_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
	{"s0_quality",			&stream_map[0].mjpegConf.quality,			MAP_TO_U8,	    88.0,	    0,		0.0,		0.0,	false},
	{NULL,			        NULL,						            -1,	            0.0,		0,	    0.0,	    0.0,	false},
};

static mapping_t Stream1[] = {
	{"s1_h264_id",			&stream_map[1].h264Conf.streamId,				MAP_TO_U32,	    1.0,		0,		0.0,		0.0,	false},
	{"s1_M",				&stream_map[1].h264Conf.gopM,					MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
	{"s1_N",				&stream_map[1].h264Conf.gopN,					MAP_TO_U8,	    30.0,		0,		0.0,		0.0,	false},
	{"s1_idr_interval",		&stream_map[1].h264Conf.idrInterval,		MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
//	{"s1_gop_model",		&stream_map[1].h264.gop_model,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s1_profile",			&stream_map[1].h264.profile,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s1_brc",				&stream_map[1].h264.brc_mode,	        MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
	{"s1_cbr_avg_bps",		&stream_map[1].h264Conf.cbrAvgBps,		MAP_TO_U32,	    4000000.0,		0,		0.0,		0.0,	false},
//	{"s1_vbr_min_bps",		&stream_map[1].h264.vbr_min_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
//	{"s1_vbr_max_bps",		&stream_map[1].h264.vbr_max_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
	{"s1_quality",			&stream_map[1].mjpegConf.quality,			MAP_TO_U8,	    88.0,	    0,		0.0,		0.0,	},
	{NULL,			        NULL,						            -1,	            0.0,		0,	    0.0,	    0.0,	false},
};

static mapping_t Stream2[] = {
	{"s2_h264_id",			&stream_map[2].h264Conf.streamId,				MAP_TO_U32,	    2.0,		0,		0.0,		0.0,	false},
	{"s2_M",				&stream_map[2].h264Conf.gopM,					MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
	{"s2_N",				&stream_map[2].h264Conf.gopN,					MAP_TO_U8,	    30.0,		0,		0.0,		0.0,	false},
	{"s2_idr_interval",		&stream_map[2].h264Conf.idrInterval,		MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
//	{"s2_gop_model",		&stream_map[2].h264.gop_model,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s2_profile",			&stream_map[2].h264.profile,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s2_brc",				&stream_map[2].h264.brc_mode,	        MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
	{"s2_cbr_avg_bps",		&stream_map[2].h264Conf.cbrAvgBps,		MAP_TO_U32,	    4000000.0,		0,		0.0,		0.0,	false},
//	{"s2_vbr_min_bps",		&stream_map[2].h264.vbr_min_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
//	{"s2_vbr_max_bps",		&stream_map[2].h264.vbr_max_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
	{"s2_quality",			&stream_map[2].mjpegConf.quality,			MAP_TO_U8,	    88.0,	    0,		0.0,		0.0,	false},
	{NULL,			        NULL,						            -1,	            0.0,	    0,	    0.0,        0.0,    false},
};

static mapping_t Stream3[] = {
	{"s3_h264_id",			&stream_map[3].h264Conf.streamId,				MAP_TO_U32,	    3.0,		0,		0.0,		0.0,	false},
	{"s3_M",				&stream_map[3].h264Conf.gopM,					MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
	{"s3_N",				&stream_map[3].h264Conf.gopN,					MAP_TO_U8,	    30.0,		0,		0.0,		0.0,	false},
	{"s3_idr_interval",		&stream_map[3].h264Conf.idrInterval,		MAP_TO_U8,	    1.0,		0,		0.0,		0.0,	false},
//	{"s3_gop_model",		&stream_map[3].h264.gop_model,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s3_profile",			&stream_map[3].h264.profile,			MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
//	{"s3_brc",				&stream_map[3].h264.brc_mode,	        MAP_TO_U8,	    0.0,		0,		0.0,		0.0,	},
	{"s3_cbr_avg_bps",		&stream_map[3].h264Conf.cbrAvgBps,		MAP_TO_U32,	    4000000.0,		0,		0.0,		0.0,	false},
//	{"s3_vbr_min_bps",		&stream_map[3].h264.vbr_min_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
//	{"s3_vbr_max_bps",		&stream_map[3].h264.vbr_max_bps,		MAP_TO_U32,	    0.0,		0,		0.0,		0.0,	},
	{"s3_quality",			&stream_map[3].mjpegConf.quality,			MAP_TO_U8,	    88.0,	    0,		0.0,		0.0,	false},
	{NULL,			        NULL,						            -1,	            0.0,	    0,	    0.0,	    0.0,    false},
};

static mapping_t EncodeMap[] = {
//	{"enc_mode",			&encode_mode,							MAP_TO_U32,	0.0,		0,		0.0,		0.0,	},

	{"s0_dptz",			&stream_map[0].dptz,							MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s0_type",			&stream_map[0].streamFormat.encode_type,			MAP_TO_U8,	1.0,		0,		0.0,		0.0,	false},
	{"s0_flip_rotate",		&stream_map[0].streamFormat.flip_rotate,			MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s0_width",			&stream_map[0].streamFormat.encode_width,		MAP_TO_U16,	1920.0,		0,		0.0,		0.0,	false},
	{"s0_height",			&stream_map[0].streamFormat.encode_height,		MAP_TO_U16,	1080.0,		0,		0.0,		0.0,	false},
	{"s0_enc_fps",		&stream_map[0].streamFormat.encode_fps,			MAP_TO_U32,	30.0,		0,		0.0,		0.0,	false},

	{"s1_dptz",			&stream_map[1].dptz,							MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s1_type",			&stream_map[1].streamFormat.encode_type,			MAP_TO_U8,	1.0,		0,		0.0,		0.0,	false},
	{"s1_flip_rotate",		&stream_map[1].streamFormat.flip_rotate,			MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s1_width",			&stream_map[1].streamFormat.encode_width,		MAP_TO_U16,	720.0,		0,		0.0,		0.0,	false},
	{"s1_height",			&stream_map[1].streamFormat.encode_height,		MAP_TO_U16,	480.0,		0,		0.0,		0.0,	false},
	{"s1_enc_fps",		&stream_map[1].streamFormat.encode_fps,			MAP_TO_U32,	30.0,		0,		0.0,		0.0,	false},

	{"s2_dptz",			&stream_map[2].dptz,							MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s2_type",			&stream_map[2].streamFormat.encode_type,			MAP_TO_U8,	1.0,		0,		0.0,		0.0,	false},
	{"s2_flip_rotate",		&stream_map[2].streamFormat.flip_rotate,			MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s2_width",			&stream_map[2].streamFormat.encode_width,		MAP_TO_U16,	0.0,		0,		0.0,		0.0,	false},
	{"s2_height",			&stream_map[2].streamFormat.encode_height,		MAP_TO_U16,	0.0,		0,		0.0,		0.0,	false},
	{"s2_enc_fps",		&stream_map[2].streamFormat.encode_fps,			MAP_TO_U32,	30.0,		0,		0.0,		0.0,	false},

	{"s3_dptz",			&stream_map[3].dptz,							MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s3_type",			&stream_map[3].streamFormat.encode_type,			MAP_TO_U8,	1.0,		0,		0.0,		0.0,	false},
	{"s3_flip_rotate",		&stream_map[3].streamFormat.flip_rotate,			MAP_TO_U8,	0.0,		0,		0.0,		0.0,	false},
	{"s3_width",			&stream_map[3].streamFormat.encode_width,		MAP_TO_U16,	0.0,		0,		0.0,		0.0,	false},
	{"s3_height",			&stream_map[3].streamFormat.encode_height,		MAP_TO_U16,	0.0,		0,		0.0,		0.0,	false},
	{"s3_enc_fps",		&stream_map[3].streamFormat.encode_fps,			MAP_TO_U32,	30.0,		0,		0.0,		0.0,	false},

	{NULL,			        NULL,						            -1,	            0.0,	    0,	    0.0,	    0.0,    },
};
static mapping_t OSDMap[] = {
	{"s0_bmp_enable",		&osd_map[0].bmp_enable,		MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s0_time_enable",		&osd_map[0].time_enable,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s0_text_enable",		&osd_map[0].text_enable,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s0_no_rotate",		&osd_map[0].no_rotate,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		2.0,	false},
	{"s0_text",				&osd_map[0].text.str,		MAP_TO_STRING,	0.0,		0,		0.0,		0.0,		OSD_TEXT_LENGTH,	false},
	//{"s0_text_type",		&osd_map[0].text.type,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		5,	false},
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
	//{"s1_text_type",		&osd_map[1].text.type,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		5,	false},
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
	//{"s2_text_type",		&osd_map[2].text.type,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		5,	false},
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
	//{"s3_text_type",		&osd_map[3].text.type,	MAP_TO_U8,	0.0,		MIN_MAX_LIMIT,		0.0,		5,	false},
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
static mapping_t PMMap[] = {
	{"pm_x",			&pm_map.x,		MAP_TO_U32,	0.0,		MIN_MAX_LIMIT,		0.0,		10000.0,	false},
	{"pm_y",			&pm_map.y,		MAP_TO_U32,	0.0,		MIN_MAX_LIMIT,		0.0,		10000.0,	false},
	{"pm_w",				&pm_map.width,		MAP_TO_U32,	0.0,		MIN_MAX_LIMIT,		0.0,		10000.0,	false},
	{"pm_h",				&pm_map.height,		MAP_TO_U32,	0.0,		MIN_MAX_LIMIT,		0.0,		10000.0,	false},
	{"pm_color",			&pm_map.color,		MAP_TO_U32,	0.0,		MIN_MAX_LIMIT,		0.0,		
COLOR_TYPE_NUM,	false},
	{"pm_action",			&pm_map.action,		MAP_TO_U32,	0.0,		MIN_MAX_LIMIT,		0.0,		
4,	false},

	{NULL,			NULL,						-1,	0.0,					0,	0.0,	0.0,		false},
};

static mapping_t LIVEMap[] = {
//	{"encode_type",		&live_map.encode_type,	MAP_TO_U32,	0.0,		0,		0.0,		0.0,	
// false,},
//	{"encode_fps",		&live_map.encode_fps,	MAP_TO_U32,	0.0,		0,		0.0,		0.0,	
// false,},
//	{"brc_mode",		&live_map.brc_mode,		MAP_TO_U32,	0.0,		0,		0.0,		0.0,	
// false,},
	{"cbr_avg_bps",		&live_map.cbr_avg_bps,	MAP_TO_U32,	0.0,		0,		0.0,		0.0,	
	false,},
//	{"encode_width",	&live_map.encode_width,	MAP_TO_U16,	0.0,		0,		0.0,		0.0,	
// false,},
//	{"encode_height",	&live_map.encode_height,MAP_TO_U16,	0.0,		0,		0.0,		0.0,	
// false,},
//	{"vbr_min_bps",		&live_map.vbr_min_bps,	MAP_TO_U32,	0.0,		0,		0.0,		0.0,	
// false,},
//	{"vbr_max_bps",		&live_map.vbr_max_bps,	MAP_TO_U32,	0.0,		0,		0.0,		0.0,	
// false,},

	{NULL,			NULL,						-1,	0.0,					0,	0.0,	0.0,		
	false,},
};
static mapping_t ImageMap[] = {
	{"exposure_mode",	&iq_map.ae.anti_flicker_mode,		MAP_TO_U32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},
	{"shutter_min",		&iq_map.ae.shutter_time_min,		MAP_TO_U32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},
	{"shutter_max",		&iq_map.ae.shutter_time_max,		MAP_TO_U32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},
	{"max_gain",			&iq_map.ae.sensor_gain_max,		MAP_TO_U16,	0.0,		NO_LIMIT,		0.0,		0.0,	false},
	{"slow_shutter",		&iq_map.ae.slow_shutter_enable,	MAP_TO_U32,	1.0,		NO_LIMIT,		0.0,		0.0,	false},
	{"vin_fps",			&iq_map.ae.current_vin_fps,		MAP_TO_U32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},

	{"ae_preference",		&iq_map.ae_preference,			MAP_TO_S32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},
	{"dn_mode",			&iq_map.day_night_mode,			MAP_TO_U32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},
	{"metering_mode",		&iq_map.metering_mode,			MAP_TO_U32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},
	{"backlight_comp",		&iq_map.back_light_comp_enable,	MAP_TO_U32,	0.0,		MIN_MAX_LIMIT,	0.0,		2.0,	false},
	{"local_exposure",		&iq_map.local_exposure_mode,		MAP_TO_U32,	0.0,		MIN_MAX_LIMIT,	0.0,		5.0,	false},
	{"mctf_strength",		&iq_map.mctf_strength,			MAP_TO_U32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},
	{"dc_iris",			&iq_map.dc_iris_enable,			MAP_TO_U32,	0.0,		MIN_MAX_LIMIT,	0.0,		2.0,	false},
	{"dc_iris_duty",		&iq_map.dc_iris_duty_balance,		MAP_TO_U32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},
	{"ae_target_ratio",		&iq_map.ae_target_ratio,			MAP_TO_U32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},

	{"saturation",			&image_map.saturation,			MAP_TO_S32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},
	{"brightness",			&image_map.brightness,			MAP_TO_S32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},
	{"hue",				&image_map.hue,				MAP_TO_S32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},
	{"contrast",			&image_map.contrast,				MAP_TO_S32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},
	{"sharpness",			&image_map.sharpness,			MAP_TO_S32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},

	{"wbc",				&awb_map.wb_mode,				MAP_TO_U32,	0.0,		NO_LIMIT,		0.0,		0.0,	false},

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
	{"LIVE",		LIVEMap,		get_live_param,		set_func_null},
    {"IMAGE",		ImageMap,		get_image_param,		set_image_param},
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

static int check_params(mapping_t * Map)
{
	int i = 0;

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

	return 0;
}

static int update_params(mapping_t * Map)
{
	check_params(Map);
//	memcpy(pInp, &cfg, sizeof(InputParameters));
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


static int parse_name_to_map_index(mapping_t * Map, char * s)
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

static void parse_content(mapping_t * Map, char * buf, int bufsize)
{
	char		* items[MAX_ITEMS_TO_PARSE] = {NULL};
	int		MapIndex;
	int		item = 0;
	int		IntContent;
	u32		U32Content;
	double	DoubleContent;
	char		msg[256];
	int i;

	// Stage one: Generate an argc/argv-type list in items[], without comments and whitespace.
	// This is context insensitive and could be done most easily with lex(1).
	item = parse_items(items, buf, bufsize);
#if 0
    for (i=0;i<=item;i++)
    {
        PRT_DBG("item[%d]items [%d][%s]\n", item, i, items[i]);
    }
#endif
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
                Map[MapIndex].Update = true;
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
}
static int load_default_params(mapping_t * Map)
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

static int input_params(mapping_t *Map, const char * content)
{
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
	return 0;
}

int output_params(mapping_t * Map, char ** pContentOut)
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
			PRT_DBG("\n-----Port [%d] closed-----\n\n", WEBSERVICE_PORT);
			return -2;
		}
		PRT_ERR("recv() returns %d.", retv);
		return -1;
	}

    if (size == 8)
    {
        FUN_OUT("\n-----ACK.result[%d]ACK.info[%d]-----\n", ((Ack *)pBuffer)->result, ((Ack *)pBuffer)->info);
    }
    else
    {
        FUN_OUT("\n-----not ACK content[%s]size[%d]-----\n", pBuffer, size);
    }
	return retv;
}

static int get_live_param(char * section_name, u32 info)
{
	PRT_DBG("\nGet View parameters of stream [%d].\n", info);

	live_map.cbr_avg_bps = stream_map[info].h264Conf.cbrAvgBps = 
	g_stEncodeInfo[info].bitrate*1000;//bps


	PRT_DBG("\n---------- Get View params --- [%d] ------\n"
            "         cbr_avg_bps   = %d\n"
            "------------------------------------------\n",
            info, live_map.cbr_avg_bps);

	return 0;
}

static int get_image_param(char * section_name, u32 info)
{
    GADI_ERR retVal = GADI_OK;

    FUN_IN("section_name [%s] ispHandle[0x%x]\n", section_name, ispHandle);

    if (!ispHandle)
    {
        ispHandle = gadi_isp_open(&openParams, &retVal);
        if (ispHandle == NULL || retVal != GADI_OK)
        {
            PRT_ERR("gadi_isp_open error retVal = %d\n", retVal);
        }

    }    
    /* image attribute */
    retVal = gadi_isp_image_get_attr(ispHandle, &imageAttr);
    if (retVal != GADI_OK)
    {
        PRT_ERR("gadi_isp_image_get_attr() error retVal = %d\n", retVal);
        return (-1);
    }

    /* auto exposure attribute */
    retVal = gadi_isp_ae_get_attr(ispHandle, &aeAttr);
    if (retVal != GADI_OK)
    {
        PRT_ERR("gadi_isp_ae_get_attr() error retVal = %d\n", retVal);
        return (-2);
    }

    /* auto white balance attribute */
    retVal = gadi_isp_awb_get_attr(ispHandle, &awbAttr);
    if (retVal != GADI_OK)
    {
        PRT_ERR("gadi_isp_awb_get_attr() error retVal = %d\n", retVal);
        return (-3);
    }
    /*image*/
    image_map.saturation = imageAttr.saturation;
    image_map.brightness = imageAttr.brightness;
    image_map.hue        = imageAttr.hue;
    image_map.contrast   = imageAttr.contrast;
    image_map.sharpness  = imageAttr.sharpness;
    iq_map.day_night_mode         = imageAttr.dayNightEnable;
    //iq_map.metering_mode          = ?
    iq_map.back_light_comp_enable = imageAttr.backLightCompEnable;
    iq_map.local_exposure_mode    = imageAttr.localExpMode;
    iq_map.mctf_strength          = imageAttr.mctfStrength;
    //iq_map.dc_iris_enable         = ?
    //iq_map.ae_target_ratio        = ?

    /*ae*/
    iq_map.ae.anti_flicker_mode   = aeAttr.antiFlickerFreq;
    iq_map.ae.shutter_time_min    = aeAttr.shutterTimeMin;
    iq_map.ae.shutter_time_max    = aeAttr.shutterTimeMax;
    //iq_map.ae.sensor_gain_max       = ?
    //iq_map.ae.slow_shutter_enable   = ?
    //iq_map.ae.current_vin_fps       = ?
    //
    //iq_map.ae_preference            = ?
    
    /*awb*/
    awb_map.wb_mode               = awbAttr.mode;
    PRT_DBG("\n"
            "\t-------IMAGE--------\n"
            "saturation\t\t=\t%d\n"
            "brightness\t\t=\t%d\n"
            "hue\t\t\t=\t%d\n"
            "contrast\t\t=\t%d\n"
            "sharpness\t\t=\t%d\n"
            "backLightCompEnable\t=\t%d\n"
            "localExpMode\t\t=\t%d\n"
            "mctfStrength\t\t=\t%d\n"
            "\t--------ae--------\t\n"
            "antiFlickerFreq\t\t=\t%d\n"
            "shutterTimeMin\t\t=\t%d\n"
            "shutterTimeMax\t\t=\t%d\n"
            "\t-------awb--------\t\n"
            "wb_mode\t\t\t=\t%d\n",
            imageAttr.saturation,
            imageAttr.brightness,
            imageAttr.hue,
            imageAttr.contrast,
            imageAttr.sharpness,
            imageAttr.backLightCompEnable,
            imageAttr.localExpMode,
            imageAttr.mctfStrength,
            aeAttr.antiFlickerFreq,
            aeAttr.shutterTimeMin,
            aeAttr.shutterTimeMax,
            awbAttr.mode
            );

    FUN_OUT();

    return (0);
}

static int get_encode_param(char * section_name, u32 info)
{
	int streamID = info, retv = 0;

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
static int set_basic_image_params(basic_iq_params * iq_params)
{
//mark
#if 0
    gadi_isp_image_set_attr();
	if (g_mw_basic_iq.day_night_mode != iq_params->day_night_mode) {
		if (mw_enable_day_night_mode(iq_params->day_night_mode) < 0) {
			PRT_ERR("mw_enable_day_night_mode error\n");
			return -1;
		}
	}
	if (g_mw_basic_iq.metering_mode != iq_params->metering_mode) {
		if (mw_set_ae_metering_mode(iq_params->metering_mode) < 0) {
			PRT_ERR("mw_set_ae_metering_mode error\n");
			return -1;
		}
	}
	if (g_mw_basic_iq.back_light_comp_enable != iq_params->back_light_comp_enable) {
		if (mw_enable_backlight_compensation(iq_params->back_light_comp_enable) < 0) {
			PRT_ERR("mw_enable_backlight_compensation error\n");
			return -1;
		}
	}
	if (g_mw_basic_iq.local_exposure_mode != iq_params->local_exposure_mode) {
		if (set_local_exposure_mode(iq_params->local_exposure_mode) < 0) {
			PRT_ERR("set_local_exposure_mode error\n");
			return -1;
		}
	}
	if (g_mw_basic_iq.mctf_strength != iq_params->mctf_strength) {
		if (mw_set_mctf_strength(iq_params->mctf_strength) < 0) {
			PRT_ERR("mw_set_mctf_strength error\n");
			return -1;
		}
	}
	if (g_mw_basic_iq.dc_iris_enable != iq_params->dc_iris_enable) {
		if (mw_enable_dc_iris_control(iq_params->dc_iris_enable) < 0) {
			PRT_ERR("mw_enable_dc_iris_control error\n");
			return -1;
		}
	}
	/*if (g_mw_basic_iq.dc_iris_duty_balance != iq_params->dc_iris_duty_balance) {
		if (mw_set_dc_iris_balance_duty(iq_params->dc_iris_duty_balance) < 0) {
			APP_ERROR("mw_set_dc_iris_balance_duty error\n");
			return -1;
		}
	}*/
	if (g_mw_basic_iq.ae_target_ratio != iq_params->ae_target_ratio) {
		if (mw_set_exposure_level(iq_params->ae_target_ratio) < 0) {
			PRT_ERR("mw_set_exposure_level error\n");
			return -1;
		}
	}
#endif
	return 0;
}

static int show_bmp(void)
{
	int retVal = 0;
    static GADI_OSD_BmpParamsT bmpPars =
    {
	    .planeId   = 0,
	    .areaId    = 2,
	    .enable    = 1,
	    .startX    = 0,
	    .startY    = 0,
	    .bmpFile   = "/webSvr/www/img/goke.bmp",
    };
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
    retVal = gadi_osd_show_bmp(osdHandle, &bmpPars);

    return retVal;

}

static int show_date(int stream, int areaId, int enable)
{
	GADI_VENC_StreamFormatT formatPar;
    int width,height=0;
    int streamId=-1;
    static GADI_OSD_TextParamsT textPar =
    {
    	.planeId   = 0,
    	.areaId    = 0,
    	.enable    = 1,
    	.type	   = 0,
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
    
    streamId = formatPar.streamId = textPar.planeId;

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
    width = stream_map[streamId].streamFormat.encode_width;
    height = stream_map[streamId].streamFormat.encode_height;
	retVal = gadi_osd_show_date(osdHandle, &textPar, width, height);

	return retVal;

}
static int osd_set_text(GADI_OSD_TextParamsT* textParam, int streamId)
{
	int retVal = 0;
    int width,height=0;
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
    width = stream_map[streamId].streamFormat.encode_width;
    height = stream_map[streamId].streamFormat.encode_height;
    
    PRT_DBG("\n"
            "======resolution======\n"
            "         %d X %d      \n"
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
            "boxHeight  :       %d\n"
            "=====================\n",
            width, height,
            textParam->planeId, textParam->areaId,
            textParam->enable, textParam->textStr,
            textParam->textLen, textParam->type,
            textParam->size, textParam->color,
            textParam->outline, textParam->bold,
            textParam->italic, textParam->startX,
            textParam->startY, textParam->boxWidth,
            textParam->boxHeight
            );
	retVal = gadi_osd_show_text(osdHandle, textParam, width, height);

	return retVal;

}


static int add_pm(const privacy_mask_t *pm_param)
{
    int retVal;
    unsigned char pmIndex;
    GADI_U32 enable = 1;

    if (!pmHandle)    
    {
        retVal = gadi_pm_init();
        pmHandle = gadi_pm_open(&retVal);
        if(retVal != GADI_OK)
        {
            PRT_ERR("gadi_pm_open error\n");
            goto err;
        }
    }
    retVal = gadi_pm_malloc(pmHandle, (void *)pm_param, &pmIndex);
    if(retVal != 0)
    {
        PRT_ERR("gadi_pm_malloc error:%d\n",retVal);
        goto err;
    }

    retVal =  gadi_pm_enable(pmHandle, pmIndex, enable);
    if(retVal != 0)
    {
        PRT_ERR("gadi_pm_enable error:%d\n",retVal);
        goto err;
    }

    return retVal;    
err:
    PRT_ERR("add_pm failed!\n");
    return retVal;    
}
static int set_pm_param(char * section_name)
{
	privacy_mask_t pm;
	pm.unit = 1;
	pm.width = (int)pm_map.width*(1920.0/800);//max width
	pm.height = (int)pm_map.height*(1080.0/600);//max height
	pm.x = (int)pm_map.x*(1920.0/800);//max width
	pm.y = (int)pm_map.y*(1080.0/600);//max height
	pm.action = pm_map.action;
	pm.color = pm_map.color;
    PRT_DBG("\n---------pm-----------\n"
            "witdh  :            %d\n"
            "height  :           %d\n"
            "x  :                %d\n"
            "y  :                %d\n"
            "action  :           %d\n"
            "color  :            %d\n"
            "----------------------\n",
            pm.width, pm.height, pm.x,
            pm.y, pm.action, pm.color
            );
    add_pm(&pm);
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
	int i,j;
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

    for (i = 0; i < CUR_STREAM_NUM; ++i) 
    {

        /*-----------------------------------------------------------------------------
         *  check update
         *-----------------------------------------------------------------------------*/
        for (j=0; j<14; j++)//15 var
        {
            if (OSDMap[i*14 + j].Update)
            {
                /*-----------------------------------------------------------------------------
                 *  bmp
                 *-----------------------------------------------------------------------------*/
                if (osd_map[i].bmp_enable!= g_mw_osd[i].bmp_enable) {
                    if (show_bmp() < 0) {
                        PRT_ERR("Add osd bmp failed !\n");
                        return -1;
                    } else {
                        g_mw_osd[i].bmp_enable = osd_map[i].bmp_enable;
                    }
                }
                
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
                osd_map[i].text.length = strlen(osd_map[i].text.str);
                osd_map[i].text.stream = i;
                PRT_DBG("\n-----Enable osd text string [%s], length [%d].-----\n",
                        osd_map[i].text.str, osd_map[i].text.length);

                textParam.planeId   = osd_map[i].text.stream;
                textParam.areaId    = 2;//0:bmp 1:time 2:show text
                textParam.enable    = 1;//osd_map[i].text_enable;
                if (osd_map[i].text.length == 0)
                {
                    strncpy(osd_map[i].text.str, " ", 2);
                    osd_map[i].text.length = 1;
                /*-----------------------------------------------------------------------------
                 *  clean text
                 *-----------------------------------------------------------------------------*/
                    textParam.enable = 0;
                }
                textParam.textLen   = osd_map[i].text.length;
                    
                strncpy(textParam.textStr, osd_map[i].text.str, textParam.textLen+1);
                textParam.type      = 0;//osd_map[i].text.type;
                textParam.size      = osd_map[i].text.size;
                textParam.color     = osd_map[i].text.color;
                textParam.outline   = osd_map[i].text.outline;
                textParam.bold      = osd_map[i].text.bold;
                textParam.italic    = osd_map[i].text.italic;
                textParam.startX    = osd_map[i].text.start_x;
                textParam.startY    = osd_map[i].text.start_y;
                textParam.boxWidth  = osd_map[i].text.box_w;
                textParam.boxHeight = osd_map[i].text.box_h;

                if (osd_set_text(&textParam, i) < 0) {
                    PRT_ERR("Add osd text string [%s] failed !\n", osd_map[i].text.str);
                    return (-1);
                } else {
                    PRT_DBG("\n-----Add osd text string [%s] successed!-----\n", osd_map[i].text.str);
                }
                OSDMap[i*15 + j].Update = false;
                break;
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
    int retVal=0;
    int i=0;

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
    for (i=5; i<sizeof(EncodeMap)/sizeof(mapping_t); i+=6)
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
    for (i=3; i<sizeof(EncodeMap)/sizeof(mapping_t); i+=6)
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
    for (i=2; i<sizeof(EncodeMap)/sizeof(mapping_t); i+=6)
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
            return (-1);
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

static int set_image_param(char * section_name)
{
    GADI_ERR retVal = GADI_OK;

	FUN_IN("Section [%s] settings:\n", section_name);
    if (!ispHandle)
    {
        ispHandle = gadi_isp_open(&openParams, &retVal);
        if (ispHandle == NULL || retVal != GADI_OK)
        {
            GADI_ERROR("gadi_isp_open error retVal = %d\n", retVal);
        }

    }
    imageAttr.saturation = image_map.saturation;
    imageAttr.brightness = image_map.brightness;
    imageAttr.hue        = image_map.hue;
    imageAttr.contrast   = image_map.contrast;
    imageAttr.sharpness  = image_map.sharpness; 
    imageAttr.dayNightEnable      = iq_map.day_night_mode;
    //iq_map.metering_mode          = ?
    imageAttr.backLightCompEnable = iq_map.back_light_comp_enable;
    imageAttr.localExpMode        = iq_map.local_exposure_mode;
    imageAttr.mctfStrength        = iq_map.mctf_strength;
    //iq_map.dc_iris_enable         = ?
    //iq_map.ae_target_ratio        = ?

    /*ae*/
    aeAttr.antiFlickerFreq        = iq_map.ae.anti_flicker_mode;
    aeAttr.shutterTimeMin         = iq_map.ae.shutter_time_min;
    aeAttr.shutterTimeMax         = iq_map.ae.shutter_time_max;
    //iq_map.ae.sensor_gain_max       = ?
    //iq_map.ae.slow_shutter_enable   = ?
    //iq_map.ae.current_vin_fps       = ?
    //
    //iq_map.ae_preference            = ?
    
    /*awb*/
    awbAttr.mode                  = awb_map.wb_mode;


    PRT_DBG("\n"
            "\t-------IMAGE--------\n"
            "saturation\t\t=\t%d\n"
            "brightness\t\t=\t%d\n"
            "hue\t\t\t=\t%d\n"
            "contrast\t\t=\t%d\n"
            "sharpness\t\t=\t%d\n"
            "backLightCompEnable\t=\t%d\n"
            "localExpMode\t\t=\t%d\n"
            "mctfStrength\t\t=\t%d\n"
            "\t--------ae--------\t\n"
            "antiFlickerFreq\t\t=\t%d\n"
            "shutterTimeMin\t\t=\t%d\n"
            "shutterTimeMax\t\t=\t%d\n"
            "\t-------awb--------\t\n"
            "wb_mode\t\t\t=\t%d\n",
            imageAttr.saturation,
            imageAttr.brightness,
            imageAttr.hue,
            imageAttr.contrast,
            imageAttr.sharpness,
            imageAttr.backLightCompEnable,
            imageAttr.localExpMode,
            imageAttr.mctfStrength,
            aeAttr.antiFlickerFreq,
            aeAttr.shutterTimeMin,
            aeAttr.shutterTimeMax,
            awbAttr.mode
            );

    if (GADI_OK != gadi_isp_image_set_attr(ispHandle, &imageAttr))
    {
        PRT_ERR("gadi_isp_image_set_attr\n");
    }
    if (GADI_OK != gadi_isp_ae_set_attr(ispHandle, &aeAttr))
    {
        PRT_ERR("gadi_isp_ae_set_attr\n");
    }
    if (GADI_OK != gadi_isp_awb_set_attr(ispHandle, &awbAttr))
    {
        PRT_ERR("gadi_isp_awb_set_attr\n");
    }    

#if 0
	if (g_mw_basic_iq.ae_preference != iq_map.ae_preference) {
		if (mw_set_iq_preference(iq_map.ae_preference) < 0) {
			PRT_ERR("mw_set_iq_preference error\n");
		}
		g_mw_basic_iq.ae_preference = iq_map.ae_preference;
	}
	if (0 != memcmp(&g_mw_basic_iq.ae, &iq_map.ae, sizeof(g_mw_basic_iq.ae))) {
		if (mw_set_ae_param(&iq_map.ae) < 0) {
			PRT_ERR("mw_set_ae_param error\n");
			return -1;
		} else {
			g_mw_basic_iq.ae = iq_map.ae;
		}
	}
	if (0 != memcmp(&g_mw_image, &image_map, sizeof(g_mw_image))) {
		if (mw_set_image_param(&image_map) < 0) {
			PRT_ERR("mw_set_image_param error\n");
			return -1;
		} else {
			g_mw_image = image_map;
		}
	}
	if (0 != memcmp(&g_mw_awb, &awb_map, sizeof(g_mw_awb))) {
		if (mw_set_awb_param(&awb_map) < 0) {
			PRT_ERR("mw_set_awb_param error\n");
			return -1;
		} else {
			g_mw_awb = awb_map;
		}
	}
	if (0 != memcmp(&g_mw_basic_iq, &iq_map, sizeof(g_mw_basic_iq))) {
		if (set_basic_image_params(&iq_map) < 0) {
			PRT_ERR("set_basic_image_params error\n");
			return -1;
		} else {
			g_mw_basic_iq = iq_map;
		}
	}
#endif
	return 0;
}

static int get_stream_param(char *section_name, u32 info)
{
    int stream_id = 0, retv = 0;

    stream_id = atoi(&section_name[6]);
    stream_map[stream_id].h264Conf.cbrAvgBps =
        g_stEncodeInfo[stream_id].bitrate*1000;
    //h264
    if (!vencHandle)
    {
        PRT_ERR("vencHandle is null\n");
        return (-1);
    }
    if(gadi_venc_get_h264_config(vencHandle, &stream_map[stream_id].h264Conf)<0)
    {
        PRT_ERR("adi: get_h264: error!\n");
        return (-2);
    }
    //mpeg
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
#else
    FUN_OUT("\n"
            "dptz             [%d]\n"
            "flipRotate       [%d]\n"
            "fps              [%d]\n"
            "resolution     [%d X %d]\n"
            "h264 M           [%d]\n"
            "h264 N           [%d]\n"
            "h264 IDR         [%d]\n"
            "mjpeg qa         [%d]\n"
            , 
            stream_map[stream_id].dptz, stream_map[stream_id].streamFormat.flip_rotate,   
            stream_map[stream_id].streamFormat.encode_fps,
            stream_map[stream_id].streamFormat.encode_width,
            stream_map[stream_id].streamFormat.encode_height,
            stream_map[stream_id].h264Conf.gopM,
            stream_map[stream_id].h264Conf.gopN,
            stream_map[stream_id].h264Conf.idrInterval,
            stream_map[stream_id].mjpegConf.quality
    );
 #endif
    //stream_map[streamId].h264Conf.gopM = 
    //stream_map[streamId].h264Conf.gopN = 
    //stream_map[streamId].h264Conf.idrInterval = 
    //stream_map[streamId].mjpegConf.quality = 
    FUN_OUT("cbrAvgBps [%d]\n", stream_map[stream_id].h264Conf.cbrAvgBps);

	return retv;
}

static int set_stream_param(char *section_name)
{
    int streamId = 0; 
    int retVal = -1;
    int h264_update = false;
    int i=0;
    GADI_VENC_BitRateRangeT stBitRate;
    
    streamId = atoi(&section_name[6]);
    FUN_OUT("cbrAvgBps [%d]\n", stream_map[streamId].h264Conf.cbrAvgBps);
    if (!vencHandle)
    {
       PRT_ERR("vencHandle is NULL\n");
       return (-2);
    }
    /*-----------------------------------------------------------------------------
     *  set bitrate
     *-----------------------------------------------------------------------------*/
    if (Params[streamId + 2].map[4].Update)
    {
	    stBitRate.streamId 	= streamId;

        stBitRate.minBps	= stream_map[streamId].h264Conf.cbrAvgBps;
	    stBitRate.maxBps	= stream_map[streamId].h264Conf.cbrAvgBps;//streams[streamId].h264Conf.vbrMaxbps;
    

        if(GADI_OK != gadi_venc_set_bitrate(vencHandle, &stBitRate))
        {
            PRT_ERR("adi:set bitrate failed\n");
            return (-1);
        }
        g_stEncodeInfo[streamId].bitrate = stream_map[streamId].h264Conf.cbrAvgBps;
        Params[streamId + 2].map[4].Update = false;
    }

#if 1
    /*-----------------------------------------------------------------------------
     *  H264 set M N idrInterval 
     *-----------------------------------------------------------------------------*/
    for (i=1; i<=5 ;i++)
    {
        if (Params[streamId + 2].map[i].Update)
        {
            h264_update = true;
            Params[streamId + 2].map[i].Update = false;
        }
    }
    if (true == h264_update)
    {
        if(gapp_video_stop_encode_stream() < 0)
        {    
            PRT_ERR("stop encode failed!\n");
            return -1;
        }

        if(gadi_vin_enter_preview(vinHandle, 0) < 0)
        {
            PRT_ERR("gadi_vin_enter_preview enter error.\n");
            return -1;
        }
        
        if((retVal = gadi_venc_set_h264_config(vencHandle, 
        &stream_map[streamId].h264Conf)) < 0)
        {
            PRT_ERR("gadi_venc_set_h264_config error, steamId=%d\n", streamId);
            return (-1);
        }
        
        if(gadi_vin_enter_preview(vinHandle, 1) < 0)
        {
            PRT_ERR("gadi_vin_enter_preview enter error.\n");
            return -1;
        }

        if(gapp_video_start_encode_stream() < 0)
        {    
            PRT_ERR("start encode failed!\n");
            return -1;
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
	PRT_DBG("\n------Section Name is : %s------\n", (char *)name);

	section = find_section((char *)name);
	if (NULL == section) {
		ack.result = -1;
		ack.info = -1;
		PRT_DBG("\n------Section [%s] is not found!------\n", (char *)name);
		return send_text((u8 *)&ack, sizeof(ack));
	} else {
        /*-----------------------------------------------------------------------------
         *  1.from board to map
         *  2.from map to cgi
         *-----------------------------------------------------------------------------*/
		retv = (*section->get)(section->name, req->info);
		retv = output_params(section->map, &content);
		PRT_DBG("\n------------content-----------\n"
                "[%s]\n"
                "-------------------------------\n", content);
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
	PRT_DBG("\n------Section Name is : %s------\n", (char *)name);
	section = find_section((char *)name);
	ack.info = ack.result = (NULL == section) ? -1 : 0;
	retv = send_text((u8 *)&ack, sizeof(ack));
	if (ack.result == -1) {
		PRT_DBG("\n------Section [%s] is not found!------\n", (char *)name);
		return -1;
	}

	// Receive parameter settings
	retv = receive_text((u8 *)content, req->info);
	content[req->info] = 0;
	if (0) {
		PRT_DBG("\n------CGI Setting : content-----\n"
                "[%s]\n"
                "---------------------------------\n", content);
	} else {
		PRT_DBG("CGI Parameter Setting is : content[%s]\n", content);

        /*-----------------------------------------------------------------------------
         *  1.from cgi to map
         *  2.from map to board
         *-----------------------------------------------------------------------------*/
		retv = input_params(section->map, content);
		retv = (*section->set)(section->name);
        PRT_DBG("\nACK.result[%d] ack.info[%d]\n", ack.result, ack.info);
		ack.result = ack.info = retv;
	}
	retv = send_text((u8 *)&ack, sizeof(ack));
	return retv;
}
static int do_change_br(Request *req)
{
	int retv = 0, stream_id = 0;
	Ack ack;
    GADI_VENC_BitRateRangeT bitrate;
    
    stream_id = (req->info >> STREAM_ID_OFFSET);
    u32 cbr_avg_bps = (req->info & ~(0xf <<
    STREAM_ID_OFFSET));
    if (cbr_avg_bps != stream_map[stream_id].h264Conf.cbrAvgBps)
    {
        Params[stream_id + 2].map[4].Update      = true;
        stream_map[stream_id].h264Conf.cbrAvgBps = cbr_avg_bps;
    }

    /*-----------------------------------------------------------------------------
     *  set bitrate
     *-----------------------------------------------------------------------------*/

    if (true == Params[stream_id + 2].map[4].Update)
    {
	    bitrate.streamId 	= stream_id;

        bitrate.minBps	= stream_map[stream_id].h264Conf.cbrAvgBps;
	    bitrate.maxBps	= stream_map[stream_id].h264Conf.cbrAvgBps;//streams[streamId].h264Conf.vbrMaxbps;
    

        if(GADI_OK != gadi_venc_set_bitrate(vencHandle, &bitrate))
        {
            PRT_ERR("adi:set bitrate failed\n");
            return (-1);
        }
        g_stEncodeInfo[stream_id].bitrate = 
        stream_map[stream_id].h264Conf.cbrAvgBps/1000;//kbps
        Params[stream_id + 2].map[4].Update = false;
        PRT_DBG("STREAM[%d] bitrate[%d](bps)\n", stream_id, cbr_avg_bps);
    }
	ack.result = retv;
	ack.info = 0;
    raise(XMLSAVE);
	return send_text((u8 *)&ack, sizeof(ack));
}

static int process_request(Request *req)
{
	PRT_DBG("\n-----Process Request ID [%d]-----\n", (int)req->id);
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

	PRT_DBG("\n-----Listen to %d-----\n", WEBSERVICE_PORT);
	if ((sockfd2 = accept(sockfd, (struct sockaddr *)&client_addr, &length)) < 0) {
		PRT_ERR("accept failed %d\n", sockfd2);
		return -1;
	}
	PRT_DBG("\n-----Port [%d] connected!client ip[%s]-----\n", WEBSERVICE_PORT, inet_ntoa((*((struct 
	sockaddr_in*)&client_addr)).sin_addr));

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
	server_addr.sin_port = htons(WEBSERVICE_PORT);
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
	PRT_DBG("\n-----[Done] Create web_service-----\n");
	return 0;
}
static int load_default_configs()
{
	char string[10 * 1024], *content;

	content = string;
	input_params(ImageMap, NULL);
	output_params(ImageMap, &content);

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
