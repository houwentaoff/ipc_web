/*
****************************************************************************
*
** \file      map.c
**
** \version   $Id: map.c 2410 2014-11-18 13:35:07Z houwentao $
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
#include "map.h"

static mw_vin_mode			vin_map;
static mw_vout_mode			vout_map;

static Mapping VinVoutMap[] = {
	{"vin_enable",			&vin_map.enable,				MAP_TO_U32,	0.0,		0,		0.0,		0.0,	},
	{"vin_mode",			&vin_map.mode,				MAP_TO_U32,	AMBA_VIDEO_MODE_AUTO,		0,		0.0,		0.0,	},
	{"vin_framerate",		&vin_map.frame_rate,			MAP_TO_U32,	AMBA_VIDEO_FPS_29_97,		0,		0.0,		0.0,	},
	{"vin_mirror",			&vin_map.mirror_mode.mirror_pattern,	MAP_TO_U32,	0.0,		0,		0.0,		0.0,	},
	{"vin_bayer",			&vin_map.mirror_mode.bayer_pattern,	MAP_TO_U32,	0.0,		0,		0.0,		0.0,	},

	{"vout_type",			&vout_map.type,			MAP_TO_U32,	AMBA_VOUT_SINK_TYPE_CVBS,		0,		0.0,		0.0,	},
	{"vout_mode",			&vout_map.mode,			MAP_TO_U32,	AMBA_VIDEO_MODE_480I,		0,		0.0,		0.0,	},

	{NULL,			NULL,						-1,	0.0,					0,	0.0,	0.0,		},
};

static Section Params[] = {
	{"VINVOUT",		VinVoutMap,		get_func_null,		set_vinvout_param},
	{"ENCODE",		EncodeMap,		get_func_null,		set_encode_param},
	{"STREAM0",		Stream0,		get_func_null,		set_stream_param},
	{"STREAM1",		Stream1,		get_func_null,		set_stream_param},
	{"STREAM2",		Stream2,		get_func_null,		set_stream_param},
	{"STREAM3",		Stream3,		get_func_null,		set_stream_param},
	{"PRIMASK",		PMMap,			get_func_null,		set_pm_param},
	{"OSD",			OSDMap,			get_func_null,		set_osd_param},
	{"DPTZ",		DPTZMap,		get_dptz_param,		set_dptz_param},
	{"LIVE",		LIVEMap,		get_live_param,		set_func_null},
	{NULL,			NULL,			NULL,				NULL}
};

typedef enum{
    VINVOUT=0,
    ENCODE,
    NULL,
}section_index
