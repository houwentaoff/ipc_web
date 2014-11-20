/*
 ****************************************************************************
 *
 ** \file      page.h
 **
 ** \version   $Id: page.h 2411 2014-11-18 13:43:30Z houwentao $
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

#ifndef  __PAGE_H__
#define  __PAGE_H__

#define LABEL_OPTION_LEN 32
#define STREAM_ID_OFFSET			(28)            /*  */

typedef struct {
	unsigned int value;/* usually increase */
//	char option[LABEL_OPTION_LEN];
    char *label;//[LABEL_OPTION_LEN];
}option_t;

typedef struct {
	char *label;/*label name */
    char *name;/*label 'for name', select id name*/
    union {
        char *name;
        char *id;
    }bind;
	option_t *options;
	int option_num;
	unsigned int selected;/* selected value*/
	char *action;
}select_Label_t;

typedef struct {
    char* id;
//	char* label;
//	char* name;
	int value;
	int maxlen;
	char* ro;
}div_input_t;

typedef struct {
	char *label;/*label value */
    char *id;/*div id*/
    union {
        char *name;
        char *id;
    }bind;
	option_t *options;
	int option_num;
	unsigned int selected;/* selected value*/
	char *action;
}div_select_t;

typedef enum input_type{
    BUTTON=0,
}input_type_e;

typedef struct {
	char *id;
    input_type_e type;
	char *value;
	char *action;
    /*no use*/
	char *name;
	char *exprop;
}input_t;

typedef enum {
	VIDEO_OPS_AUTO,
	VIDEO_OPS_1080P,
	VIDEO_OPS_720P,
	VIDEO_OPS_NUM
}VIDEO_MODE_OPTIONS;

typedef enum {
	FPS_OPS_AUTO,
	FPS_OPS_5,
	FPS_OPS_6,
	FPS_OPS_10,
	FPS_OPS_13,
	FPS_OPS_15,
	FPS_OPS_25,
	FPS_OPS_29,
	FPS_OPS_30,
	FPS_OPS_59,
	FPS_OPS_60,
	FPS_OPS_NUM
}FPS_OPTIONS;

typedef enum {
	VOUT_TYPE_OFF,
	VOUT_TYPE_CVBS,
	VOUT_TYPE_HDMI,
	VOUT_TYPE_NUM
}VOUT_TYPE_OPTIONS;

typedef enum {
	VOUT_VIDEO_480P,
	VOUT_VIDEO_576P,
	VOUT_VIDEO_720P,
	VOUT_VIDEO_1080I,
	VOUT_VIDEO_1080P30,
	VOUT_VIDEO_OPS_NUM
}VOUT_VIDEO_OPTIONS;

//ENC
typedef enum {
	H264_ID,
	M,
	N,
	IDR_INTERVAL,
	GOP_MODEL,
	PROFILE,
	BRC,
	CBR_AVG_BPS,
	VBR_MIN_BPS,
	VBR_MAX_BPS,
	QUALITY,
	STREAM_PARAM_TYPE_NUM
}STREAM_PARAMS_TYPE;

typedef enum {
	ENC_TYPE,
	ENC_FPS,
	ENC_DPTZ,
    ENC_RESOLUTION,
	ENC_FLIP_ROTATE,
	ENC_WIDTH,
	ENC_HEIGHT,
	ENC_NUM,
}ENC_PARAMS_TYPE;

typedef enum{
    ENC_STREAM1,
    ENC_STREAM2,
    ENC_STREAM3,
    ENC_STREAM4,
    ENC_STREAM_NUM,
}ENC_TYPE_STREAM;

typedef enum {
	OFF,
	H_264,
	MJPEG,
	TYPE_LEN
}TYPE_OPTIONS;

typedef enum {
	ENC_FPS_60,
	ENC_FPS_30,
	ENC_FPS_25,
	ENC_FPS_20,
	ENC_FPS_15,
	ENC_FPS_10,
	ENC_FPS_6,
	ENC_FPS_5,
	ENC_FPS_4,
	ENC_FPS_3,
	ENC_FPS_2,
	ENC_FPS_1,
	ENC_FPS_LIST_LEN
}ENC_FPS_LIST;

typedef enum {
	DPTZ_DISABLE,
	DPTZ_ENABLE,
	DPTZ_LEN
}DPTZ_OPTIONS;

typedef enum {
	RES_OPS_1920x1080,
	RES_OPS_1440x1080,
	RES_OPS_1280x1024,
	RES_OPS_1280x960,
	RES_OPS_1280x720,
	RES_OPS_800x600,
	RES_OPS_720x576,
	RES_OPS_720x480,
	RES_OPS_640x480,
	RES_OPS_352x288,
	RES_OPS_352x240,
	RES_OPS_320x240,
	RES_OPS_176x144,
	RES_OPS_176x120,
	RES_OPS_160x120,
	RES_OPTIONS_LEN
}RES_OPTIONS;

typedef enum {
	FR_OPS_NORMAL,
	FR_OPS_HFLIP,
	FR_OPS_VFLIP,
	FR_OPS_ROTATE_90,
	FR_OPS_ROTATE_180,
	FR_OPS_ROTATE_270,
	FR_OPTIONS_LEN
}FR_OPTIONS;
//liveview
typedef enum
{
	LIVE_ENCODE_TYPE = 0,
	LIVE_ENCODE_FPS,
	LIVE_ENCODE_WIDTH,
	LIVE_ENCODE_HEIGHT,
	LIVE_BRC_MODE,
	LIVE_CBR_AVG_BPS,
	LIVE_VBR_MIN_BPS,
	LIVE_VBR_MAX_BPS,
	LIVE_PARAMS_NUM
}LIVE_PARAMS;

extern int   view_page(int (*callback)());
extern int   view_page_get_params();
extern int   enc_page(int (*callback)());
extern int   enc_page_get_params();
extern int   pm_page();
extern int   _3a_page(int (*callback)());
extern int   _3a_page_get_params();
extern int   osd_page();
extern int   sys_page();
extern int   vivo_page(int (*callback)());
extern int   vivo_page_get_params();


#endif//__PAGE_H__

