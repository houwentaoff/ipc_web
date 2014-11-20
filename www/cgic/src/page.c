/*
****************************************************************************
*
** \file      page.c
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

#include "page.h"
#include "include.h"

#define MAX_HEAD            (1024)            /*  */
#define MAXSTRLEN           (256)               /*错误通常由此引起 */
#define SMALLHTML           (6*1024)            /*  */
#define DIV_SELECT_SIZE     (2*1024)            /* tmp, div select size*/
#define DIV_SELECT_SMALLSIZE     (256)            /* tmp, div select size*/
#define STREAM_NUM 4
#define STREAM_NODE_LEN (STREAM_PARAM_TYPE_NUM)
#define ENC_NODE_LEN 25
#define NAME_LEN 20
#define ENC_PARAM_TYPE_NUM 4
#define ENC_PARAM_SPEC_TYPE_NUM 2
#define MAX_OPTION_LEN 16
/*  */



static ParamData live_params[LIVE_PARAMS_NUM];
static ParamData vinvout_params[VINVOUT_PARAMS_NUM];
static ParamData img_params[IMG_PARAMS_NUM];

static ParamData stream_params[STREAM_NODE_LEN];
static ParamData enc_params[ENC_NODE_LEN];


//static int bitrate=0;//码率
int   view_page_get_params();
int   view_create_params()
{

    memset(live_params, 0, sizeof(ParamData)*LIVE_PARAMS_NUM);
	memset(live_params[ENCODE_TYPE].param_name, 0, PARAM_NAME_LEN);
//	strcat(live_params[ENCODE_TYPE].param_name, "encode_type");
	strcat(live_params[ENCODE_TYPE].param_name, "s0_cbr_avg_bps");

	live_params[ENCODE_FPS].value = 30;
	memset(live_params[ENCODE_FPS].param_name, 0, PARAM_NAME_LEN);
//	strcat(live_params[ENCODE_FPS].param_name, "encode_fps");
	strcat(live_params[ENCODE_FPS].param_name, "s1_cbr_avg_bps");

	live_params[ENCODE_WIDTH].value = 0;
	memset(live_params[ENCODE_WIDTH].param_name, 0, PARAM_NAME_LEN);
//	strcat(live_params[ENCODE_WIDTH].param_name, "encode_width");
	strcat(live_params[ENCODE_WIDTH].param_name, "s2_cbr_avg_bps");

	live_params[ENCODE_HEIGHT].value =0;
	memset(live_params[ENCODE_HEIGHT].param_name, 0, PARAM_NAME_LEN);
	strcat(live_params[ENCODE_HEIGHT].param_name, "s3_cbr_avg_bps");
//	strcat(live_params[ENCODE_HEIGHT].param_name, "encode_height");

	live_params[BRC_MODE].value = 0;
	memset(live_params[BRC_MODE].param_name, 0, PARAM_NAME_LEN);
	strcat(live_params[BRC_MODE].param_name, "brc_mode");

	live_params[CBR_AVG_BPS].value = 4000000;
	memset(live_params[CBR_AVG_BPS].param_name, 0, PARAM_NAME_LEN);
	strcat(live_params[CBR_AVG_BPS].param_name, "encode_height");

	live_params[VBR_MIN_BPS].value = 1000000;
	memset(live_params[VBR_MIN_BPS].param_name, 0, PARAM_NAME_LEN);
	strcat(live_params[VBR_MIN_BPS].param_name, "vbr_min_bps");

	live_params[VBR_MAX_BPS].value = 6000000;
	memset(live_params[VBR_MAX_BPS].param_name, 0, PARAM_NAME_LEN);
	strcat(live_params[VBR_MAX_BPS].param_name, "vbr_max_bps");    

    return (GK_CGI_NO_ERROR);
}
int   view_page_get_params()
{
	section_Param section_param;
    int stream_id=0;
	char extroInfo[4]={0};
    char sectionName[10]={0};

    cgiFormInteger("stream", &stream_id, 0);
	sprintf(extroInfo, "%d", stream_id);
    sprintf(sectionName, "STREAM%d", stream_id);
    /*-----------------------------------------------------------------------------
     *  1. init map.
     *  2. send to server.
     *  3. set map.
     *-----------------------------------------------------------------------------*/

    view_create_params();

	live_params[ENCODE_TYPE].value = 0;
//	if (ret < 0) {
//		fprintf(stdout,"1:set params failed");
//	} else {
//		if (ret == 0) {
//			fprintf(stdout,"0:set params succeeded");
//		} else {
//			if (ret == 1) {
				memset(&section_param, 0, sizeof(section_param));
				section_param.sectionName = sectionName;//"LIVE";
				section_param.sectionPort = LIVE;
				section_param.paramData = live_params;
				section_param.extroInfo = extroInfo;//stream_id;
				section_param.paramDataNum = LIVE_PARAMS_NUM;
//				if ((&virtual_PageOps)->get_section_param(currentPage, &section_param) == -1) {
//					return -2;
//				}
                if (Base_get_section_param(&section_param)==-1)
                {
                    FUN_OUT("line[%d]\n", __LINE__);
                    return (GK_CGI_ERROR);
                }

                /*-----------------------------------------------------------------------------
                 *  set section_param.data to web
                 *-----------------------------------------------------------------------------*/
                
    FUN_OUT();
	return (GK_CGI_NO_ERROR);
}
int   _3a_page_get_params()
{
    /*-----------------------------------------------------------------------------
     *  1. init map.
     *  2. send to server.
     *  3. set map.
     *-----------------------------------------------------------------------------*/
    
    return (GK_CGI_NO_ERROR);
}
static int _get_name (char* ret, int streamID, char* name)
{
	sprintf(ret, "s%d_%s", streamID, name);
	return 0;
}

static int enc_create_params ()
{
	int i = 0;
	int j = 0;
	int streamID=0;
	char ret[NAME_LEN] = {0};

    cgiFormInteger("stream", &streamID, 0);

    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "h264_id");
    strcat(stream_params[i].param_name, ret);
    stream_params[i].value = 0;
    i++;

    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "M");
    strcat(stream_params[i].param_name,ret);
    stream_params[i].value = 0;
    i++;

    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "N");
    strcat(stream_params[i].param_name,ret);
    stream_params[i].value = 0;
    i++;

    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "idr_interval");
    strcat(stream_params[i].param_name,ret);
    stream_params[i].value = 0;
    i++;

    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "gop_model");
    strcat(stream_params[i].param_name,ret);
    stream_params[i].value = 0;
    i++;

    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "profile");
    strcat(stream_params[i].param_name,ret);
    stream_params[i].value = 0;
    i++;

    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "brc");
    strcat(stream_params[i].param_name,ret);
    stream_params[i].value = 0;
    i++;

    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "cbr_avg_bps");
    strcat(stream_params[i].param_name,ret);
    stream_params[i].value = 0;
    i++;

    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "vbr_min_bps");
    strcat(stream_params[i].param_name,ret);
    stream_params[i].value = 0;
    i++;

    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "vbr_max_bps");
    strcat(stream_params[i].param_name,ret);
    stream_params[i].value = 0;
    i++;

    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "quality");
    strcat(stream_params[i].param_name,ret);
    stream_params[i].value = 0;
    i++;

    /*-----------------------------------------------------------------------------
     *  一部分是流的一部分是264的
     *-----------------------------------------------------------------------------*/
    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "type");
    strcat(enc_params[j].param_name,ret);
    enc_params[j].value = 0;
    j++;

    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "enc_fps");
    strcat(enc_params[j].param_name,ret);
    enc_params[j].value = 0;
    j++;

    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "dptz");
    strcat(enc_params[j].param_name,ret);
    enc_params[j].value = 0;
    j++;
//Resolution
    memset(ret,0,NAME_LEN);
    _get_name(ret, streamID, "flip_rotate");
    strcat(enc_params[j].param_name,ret);
    enc_params[j].value = 0;
    j++;


    strcat(enc_params[j].param_name,"enc_mode");
    enc_params[j].value = 0;
    j++;

    strcat(enc_params[j].param_name,"s0_width");
    enc_params[j].value = 1280;
    j++;

	strcat(enc_params[j].param_name,"s0_height");
	enc_params[j].value = 720;
	j++;

	strcat(enc_params[j].param_name,"s1_width");
	enc_params[j].value = 720;
	j++;

	strcat(enc_params[j].param_name,"s1_height");
	enc_params[j].value = 480;
	j++;

	strcat(enc_params[j].param_name,"s2_width");
	enc_params[j].value = 352;
	j++;

	strcat(enc_params[j].param_name,"s2_height");
	enc_params[j].value = 240;
	j++;

	strcat(enc_params[j].param_name,"s3_width");
	enc_params[j].value = 352;
	j++;

	strcat(enc_params[j].param_name,"s3_height");
	enc_params[j].value = 240;

	return (0);

}

 int   enc_page_get_params()
{
	int ret = 0;
	char name[NAME_LEN] = {0};
	int i;
	section_Param section_param;
//	unsigned int stream_port[] = {STREAM0, STREAM1, STREAM2, STREAM3};

    /*-----------------------------------------------------------------------------
     *  1. init map.
     *  2. send to server.
     *  3. set map.
     *-----------------------------------------------------------------------------*/
    enc_create_params();
//	if (ret < 0) {
//		fprintf(stdout,"1:set params failed");
//	} else {
//		if (ret == 0) {
//			fprintf(stdout,"0:set params succeeded");
//		} else {
//			if (ret == 1) {
				memset(&section_param, 0, sizeof(section_Param));
				section_param.sectionName  = "ENCODE";
				section_param.sectionPort  = ENCODE;
				section_param.paramData    = enc_params;
				section_param.extroInfo    = "";
				section_param.paramDataNum = ENC_NODE_LEN;
                if (Base_get_section_param(&section_param)==-1)
                {
                    return (GK_CGI_ERROR);
                }
//				if ((&virtual_PageOps)->get_section_param(currentPage, &section_param) == -1) {
//					return -1;
//				}
//				for ( i = 0; i < STREAM_NUM; i++ ) {
					memset(name, 0, NAME_LEN);
					sprintf(name, "STREAM%d", i);
					memset(&section_param, 0, sizeof(section_Param));
					section_param.sectionName = name;
					section_param.sectionPort = ENCODE_PORT;//stream_port[i];
					section_param.paramData = stream_params;
					section_param.extroInfo = "";
					section_param.paramDataNum = STREAM_NODE_LEN;
                if (Base_get_section_param(&section_param)==-1)
                {
                    return (GK_CGI_ERROR);
                }

//				}
//			} else {
//				fprintf(stdout,"1:unexpected error %d",ret);
//			}
//		}
//	}
	return ret;
}
int _3a_page(int (*callback)())
{

#if 0
    char *text=NULL;
    FUN_IN();

    text = (char *)malloc(SMALLHTML);/*6k may much small*/
    if (!text)
    {
        PRT_ERR("text is null\n");
        return (GK_MEM_ERROR);
    }
   div_select_t select_label[3] =
    {
        {
            .id         = "vin_mode",
            .label      = "Resolution :",    
            .options    = NULL,
            .option_num = VIDEO_OPS_NUM,
            .selected   = 2,
            .action     = NULL,
        },
        {
            .id         = "vin_framerate",
            .label      = "Frame Rate (fps) :",    
            .options    = NULL,
            .option_num = FPS_OPS_NUM,
            .selected   = 2,
            .action     = NULL,
        },
    };
    div_select_t select_label_vout[3] =
    {
        {
            .id         = "vout_type",
            .label      = "Type  :",    
            .options    = NULL,
            .option_num = VOUT_TYPE_NUM,
            .selected   = 2,
            .action     = NULL,
        },
        {
            .id         = "vout_mode",
            .label      = "Resolution :",    
            .options    = NULL,
            .option_num = VOUT_VIDEO_OPS_NUM,
            .selected   = 2,
            .action     = NULL,
        },
    };
#endif
    /*-----------------------------------------------------------------------------
     *  preference
     *-----------------------------------------------------------------------------*/
//    select_label_Image Property[]
    return (GK_CGI_NO_ERROR);
    
}
static int _3a_page_create_params()
{
	memset(img_params,0,sizeof(ParamData)*IMG_PARAMS_NUM);
	strcat(img_params[PREFERENCE].param_name, "preference");
	img_params[PREFERENCE].value = 0;

	strcat(img_params[DN_MODE].param_name, "dn_mode");
	img_params[DN_MODE].value = 0;

	strcat(img_params[EXPOSURE_MODE].param_name, "exposure_mode");
	img_params[EXPOSURE_MODE].value = 0;

	strcat(img_params[BACKLIGHT_COMP].param_name, "backlight_comp");
	img_params[BACKLIGHT_COMP].value = 0;

	strcat(img_params[DC_IRIS].param_name, "dc_iris");
	img_params[DC_IRIS].value = 0;

	strcat(img_params[DC_IRIS_DUTY].param_name, "dc_iris_duty");
	img_params[DC_IRIS_DUTY].value = 0;

	strcat(img_params[LOCAL_EXPOSURE].param_name, "local_exposure");
	img_params[LOCAL_EXPOSURE].value = 0;

	strcat(img_params[MCTF_STRENGTH].param_name, "mctf_strength");
	img_params[MCTF_STRENGTH].value = 0;

	strcat(img_params[SHUTTER_MIN].param_name, "shutter_min");
	img_params[SHUTTER_MIN].value = SHUTTER_8000;

	strcat(img_params[SHUTTER_MAX].param_name, "shutter_max");
	img_params[SHUTTER_MAX].value = SHUTTER_30;

	strcat(img_params[MAX_GAIN].param_name, "max_gain");
	img_params[MAX_GAIN].value = GAIN_36db;

	strcat(img_params[AE_TARGET_RATIO].param_name, "ae_target_ratio");
	img_params[AE_TARGET_RATIO].value = 100;

	strcat(img_params[VIN_FPS].param_name, "vin_fps");
	img_params[VIN_FPS].value = FPS_29;

	strcat(img_params[SATURATION].param_name, "saturation");
	img_params[SATURATION].value = 20;

	strcat(img_params[BRIGHTNESS].param_name, "brightness");
	img_params[BRIGHTNESS].value = 40;

	strcat(img_params[HUE].param_name, "hue");
	img_params[HUE].value = 60;

	strcat(img_params[CONTRAST].param_name, "contrast");
	img_params[CONTRAST].value = 80;

	strcat(img_params[SHARPNESS].param_name, "sharpness");
	img_params[SHARPNESS].value = 100;

	strcat(img_params[WBC].param_name, "wbc");
	img_params[WBC].value = 0;

	return (GK_CGI_NO_ERROR);    
    //_add_preference
}
static int vivo_create_params () 
{
	memset(vinvout_params, 0, sizeof(ParamData)*VINVOUT_PARAMS_NUM);
	strcat(vinvout_params[VIN_MODE].param_name, "vin_mode");
	vinvout_params[VIN_MODE].value = 0;

	strcat(vinvout_params[VIN_FRAMERATE].param_name, "vin_framerate");
	vinvout_params[VIN_FRAMERATE].value =0;

	strcat(vinvout_params[VIN_MIRROR].param_name, "vin_mirror");
	vinvout_params[VIN_MIRROR].value = -1;

	strcat(vinvout_params[VIN_BAYER].param_name, "vin_bayer");
	vinvout_params[VIN_BAYER].value = -1;

	strcat(vinvout_params[VOUT_TYPE].param_name, "vout_type");
	vinvout_params[VOUT_TYPE].value = VOUT_CVBS;

	strcat(vinvout_params[VOUT_MODE].param_name, "vout_mode");
	vinvout_params[VOUT_MODE].value = VIDEO_MODE_480I;
	return (GK_CGI_NO_ERROR);
}



int vivo_page_get_params()
{
    	int ret = 0;
//	char name[NAME_LEN] = {0};
//	int i;
//	unsigned int stream_port[] = {STREAM0,STREAM1,STREAM2,STREAM3};

//	ret = (&virtual_PageOps)->process_PostData(currentPage);
	section_Param section_param;
    /*-----------------------------------------------------------------------------
     *  1. init map.
     *  2. send to server.
     *  3. set map.
     *-----------------------------------------------------------------------------*/

    FUN_IN();
    vivo_create_params();
//	if (ret < 0) {
//		fprintf(stdout,"1:set params failed");
//	} else {
//		if (ret == 0) {
//			fprintf(stdout,"0:set params succeeded");
//		} else {
//			if (ret == 1) {
				memset(&section_param, 0, sizeof(section_Param));
				section_param.sectionName = "VINVOUT";
				section_param.sectionPort = VINVOUT;
				section_param.paramData = vinvout_params;
				section_param.extroInfo = "";
				section_param.paramDataNum = VINVOUT_PARAMS_NUM;
//				if ((&virtual_PageOps)->get_section_param(currentPage, &section_param) == -1) {
//					return -2;
//				}
                if (Base_get_section_param(&section_param)==-1)
                {
                    return (-2);
                }

                /*-----------------------------------------------------------------------------
                 *  set section_param.data to web
                 *-----------------------------------------------------------------------------*/
                

//				for ( i = 0; i < STREAM_NUM; i++ ) {
//					memset(name, 0, NAME_LEN);
//					sprintf(name, "STREAM%d", i);
//					memset(&section_param, 0, sizeof(section_Param));
//					section_param.sectionName = name;
//					section_param.sectionPort = stream_port[i];
//					section_param.paramData = stream_params;
//					section_param.extroInfo = "";
//					section_param.paramDataNum = STREAM_NODE_LEN;
//					if ((&virtual_PageOps)->get_section_param(currentPage, &section_param) == -1) {
//						return -1;
//					}
//				}
//			} else {
//				fprintf(stdout,"1:unexpected error %d",ret);
//			}
//		}
//	}
    FUN_OUT();
	return ret;
}

int vivo_page(int (*callback)())
{
    char *text=NULL;
    div_select_t select_label[3] =
    {
        {
            .id         = "vin_mode",
            .label      = "Resolution :",    
            .options    = NULL,
            .option_num = VIDEO_OPS_NUM,
            .selected   = 2,
            .action     = NULL,
        },
        {
            .id         = "vin_framerate",
            .label      = "Frame Rate (fps) :",    
            .options    = NULL,
            .option_num = FPS_OPS_NUM,
            .selected   = 2,
            .action     = NULL,
        },
    };
    div_select_t select_label_vout[3] =
    {
        {
            .id         = "vout_type",
            .label      = "Type  :",    
            .options    = NULL,
            .option_num = VOUT_TYPE_NUM,
            .selected   = 2,
            .action     = NULL,
        },
        {
            .id         = "vout_mode",
            .label      = "Resolution :",    
            .options    = NULL,
            .option_num = VOUT_VIDEO_OPS_NUM,
            .selected   = 2,
            .action     = NULL,
        },
    };
    char *vin_mode[]={"Auto", "1080P", "720P", NULL};
    char *vin_fps[]={"Auto", "5", "6", "10", "13", "15", "25", "29.97", "30", "59.94", "60", NULL};
//    char *vout_mode[]={"480I", "576I", "720P", "1080I", "1080P30", NULL};
    char *vout_type[]={"OFF", "CVBS", "HDMI", NULL};
//    char **divs[]={vin_mode, vin_fps};
    char div_text_1[DIV_SELECT_SIZE]={0};
    char div_text_2[DIV_SELECT_SIZE]={0};
    int i;

    FUN_IN();

    text = (char *)malloc(SMALLHTML);/*6k may much small*/
    if (!text)
    {
        PRT_ERR("text is null\n");
        return (GK_MEM_ERROR);
    }

    /*-----------------------------------------------------------------------------
     *  type 不一样对应的 mode也不一样
     *-----------------------------------------------------------------------------*/
#define __VOUT_TYPE    0
    select_label_vout[__VOUT_TYPE].options = (option_t *)malloc(select_label_vout[__VOUT_TYPE].option_num * sizeof(option_t));
    memset(select_label_vout[__VOUT_TYPE].options, 0, select_label_vout[__VOUT_TYPE].option_num * sizeof(option_t));

    select_label_vout[__VOUT_TYPE].options[VOUT_TYPE_OFF].value = VOUT_OFF;
    select_label_vout[__VOUT_TYPE].options[VOUT_TYPE_OFF].label = vout_type[VOUT_TYPE_OFF];
    select_label_vout[__VOUT_TYPE].options[VOUT_TYPE_CVBS].value = VOUT_CVBS;
    select_label_vout[__VOUT_TYPE].options[VOUT_TYPE_CVBS].label = vout_type[VOUT_TYPE_CVBS];
    select_label_vout[__VOUT_TYPE].options[VOUT_TYPE_HDMI].value = VOUT_HDMI;
    select_label_vout[__VOUT_TYPE].options[VOUT_TYPE_HDMI].label = vout_type[VOUT_TYPE_HDMI];
#define __VOUT_MODE   1

    /*-----------------------------------------------------------------------------
     *  get type then display different mode. 
     *-----------------------------------------------------------------------------*/
    if ((vinvout_params[__VOUT_MODE].value) == VOUT_OFF) {
        select_label_vout[__VOUT_MODE].options = (option_t *)malloc(select_label_vout[__VOUT_MODE].option_num * sizeof(option_t));
        memset(select_label_vout[__VOUT_MODE].options, 0, select_label_vout[__VOUT_MODE].option_num * sizeof(option_t));

        select_label_vout[__VOUT_MODE].options[0].value = VIDEO_MODE_480I;
        select_label_vout[__VOUT_MODE].options[0].label = "480I";
        select_label_vout[__VOUT_MODE].options[1].value = VIDEO_MODE_576I;
        select_label_vout[__VOUT_MODE].options[1].label = "576I";
    }
    else if ((vinvout_params[__VOUT_MODE].value) == VOUT_HDMI) 
    {
        select_label_vout[__VOUT_MODE].options = (option_t *)malloc(select_label_vout[__VOUT_MODE].option_num * sizeof(option_t));
        memset(select_label_vout[__VOUT_MODE].options, 0, select_label_vout[__VOUT_MODE].option_num * sizeof(option_t));

        select_label_vout[__VOUT_MODE].options[VOUT_VIDEO_480P].value = VIDEO_MODE_480P;
        select_label_vout[__VOUT_MODE].options[VOUT_VIDEO_480P].label = "480P";
        select_label_vout[__VOUT_MODE].options[VOUT_VIDEO_576P].value = VIDEO_MODE_576P;
        select_label_vout[__VOUT_MODE].options[VOUT_VIDEO_576P].label = "576P";   
        select_label_vout[__VOUT_MODE].options[VOUT_VIDEO_720P].value = VOUT_VIDEO_720P;
        select_label_vout[__VOUT_MODE].options[VOUT_VIDEO_720P].label = "720P";
        select_label_vout[__VOUT_MODE].options[VOUT_VIDEO_1080I].value = VOUT_VIDEO_1080I;
        select_label_vout[__VOUT_MODE].options[VOUT_VIDEO_1080I].label = "1080I";   
        select_label_vout[__VOUT_MODE].options[VOUT_VIDEO_1080P30].value = VOUT_VIDEO_1080P30;
        select_label_vout[__VOUT_MODE].options[VOUT_VIDEO_1080P30].label = "1080P30";   
    }
    else 
    {
        select_label_vout[__VOUT_MODE].options = (option_t *)malloc(select_label_vout[__VOUT_MODE].option_num * sizeof(option_t));
        memset(select_label_vout[__VOUT_MODE].options, 0, select_label_vout[__VOUT_MODE].option_num * sizeof(option_t));

        select_label_vout[__VOUT_MODE].options[0].value = VIDEO_MODE_480I;
        select_label_vout[__VOUT_MODE].options[0].label = "480I";
        select_label_vout[__VOUT_MODE].options[1].value = VIDEO_MODE_576I;
        select_label_vout[__VOUT_MODE].options[1].label = "576I";
       
    }

#define __RESOLUTION  0
    select_label[__RESOLUTION].options = (option_t *)malloc(select_label[__RESOLUTION].option_num * sizeof(option_t));
    memset(select_label[__RESOLUTION].options, 0, select_label[__RESOLUTION].option_num * sizeof(option_t));

    select_label[__RESOLUTION].options[VIDEO_OPS_AUTO].value = VIDEO_MODE_AUTO;
    select_label[__RESOLUTION].options[VIDEO_OPS_AUTO].label = vin_mode[VIDEO_OPS_AUTO];
    select_label[__RESOLUTION].options[VIDEO_OPS_1080P].value = VIDEO_MODE_1080P;
    select_label[__RESOLUTION].options[VIDEO_OPS_1080P].label = vin_mode[VIDEO_OPS_1080P];
    select_label[__RESOLUTION].options[VIDEO_OPS_720P].value = VIDEO_MODE_720P;
    select_label[__RESOLUTION].options[VIDEO_OPS_720P].label = vin_mode[VIDEO_OPS_720P];

#define __FPS       1

    select_label[__FPS].options = (option_t *)malloc(select_label[__FPS].option_num * sizeof(option_t));
    memset(select_label[__FPS].options, 0, select_label[__FPS].option_num * sizeof(option_t));

    select_label[__FPS].options[FPS_OPS_AUTO].value = FPS_AUTO;
    select_label[__FPS].options[FPS_OPS_AUTO].label = vin_fps[FPS_OPS_AUTO];
    select_label[__FPS].options[FPS_OPS_5].value = FPS_5;
    select_label[__FPS].options[FPS_OPS_5].label = vin_fps[FPS_OPS_5];
    select_label[__FPS].options[FPS_OPS_6].value = FPS_6;
    select_label[__FPS].options[FPS_OPS_6].label = vin_fps[FPS_OPS_6];
    select_label[__FPS].options[FPS_OPS_10].value = FPS_10;
    select_label[__FPS].options[FPS_OPS_10].label = vin_fps[FPS_OPS_10];
    select_label[__FPS].options[FPS_OPS_13].value = FPS_13;
    select_label[__FPS].options[FPS_OPS_13].label = vin_fps[FPS_OPS_13];
    select_label[__FPS].options[FPS_OPS_15].value = FPS_15;
    select_label[__FPS].options[FPS_OPS_15].label = vin_fps[FPS_OPS_15];
    select_label[__FPS].options[FPS_OPS_25].value = FPS_25;
    select_label[__FPS].options[FPS_OPS_25].label = vin_fps[FPS_OPS_25];
    select_label[__FPS].options[FPS_OPS_29].value = FPS_29;
    select_label[__FPS].options[FPS_OPS_29].label = vin_fps[FPS_OPS_29];
    select_label[__FPS].options[FPS_OPS_30].value = FPS_30;
    select_label[__FPS].options[FPS_OPS_30].label = vin_fps[FPS_OPS_30];
    select_label[__FPS].options[FPS_OPS_59].value = FPS_59;
    select_label[__FPS].options[FPS_OPS_59].label = vin_fps[FPS_OPS_59];
    select_label[__FPS].options[FPS_OPS_60].value = FPS_60;
    select_label[__FPS].options[FPS_OPS_60].label = vin_fps[FPS_OPS_60];

#if 0
    j=0;
    while (j<2){
        select_label[j].options = (option_t *)malloc(select_label[j].option_num * sizeof(option_t));
        for (i = 0; i<select_label[j].option_num;i++)
        {
            (select_label[j].options+i)->value = i;
            (select_label[j].options+i)->label = divs[j][i];/*mark*/
            PRT_DBG("label[%s]\n", divs[j][i]);
        }        
        j++;
    }
#endif
    char head_html_buf[MAX_HEAD]={0};

    sprintf(head_html_buf, head_html, "vinvout", "");
    sprintf(text, "%s", head_html_buf);
    strncat(text, nav, strlen(nav));
    //get msg from ctrl server
    //get 分辨率
    //get fps
    callback();
//    vivo_page_get_params();
    select_label[__RESOLUTION].selected = vinvout_params[VIN_MODE].value;
    select_label[__FPS].selected = vinvout_params[VIN_FRAMERATE].value;

    select_label_vout[__VOUT_TYPE].selected = vinvout_params[VOUT_TYPE].value;
    select_label_vout[__VOUT_MODE].selected = vinvout_params[VOUT_MODE].value;

    PRT_DBG("selected[%d]\n", select_label[__FPS].selected);
    i=0;
    while (i< VINVOUT_PARAMS_NUM)
    {
        PRT_DBG("name[%s] param_value[%s] value[%d]\n",
                vinvout_params[i].param_name,
                vinvout_params[i].param_value,
                vinvout_params[i].value);
        i++;
    }

    i =0;
    while (i<2)/*vin*/
    {
        if (i == 3)
        {
            strncat(div_text_1, "<br><br>", strlen("<br><br>"));//????
        }
        create_div_select(div_text_1, &select_label[i]);
        free (select_label[i].options);
        i++;
    }
    i =0;
    while (i<2)/*vout*/
    {
        if (i == 3)
        {
            strncat(div_text_2, "<br><br>", strlen("<br><br>"));//????
        }
        create_div_select(div_text_2, &select_label_vout[i]);
        free (select_label_vout[i].options);
        i++;
    }

    char *pos=text;
    pos += strlen(text);

    sprintf(pos, vivocontent, div_text_1, div_text_2);
    PRT_DBG("divtext[%s]\n", div_text_1);

    fprintf(cgiOut, "%s", text);    
    FUN_OUT();

#undef  __RESOLUTION
#undef  __FPS
#undef  __VOUT_TYPE
#undef  __VOUT_MODE
    return (GK_CGI_NO_ERROR);
}

int   view_page(int (*callback)())
{
    char *text=NULL;
//    div_input_t ChangeCBRAvgBps
//    char *load_activeX = "onload=\"javascript: OnLoadActiveX('192.168.103.47', 0, 1, 0, 1);\"";/*Sean Hou: ip need to replace by js fun */
    FUN_IN();
    /*init display*/
    text = (char *)malloc(SMALLHTML);/*6k may much small*/
    if (!text)
    {
        PRT_ERR("text is null\n");
        return (GK_MEM_ERROR);
    }        
    int stream_Id = -1;

    if (cgiFormIntegerBounded("stream", &stream_Id, STREAM_ID0, STREAM_ID_NUM, 0) != cgiFormSuccess)
    {
        PRT_ERR("index[%d]\n", stream_Id);
        return (GK_CGI_ARGV_ERROR);/*防止绕过密码直接操作,需要通过首页登陆才能进入*/
    }
    PRT_DBG("index[%d]\n", stream_Id);

    char body_onload[MAXSTRLEN]={0};
    sprintf(body_onload, onload, stream_Id, stream_Id);
    sprintf(text, head_html, "liveview", body_onload);
    PRT_DBG("text[%s]\n", text);
    strncat(text, nav, strlen(nav));
//    strncat(text, video_html, strlen(video_html));
    int brate=0;
    char *pos=text;
    pos += strlen(text);
    callback();

    brate = live_params[stream_Id].value/1000;
    PRT_DBG("live_params[CBR_AVG_BPS].value[%d]\n", live_params[stream_Id].value)
    sprintf(pos, liveviewcontent, stream_Id, brate);
    fprintf(cgiOut, "%s", text);

//    PRT_DBG("size[%d]text[%s]\n",strlen(text), text);
   
    free(text);

    FUN_OUT();

    return (GK_CGI_NO_ERROR);
}


int   enc_page(int (*callback)())
{
    int i;
    char *text=NULL;
#if 0
    char *enc_mode[]      = {"Normal mode", "High Mega-pixel mode", "Low delay mode"};
    char *s0_type[]       = {"OFF", "H.264", "MJPEG"};//Type
    char *s0_enc_fps[]    = {"60", "30", "25", "20", "15", "10", "6", "5", "4", "3", "2", "1"};//Encode FPS 
    char *s0_dptz[]       = {"Disable", "Enable"};//PTZ
    char *s0_resolution[] = {"1920 x1080", "1440 x1080", "1280 x1024", "1280 x 960",
        "1280 x 720", "800 x 600", "720 x 576", "720 x 480", "640 x 480",
        "352 x 288", "352 x 240", "320 x 240", "176 x 144", "176 x 120",
        "160 x 120",
    };//Resolution
    char *s0_flip_rotate[] = {"Normal", "Horizontal Flip", "Vertical Flip", "Rotate Clockwise 90", "Rotate 180", "Rotate Clockwise 270"};//Flip & Rotate

    char **labels[]={s0_type, s0_enc_fps, s0_dptz, s0_resolution, s0_flip_rotate};
#endif
    char js_var[]={"s0_type\0""s0_enc_fps\0""s0_dptz\0""s0_resolution\0""s0_flip_rotate\0"};
    char *js_var_name[11]={0};
    char *var = js_var;
    do{
        js_var_name[i] = ++var;
    }while(var = strchr(js_var, '\0'));
    
    /*-----------------------------------------------------------------------------
     *  select
     *-----------------------------------------------------------------------------*/
#if 0
    div_select_t select_labeltop    =
    {
        .label      = "Encode Mode :",    
        .bind.id    = "enc_mode",
        .options    = NULL,
        .option_num = 3,
        .selected   = 2,
        .action     = "setEncodeMode(this.options[this.selectedIndex].value)",
    };//top
#endif
    div_select_t select_title =
    {
            .label      = "stream :",    
            .id    = "stream_id",
            .options    = NULL,
            .option_num = ENC_STREAM_NUM,
            .selected   = 0,
            .action     = "setStreamIndex(this.options[this.selectedIndex].value)",    
    };



    div_select_t select_label[ENC_NUM] =
    {
        {
            .label      = "Type :",    
            .id    = js_var_name[ENC_TYPE],
            .options    = NULL,
            .option_num = TYPE_LEN,
            .selected   = 2,
            .action     = NULL,
        },
        {
            .label      = "Encode FPS :",    
            .id    = js_var_name[ENC_FPS],
            .options    = NULL,
            .option_num = ENC_FPS_LIST_LEN,
            .selected   = 2,
            .action     = NULL,
        },        
        {
            .label      = "DPTZ Type :",    
            .id    = js_var_name[ENC_DPTZ],
            .options    = NULL,
            .option_num = DPTZ_LEN,
            .selected   = 2,
            .action     = NULL,
        },
        {
            .label      = "Resolution :",    
            .id    = js_var_name[ENC_RESOLUTION],
            .options    = NULL,
            .option_num = RES_OPTIONS_LEN,
            .selected   = 2,
            .action     = NULL,
        },            
        {
            .label      = "Flip & Rotate :",    
            .id    = js_var_name[ENC_FLIP_ROTATE],
            .options    = NULL,
            .option_num = FR_OPTIONS_LEN,
            .selected   = 2,
            .action     = NULL,
        },                       
    };
    int streamId=0;
    
    FUN_IN();     
    cgiFormIntegerBounded("streamId", &streamId, STREAM_ID0, STREAM_ID_NUM, STREAM_ID0);
    change_js_var(js_var, streamId/*stream num*/);

    /*init select*/
    text = (char *)malloc(SMALLHTML);
    if (!text)
    {
        PRT_ERR("text is null\n");
        return (GK_MEM_ERROR);
    }    
    select_title.options = (option_t *)malloc(select_title.option_num * sizeof(option_t));
    select_title.options[ENC_STREAM1].value = ENC_STREAM1;
    select_title.options[ENC_STREAM1].label = "0";
    select_title.options[ENC_STREAM2].value = ENC_STREAM2;
    select_title.options[ENC_STREAM2].label = "1";
    select_title.options[ENC_STREAM3].value = ENC_STREAM3;
    select_title.options[ENC_STREAM3].label = "2";
    select_title.options[ENC_STREAM4].value = ENC_STREAM4;
    select_title.options[ENC_STREAM4].label = "3";

    select_label[ENC_TYPE].options = (option_t *)malloc(select_label[ENC_TYPE].option_num * sizeof(option_t));
    select_label[ENC_TYPE].options[OFF].value = OFF;
    select_label[ENC_TYPE].options[OFF].label = "OFF";
    select_label[ENC_TYPE].options[H_264].value = H_264;
    select_label[ENC_TYPE].options[H_264].label = "H.264";
    select_label[ENC_TYPE].options[MJPEG].value = MJPEG;
    select_label[ENC_TYPE].options[MJPEG].label = "MJPEG";

    select_label[ENC_FPS].options = (option_t *)malloc(select_label[ENC_FPS].option_num * sizeof(option_t));
    select_label[ENC_FPS].options[ENC_FPS_60].value = FPS_60;
    select_label[ENC_FPS].options[ENC_FPS_60].label = "60";
    select_label[ENC_FPS].options[ENC_FPS_30].value = FPS_30;
    select_label[ENC_FPS].options[ENC_FPS_30].label = "30";
    select_label[ENC_FPS].options[ENC_FPS_25].value = FPS_25;
    select_label[ENC_FPS].options[ENC_FPS_25].label = "25";
    select_label[ENC_FPS].options[ENC_FPS_20].value = FPS_20;
    select_label[ENC_FPS].options[ENC_FPS_20].label = "20";
    select_label[ENC_FPS].options[ENC_FPS_15].value = FPS_15;
    select_label[ENC_FPS].options[ENC_FPS_15].label = "15";
    select_label[ENC_FPS].options[ENC_FPS_10].value = FPS_10;
    select_label[ENC_FPS].options[ENC_FPS_10].label = "10";
    select_label[ENC_FPS].options[ENC_FPS_6].value = FPS_6;
    select_label[ENC_FPS].options[ENC_FPS_6].label = "6";
    select_label[ENC_FPS].options[ENC_FPS_5].value = FPS_5;
    select_label[ENC_FPS].options[ENC_FPS_5].label = "5";
    select_label[ENC_FPS].options[ENC_FPS_4].value = FPS_4;
    select_label[ENC_FPS].options[ENC_FPS_4].label = "4";
    select_label[ENC_FPS].options[ENC_FPS_3].value = FPS_3;
    select_label[ENC_FPS].options[ENC_FPS_3].label = "3";
    select_label[ENC_FPS].options[ENC_FPS_2].value = FPS_2;
    select_label[ENC_FPS].options[ENC_FPS_2].label = "5";
    select_label[ENC_FPS].options[ENC_FPS_1].value = FPS_1;
    select_label[ENC_FPS].options[ENC_FPS_1].label = "1";

   select_label[ENC_DPTZ].options = (option_t *)malloc(select_label[ENC_DPTZ].option_num * sizeof(option_t));
   select_label[ENC_DPTZ].options[DPTZ_DISABLE].value = DPTZ_DISABLE;
   select_label[ENC_DPTZ].options[DPTZ_DISABLE].label = "Disable";
   select_label[ENC_DPTZ].options[DPTZ_ENABLE].value = DPTZ_ENABLE;
   select_label[ENC_DPTZ].options[DPTZ_ENABLE].label = "Enable";

   select_label[ENC_RESOLUTION].options = (option_t *)malloc(select_label[ENC_RESOLUTION].option_num * sizeof(option_t));
   select_label[ENC_RESOLUTION].options[RES_OPS_1920x1080].value = RES_1920x1080;
   select_label[ENC_RESOLUTION].options[RES_OPS_1920x1080].label = "1920 x 1080";
   select_label[ENC_RESOLUTION].options[RES_OPS_1440x1080].value = RES_1440x1080;
   select_label[ENC_RESOLUTION].options[RES_OPS_1440x1080].label = "1440 x 1080";
   select_label[ENC_RESOLUTION].options[RES_OPS_1280x1024].value = RES_1280x1024;
   select_label[ENC_RESOLUTION].options[RES_OPS_1280x1024].label = "1280 x 1024";
   select_label[ENC_RESOLUTION].options[RES_OPS_1280x960].value = RES_1280x960;
   select_label[ENC_RESOLUTION].options[RES_OPS_1280x960].label = "1280 x 960";   
   select_label[ENC_RESOLUTION].options[RES_OPS_1920x1080].value = RES_1280x720;
   select_label[ENC_RESOLUTION].options[RES_OPS_1920x1080].label = "1280 x 720";
   select_label[ENC_RESOLUTION].options[RES_OPS_800x600].value = RES_800x600;
   select_label[ENC_RESOLUTION].options[RES_OPS_800x600].label = "800 x 600";
   select_label[ENC_RESOLUTION].options[RES_OPS_720x576].value = RES_720x576;
   select_label[ENC_RESOLUTION].options[RES_OPS_720x576].label = "720 x 576";
   select_label[ENC_RESOLUTION].options[RES_OPS_720x480].value = RES_720x480;
   select_label[ENC_RESOLUTION].options[RES_OPS_720x480].label = "720 x 480";
   select_label[ENC_RESOLUTION].options[RES_OPS_640x480].value = RES_640x480;
   select_label[ENC_RESOLUTION].options[RES_OPS_640x480].label = "640 x 480";
   select_label[ENC_RESOLUTION].options[RES_OPS_352x288].value = RES_352x288;
   select_label[ENC_RESOLUTION].options[RES_OPS_352x288].label = "352 x 288";
   select_label[ENC_RESOLUTION].options[RES_OPS_352x240].value = RES_352x240;
   select_label[ENC_RESOLUTION].options[RES_OPS_352x240].label = "352 x 240";
   select_label[ENC_RESOLUTION].options[RES_OPS_320x240].value = RES_320x240;
   select_label[ENC_RESOLUTION].options[RES_OPS_320x240].label = "320 x 240";

   
   select_label[ENC_RESOLUTION].options[RES_OPS_176x144].label = "176 x 144";
   select_label[ENC_RESOLUTION].options[RES_OPS_176x144].value = RES_176x144;

   select_label[ENC_RESOLUTION].options[RES_OPS_176x120].label = "176 x 120";
   select_label[ENC_RESOLUTION].options[RES_OPS_176x120].value = RES_176x120;

   select_label[ENC_RESOLUTION].options[RES_OPS_160x120].label = "160 x 120";
   select_label[ENC_RESOLUTION].options[RES_OPS_160x120].value = RES_160x120;
//Flip & Rotate
   select_label[ENC_FLIP_ROTATE].options = (option_t *)malloc(select_label[ENC_FLIP_ROTATE].option_num * sizeof(option_t));
   select_label[ENC_FLIP_ROTATE].options[FR_OPS_NORMAL].value = FR_NORMAL;
   select_label[ENC_FLIP_ROTATE].options[FR_OPS_NORMAL].label = "Normal";
   select_label[ENC_FLIP_ROTATE].options[FR_OPS_HFLIP].value = FR_HFLIP;
   select_label[ENC_FLIP_ROTATE].options[FR_OPS_HFLIP].label = "Horizontal Flip";
   select_label[ENC_FLIP_ROTATE].options[FR_OPS_VFLIP].value = FR_VFLIP;
   select_label[ENC_FLIP_ROTATE].options[FR_OPS_VFLIP].label = "Vertical Flip";
   select_label[ENC_FLIP_ROTATE].options[FR_OPS_ROTATE_90].value = FR_ROTATE_90;
   select_label[ENC_FLIP_ROTATE].options[FR_OPS_ROTATE_90].label = "Rotate Clockwise 90";
   select_label[ENC_FLIP_ROTATE].options[FR_OPS_ROTATE_180].value = FR_ROTATE_180;
   select_label[ENC_FLIP_ROTATE].options[FR_OPS_ROTATE_180].label = "Rotate 180";
   select_label[ENC_FLIP_ROTATE].options[FR_OPS_ROTATE_270].value = FR_ROTATE_270;
   select_label[ENC_FLIP_ROTATE].options[FR_OPS_ROTATE_270].label = "Rotate Clockwise 270";

    char head_html_buf[MAX_HEAD]={0};

    sprintf(head_html_buf, head_html, "enc", "");
    sprintf(text, "%s", head_html_buf);
    strncat(text, nav, strlen(nav));
    /*stream 0*/
       //get msg from ctrl server
    //get 分辨率
    //get fps
    callback();
#if 1
    int idx = ENC_TYPE;
    for (idx = ENC_TYPE; idx<ENC_NUM; idx++)
    {
        select_label[idx].selected =  enc_params[idx].value;
    }
#else 
    select_label[ENC_TYPE].selected =  enc_params[ENC_TYPE].value;
    select_label[ENC_FPS].selected = enc_params[ENC_FPS].value;
    select_label[ENC_DPTZ].selected = enc_params[ENC_DPTZ].value;
    select_label[ENC_RESOLUTION].selected = enc_params[ENC_RESOLUTION].value;
    select_label[ENC_FLIP_ROTATE].selected = enc_params[ENC_FLIP_ROTATE].value;
#endif
    char div_text[DIV_SELECT_SIZE]={0};
    char div_text_1[DIV_SELECT_SMALLSIZE]={0};
    for (i=0;i<ENC_NUM;i++)//enc
    {
        create_div_select(div_text, &select_label[i]);
        free (select_label[i].options);
    }
    create_div_select(div_text_1, &select_title);
    free (select_title[i].options);
    char *pos=text;
    pos += strlen(text);
    sprintf(pos, enccontent, div_text_1, div_text);
    PRT_DBG("div_text[%s]div_text_1[%s]\n", div_text, div_text_1);
    fprintf(cgiOut, "%s", text);
    PRT_DBG("size[%d]text[%s]\n",strlen(text), text);
    free(text);

    FUN_OUT();

    return (GK_CGI_NO_ERROR);
}
int   pm_page()
{

    return (GK_CGI_NO_ERROR);
}
int   osd_page()
{

    return (GK_CGI_NO_ERROR);
}
int   sys_page()
{

    return (GK_CGI_NO_ERROR);
}

