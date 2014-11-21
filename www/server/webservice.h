/*
****************************************************************************
*
** \file      webservice.h
**
** \version   $Id: webservice.h 2409 2014-11-18 13:25:00Z houwentao $
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
#ifndef __WEBSERVICE_H__
#define __WEBSERVICE_H__


//#define SERVER_CONFIG_PATH		"/etc/ambaipcam/mediaserver"

#define ENCODE_SERVER_PORT			(20000)
//#define IMAGE_SERVER_PORT			(20002)
#define BUFFER_SIZE					(1024)
#define ENCODE_STREAM_NUM           (4)
#define STREAM_ID_OFFSET			(28)

#define false                  (0)       /*  */
#define true                !(false)      /*  */

//#define	FILE_CONTENT_SIZE		(10 * 1024)
//
typedef int bool;
typedef unsigned int u32;
typedef unsigned char u8;
typedef signed char s8;
typedef signed int s32;
typedef signed short s16;
typedef unsigned short u16;

/*=========================Start AMBA_VIDEO_INFO==============================*/
#if 0
#define AMBA_VIDEO_FPS(fps)				(fps)
#define AMBA_VIDEO_FRAC_FPS(fps)		(fps)
#define AMBA_VIDEO_FPS_AUTO			0			//0
#define AMBA_VIDEO_FPS_1				512000000	//(512000000 / 1)
#define AMBA_VIDEO_FPS_2				256000000	//(512000000 / 2)
#define AMBA_VIDEO_FPS_3				170666667	//(512000000 / 3)
#define AMBA_VIDEO_FPS_4				128000000	//(512000000 / 4)
#define AMBA_VIDEO_FPS_5				102400000	//(512000000 / 5)
#define AMBA_VIDEO_FPS_6				85333333	//(512000000 / 6)
#define AMBA_VIDEO_FPS_10				51200000	//(512000000 / 10)
#define AMBA_VIDEO_FPS_15				34133333	//(512000000 / 15)
#define AMBA_VIDEO_FPS_20				25600000	//(512000000 / 20)
#define AMBA_VIDEO_FPS_24				21333333	//(512000000 / 24)
#define AMBA_VIDEO_FPS_25				20480000	//(512000000 / 25)
#define AMBA_VIDEO_FPS_30				17066667	//(512000000 / 30)
#define AMBA_VIDEO_FPS_50				10240000	//(512000000 / 50)
#define AMBA_VIDEO_FPS_60				8533333		//(512000000 / 60)
#define AMBA_VIDEO_FPS_120			4266667		//(512000000 / 120)
#define AMBA_VIDEO_FPS_29_97			17083750	//(512000000 / 2997 * 100)
#define AMBA_VIDEO_FPS_59_94			8541875		//(512000000 / 5994 * 100)
#define AMBA_VIDEO_FPS_23_976			21354688	//(512000000 / 23976 * 1000)
#define AMBA_VIDEO_FPS_12_5			40960000	//(512000000 / 25 * 2)
#define AMBA_VIDEO_FPS_6_25			81920000	//(512000000 / 25 * 4)
#define AMBA_VIDEO_FPS_3_125			163840000	//(512000000 / 25 * 8)
#define AMBA_VIDEO_FPS_7_5			68266667	//(512000000 / 15 * 2)
#define AMBA_VIDEO_FPS_3_75			136533333	//(512000000 / 15 * 4)
#else
#define AMBA_VIDEO_FPS(fps)			(fps)
#define AMBA_VIDEO_FRAC_FPS(fps)		(0x10000 + fps)
#define AMBA_VIDEO_FPS_AUTO			AMBA_VIDEO_FPS(0)
#define AMBA_VIDEO_FPS_1			AMBA_VIDEO_FPS(1)
#define AMBA_VIDEO_FPS_2			AMBA_VIDEO_FPS(2)
#define AMBA_VIDEO_FPS_3			AMBA_VIDEO_FPS(3)
#define AMBA_VIDEO_FPS_4			AMBA_VIDEO_FPS(4)
#define AMBA_VIDEO_FPS_5			AMBA_VIDEO_FPS(5)
#define AMBA_VIDEO_FPS_6			AMBA_VIDEO_FPS(6)
#define AMBA_VIDEO_FPS_10			AMBA_VIDEO_FPS(10)
#define AMBA_VIDEO_FPS_15			AMBA_VIDEO_FPS(15)
#define AMBA_VIDEO_FPS_20			AMBA_VIDEO_FPS(20)
#define AMBA_VIDEO_FPS_24			AMBA_VIDEO_FPS(24)
#define AMBA_VIDEO_FPS_25			AMBA_VIDEO_FPS(25)
#define AMBA_VIDEO_FPS_30			AMBA_VIDEO_FPS(30)
#define AMBA_VIDEO_FPS_50			AMBA_VIDEO_FPS(50)
#define AMBA_VIDEO_FPS_60			AMBA_VIDEO_FPS(60)
#define AMBA_VIDEO_FPS_120			AMBA_VIDEO_FPS(120)
#define AMBA_VIDEO_FPS_29_97			AMBA_VIDEO_FRAC_FPS(0)
#define AMBA_VIDEO_FPS_59_94			AMBA_VIDEO_FRAC_FPS(1)
#define AMBA_VIDEO_FPS_23_976			AMBA_VIDEO_FRAC_FPS(2)
#define AMBA_VIDEO_FPS_12_5			AMBA_VIDEO_FRAC_FPS(3)
#define AMBA_VIDEO_FPS_6_25			AMBA_VIDEO_FRAC_FPS(4)
#define AMBA_VIDEO_FPS_3_125			AMBA_VIDEO_FRAC_FPS(5)
#define AMBA_VIDEO_FPS_7_5			AMBA_VIDEO_FRAC_FPS(6)
#define AMBA_VIDEO_FPS_3_75			AMBA_VIDEO_FRAC_FPS(7)
#define AMBA_VOUT_SINK_TYPE_CVBS    (1)
#endif
/* ======================= Standart format defines ========================== */
enum amba_video_mode {
	AMBA_VIDEO_MODE_AUTO		= 0,

	AMBA_VIDEO_MODE_320_240		= 1,		//320x240
	AMBA_VIDEO_MODE_320_288		= 2,		//320x288
	AMBA_VIDEO_MODE_360_240		= 3,		//360x240
	AMBA_VIDEO_MODE_360_288		= 4,		//360x288
	AMBA_VIDEO_MODE_480_240		= 5,		//480x240
	AMBA_VIDEO_MODE_720_240		= 6,		//720x240
	AMBA_VIDEO_MODE_960_240		= 7,		//960x240
	AMBA_VIDEO_MODE_VGA		= 8,		//640x480
	AMBA_VIDEO_MODE_SVGA		= 9,		//800x600
	AMBA_VIDEO_MODE_XGA		= 10,		//1024x768
	AMBA_VIDEO_MODE_SXGA		= 11,		//1280x1024
	AMBA_VIDEO_MODE_UXGA		= 12,		//1600x1200
	AMBA_VIDEO_MODE_QXGA		= 13,		//2048x1536
	AMBA_VIDEO_MODE_WVGA		= 14,		//800x480
	AMBA_VIDEO_MODE_WSVGA		= 15,		//1024x600
	AMBA_VIDEO_MODE_1280_960	= 16,		//1280x960
	AMBA_VIDEO_MODE_WXGA		= 17,		//1280x800
	AMBA_VIDEO_MODE_WSXGA		= 18,		//1440x900
	AMBA_VIDEO_MODE_WSXGAP		= 19,		//1680x1050
	AMBA_VIDEO_MODE_WUXGA		= 20,		//1920x1200

	/* Add new format below and update AMBA_VIDEO_MODE_NUM*/
	AMBA_VIDEO_MODE_480_640		= 21,		//480x640
	AMBA_VIDEO_MODE_SXGAP		= 22,		//1400x1050
	AMBA_VIDEO_MODE_QSXGA		= 23,		//2592x1944

	//Still Standard
	AMBA_VIDEO_MODE_3M_4_3		= 0xFFE0,
	AMBA_VIDEO_MODE_3M_16_9		= 0xFFE1,
	AMBA_VIDEO_MODE_4M_4_3		= 0xFFE2,
	AMBA_VIDEO_MODE_4M_16_9		= 0xFFE3,
	AMBA_VIDEO_MODE_5M_4_3		= 0xFFE4,
	AMBA_VIDEO_MODE_5M_16_9		= 0xFFE5,

	//Video Standard
	AMBA_VIDEO_MODE_480I		= 0xFFF0,
	AMBA_VIDEO_MODE_576I		= 0xFFF1,
	AMBA_VIDEO_MODE_D1_NTSC		= 0xFFF2,	//480p
	AMBA_VIDEO_MODE_D1_PAL		= 0xFFF3,	//576p
	AMBA_VIDEO_MODE_720P		= 0xFFF4,
	AMBA_VIDEO_MODE_720P_PAL	= 0xFFF5,	//720p50
	AMBA_VIDEO_MODE_1080I		= 0xFFF6,
	AMBA_VIDEO_MODE_1080I_PAL	= 0xFFF7,	//1080i50
	AMBA_VIDEO_MODE_1080P		= 0xFFF8,
	AMBA_VIDEO_MODE_1080P_PAL	= 0xFFF9,	//1080i50
	AMBA_VIDEO_MODE_1080I48		= 0xFFFA,	//1080i48
	AMBA_VIDEO_MODE_1080P24		= 0xFFFB,	//1080p24

	AMBA_VIDEO_MODE_TEST		= 0xFFFE,
	AMBA_VIDEO_MODE_MAX	= 0xFFFF,
};

typedef enum {
	HIGH_FRAMERATE = 0,
	LOW_DELAY,
	HIGH_MP,
	TOTAL_ENCODE_MODE,
} ENCODE_MODE;


typedef enum {
	REQ_SET_FORCEIDR = 17,
	REQ_GET_VIDEO_PORT,

	REQ_STREAM_START,
	REQ_STREAM_STOP,

	REQ_CHANGE_BR,
	REQ_CHANGE_FR,
	REQ_CHANGE_BRC,

	REQ_GET_PARAM = 100,
	REQ_SET_PARAM,
	REQ_AAA_CONTROL,
} REQUSET_ID;


typedef enum {
	AAA_START = 0,
	AAA_STOP,
} AAA_CONTROL_ID;


typedef struct {
	u32		id;
	u32		info;
	u32		dataSize;
} Request;/*REQ*/


typedef struct {
	u32		result;
	u32		info;
} Ack;/*ACK*/


typedef enum {
	DATA_MAGIC = 0x20110725,
	DATA_VERSION = 0x00000012,
} VERSION_MAGIC;


typedef struct {
	u32		magic;
	u32		version;
} VERSION;

typedef enum {
	MAP_TO_U32 = 0,
	MAP_TO_U16,
	MAP_TO_U8,
	MAP_TO_S32,
	MAP_TO_S16,
	MAP_TO_S8,
	MAP_TO_DOUBLE,
	MAP_TO_STRING,
} mapping_data_type;


typedef enum {
	NO_LIMIT = 0,
	MIN_MAX_LIMIT,
	MIN_LIMIT,
	MAX_LIMIT,
} mapping_data_constraint;


typedef struct {
	char * 					TokenName;		// name
	void * 					Address;		// address
	mapping_data_type		Type;			// type: 0 - u32, 1 - u16, 2 - u8, 3 - int(s32), 4 - double, 5 - char[]>
	double					Default;			// default value>
	mapping_data_constraint	ParamLimits;	// 0 - no limits, 1 - min and max limits, 2 - only min limit, 3 - only max limit
	double					MinLimit;		// Minimun value
	double					MaxLimit;		// Maximun value
	int						StringLengthLimit;	// Dimension of type char[]
    bool                    Update;
} Mapping;

typedef int (*get_func)(char * section_name, u32 info);
typedef int (*set_func)(char * section_name);

typedef struct {
	char *		name;
	Mapping *	map;
	get_func		get;
	set_func		set;
} Section;


typedef struct {
	u32				enable;
	u32				mode;
	u32				frame_rate;
//	mw_mirror_mode	mirror_mode;
} gk_vin_mode;


typedef struct {
	u32				mode;
	u32				type;
} gk_vout_mode;


typedef struct
{
    u32            streamId;
    u8             encode_type;
    u8             sourceBuf;
    u8             flip_rotate;
    u16            encode_width;
    u16            encode_height;
    u16            xOffset;
    u16            yOffset;
    u32            encode_fps;
}gk_venc_StreamFormat;

typedef struct
{
    u32            streamId;
    u8             gopM;
    u8             gopN;
    u8             idrInterval;
    u8             gopModel;
    u8             profile;
    u8             brcMode;
    u32            cbrAvgBps;
    u32            vbrMinbps;
    u32            vbrMaxbps;
}gk_venc_H264Config;

typedef struct
{
    u32            streamId;
    u8             chromaFormat;
    u8             quality;
}gk_venc_MjpegConfig;

typedef struct
{
    gk_venc_StreamFormat streamFormat;
    gk_venc_H264Config   h264Conf;
    gk_venc_MjpegConfig  mjpegConf;
    u8                 dptz;
}gk_encode_stream;

static inline int get_func_null(char *section_name, u32 info)
{
	return 0;
}

static inline int set_func_null(char *section_name)
{
	return 0;
}



void * webserver_process(void * argv);


#endif
