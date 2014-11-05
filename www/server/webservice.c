/*
****************************************************************************
*
** \file      webservice.c
**
** \version   $Id: webservice.c 2313 2014-11-05 09:49:02Z houwentao $
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

#include "webservice.h"
#include "video.h"

#define INOUT_DEBUG           1  	/*1 is open , o is close */
#define APP_DEBUG             1     /*1 is open , 0 is close */
#define APP_ERR               1     /*1 is open , 0 is close */

#define	MAX_ITEMS_TO_PARSE		(3*1000)

#if INOUT_DEBUG
#define FUN_IN(fmt, args...)            printf("===> %s()"fmt"\n", __func__, ##args)/*  */
#define FUN_OUT(fmt, args...)           printf("<=== %s()"fmt"\n", __func__, ##args) /*  */
#else
#define FUN_IN(fmt, args...)
#define FUN_OUT(fmt, args...)
#endif

#if APP_DEBUG
#define PRT_DBG(fmt, args...)         printf("%s():line[%d]"fmt"\n", __func__, __LINE__, ##args)/*  */
#else
#define PRT_DBG(fmt, args...)
#endif

#if APP_ERR
#define PRT_ERR(fmt, args...)                                                               \
do                                                                                          \
{                                                                                           \
    printf("\033[5;41;32m [ERROR] ---> %s ():line[%d]:\033[0m\n", __func__, __LINE__);      \
    printf(" "fmt, ##args);                                                                 \
}while(0)    /* */
#else
#define PRT_ERR(fmt, args...)
#endif

static u8						g_buffer[BUFFER_SIZE];
static gk_vin_mode			vin_map;
static gk_vout_mode			vout_map;
static int sockfd = -1;
static int sockfd2 = -1;

extern CAMCONTROL_Encode_Cmd g_stEncodeInfo[4];

static int set_vinvout_param(char * section_name);
static int get_vinvout_param(char * section_name, u32 info);

static Mapping VinVoutMap[] = {
	{"vin_enable",			&vin_map.enable,				MAP_TO_U32,	0.0,		0,		0.0,		0.0,	},
	{"vin_mode",			&vin_map.mode,				MAP_TO_U32,	AMBA_VIDEO_MODE_AUTO,		0,		0.0,		0.0,	},
	{"vin_framerate",		&vin_map.frame_rate,			MAP_TO_U32,	AMBA_VIDEO_FPS_29_97,		0,		0.0,		0.0,	},
//	{"vin_mirror",			&vin_map.mirror_mode.mirror_pattern,	MAP_TO_U32,	0.0,		0,		0.0,		0.0,	},
//	{"vin_bayer",			&vin_map.mirror_mode.bayer_pattern,	MAP_TO_U32,	0.0,		0,		0.0,		0.0,	},

	{"vout_type",			&vout_map.type,			MAP_TO_U32,	AMBA_VOUT_SINK_TYPE_CVBS,		0,		0.0,		0.0,	},
	{"vout_mode",			&vout_map.mode,			MAP_TO_U32,	AMBA_VIDEO_MODE_480I,		0,		0.0,		0.0,	},

	{NULL,			NULL,						-1,	0.0,					0,	0.0,	0.0,		},
};
static Section Params[] = {
	{"VINVOUT",		VinVoutMap,		get_vinvout_param,		set_vinvout_param},
//	{"ENCODE",		EncodeMap,		get_func_null,		set_encode_param},
//	{"STREAM0",		Stream0,		get_func_null,		set_stream_param},
//	{"STREAM1",		Stream1,		get_func_null,		set_stream_param},
//	{"STREAM2",		Stream2,		get_func_null,		set_stream_param},
//	{"STREAM3",		Stream3,		get_func_null,		set_stream_param},
//	{"PRIMASK",		PMMap,			get_func_null,		set_pm_param},
//	{"OSD",			OSDMap,			get_func_null,		set_osd_param},
//	{"DPTZ",		DPTZMap,		get_dptz_param,		set_dptz_param},
//	{"LIVE",		LIVEMap,		get_live_param,		set_func_null},
	{NULL,			NULL,			NULL,				NULL}
};

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
					PRT_ERR("Error in input parameter %s. Please "
						"check configuration file.\n"
						"Value should be in [%d, %d] range.\n"
						"Use default value [%d]\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].MaxLimit, (int)Map[i].Default);
					*(u32 *)(Map[i].Address) = (u32)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MIN_LIMIT) {
				if (*(u32 *)(Map[i].Address) < (u32)(int)(Map[i].MinLimit)) {
					PRT_ERR("Error in input parameter %s. Please "
						"check configuration file.\n"
						"Value should be larger than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].Default);
					*(u32 *)(Map[i].Address) = (u32)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MAX_LIMIT) {
				if (*(u32 *)(Map[i].Address) > (u32)(int)(Map[i].MaxLimit)) {
					PRT_ERR("Error in input parameter %s. Please "
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
					PRT_ERR("Error in input parameter %s. Please "
						"check configuration file.\n"
						"Value should be in [%d, %d] range.\n"
						"Use default value [%d]\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].MaxLimit, (int)Map[i].Default);
					*(u16 *)(Map[i].Address) = (u16)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MIN_LIMIT) {
				if (*(u16 *)(Map[i].Address) < (u16)(int)(Map[i].MinLimit)) {
					PRT_ERR("Error in input parameter %s. Please "
						"check configuration file.\n"
						"Value should be larger than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].Default);
					*(u16 *)(Map[i].Address) = (u16)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MAX_LIMIT) {
				if (*(u16 *)(Map[i].Address) > (u16)(int)(Map[i].MaxLimit)) {
					PRT_ERR("Error in input parameter %s. Please "
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
					PRT_ERR("Error in input parameter %s. Please "
						"check configuration file.\n"
						"Value should be in [%d, %d] range.\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].MaxLimit, (int)Map[i].Default);
					*(u8 *)(Map[i].Address) = (u8)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MIN_LIMIT) {
				if (*(u8 *)(Map[i].Address) < (u8)(int)(Map[i].MinLimit)) {
					PRT_ERR("Error in input parameter %s. Please "
						"check configuration file.\n"
						"Value should be larger than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].Default);
					*(u8 *)(Map[i].Address) = (u8)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MAX_LIMIT) {
				if (*(u8 *)(Map[i].Address) > (u8)(int)(Map[i].MaxLimit)) {
					PRT_ERR("Error in input parameter %s. Please "
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
					PRT_ERR("Error in input parameter %s. Please "
						"check configuration file.\n"
						"Value should be in [%d, %d] range.\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].MaxLimit, (int)Map[i].Default);
					*(int *)(Map[i].Address) = (int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MIN_LIMIT) {
				if (*(int *)(Map[i].Address) < (int)(Map[i].MinLimit)) {
					PRT_ERR("Error in input parameter %s. Please "
						"check configuration file.\n"
						"Value should be larger than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].Default);
					*(int *)(Map[i].Address) = (int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MAX_LIMIT) {
				if (*(int *)(Map[i].Address) > (int)(Map[i].MaxLimit)) {
					PRT_ERR("Error in input parameter %s. Please "
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
					PRT_ERR("Error in input parameter %s. Please "
						"check configuration file.\n"
						"Value should be in [%d, %d] range.\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].MaxLimit, (int)Map[i].Default);
					*(s16 *)(Map[i].Address) = (s16)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MIN_LIMIT) {
				if (*(s16 *)(Map[i].Address) < (s16)(int)(Map[i].MinLimit)) {
					PRT_ERR("Error in input parameter %s. Please "
						"check configuration file.\n"
						"Value should be larger than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].Default);
					*(s16 *)(Map[i].Address) = (s16)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MAX_LIMIT) {
				if (*(s16 *)(Map[i].Address) > (s16)(int)(Map[i].MaxLimit)) {
					PRT_ERR("Error in input parameter %s. Please "
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
					PRT_ERR("Error in input parameter %s. Please "
						"check configuration file.\n"
						"Value should be in [%d, %d] range.\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].MaxLimit, (int)Map[i].Default);
					*(s8 *)(Map[i].Address) = (s8)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MIN_LIMIT) {
				if (*(s8 *)(Map[i].Address) < (s8)(int)(Map[i].MinLimit)) {
					PRT_ERR("Error in input parameter %s. Please "
						"check configuration file.\n"
						"Value should be larger than [%d].\n"
						"Use default value [%d].\n",
						Map[i].TokenName, (int)Map[i].MinLimit,
						(int)Map[i].Default);
					*(s8 *)(Map[i].Address) = (s8)(int)(Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MAX_LIMIT) {
				if (*(s8 *)(Map[i].Address) > (s8)(int)(Map[i].MaxLimit)) {
					PRT_ERR("Error in input parameter %s. Please "
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
					PRT_ERR("Error in input parameter %s. Please "
						"check configuration file.\n"
						"Value should be in [%.2lf, %.2lf] range.\n"
						"Use default value [%.2lf].\n",
						Map[i].TokenName, Map[i].MinLimit,
						Map[i].MaxLimit, Map[i].Default);
					*(double *)(Map[i].Address) = (Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MIN_LIMIT) {
				if (*(double *)(Map[i].Address) < (Map[i].MinLimit)) {
					PRT_ERR("Error in input parameter %s. Please "
						"check configuration file.\n"
						"Value should be larger than [%.2lf].\n"
						"Use default value [%.2lf].\n",
						Map[i].TokenName, Map[i].MinLimit,
						Map[i].Default);
					*(double *)(Map[i].Address) = (Map[i].Default);
				}
			} else if (Map[i].ParamLimits == MAX_LIMIT) {
				if (*(double *)(Map[i].Address) > (Map[i].MaxLimit)) {
					PRT_ERR("Error in input parameter %s. Please "
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
			* (u32 *) (Map[MapIndex].Address) = U32Content;
			sprintf(msg, "%s.", msg);
			break;

		case MAP_TO_U16:						// Numerical, unsigned integer (u16)
			if (1 != sscanf(items[i+2], "%u", &U32Content)) {
				PRT_ERR("Parsing error: Expected numerical value for Parameter [%s], found [%s].\n",
					items[i], items[i+2]);
				exit(0);
			}
			* (u16 *) (Map[MapIndex].Address) = U32Content;
			sprintf(msg, "%s.", msg);
			break;

		case MAP_TO_U8:							// Numerical, unsigned integer (u8)
			if (1 != sscanf(items[i+2], "%u", &U32Content)) {
				PRT_ERR("Parsing error: Expected numerical value for Parameter [%s], found [%s].\n",
					items[i], items[i+2]);
				exit(0);
			}
			* (u8 *) (Map[MapIndex].Address) = U32Content;
			sprintf(msg, "%s.", msg);
			break;

		case MAP_TO_S32:						// Numerical, signed integer
			if (1 != sscanf(items[i+2], "%d", &IntContent)) {
				PRT_ERR("Parsing error: Expected numerical value for Parameter [%s], found [%s].\n",
					items[i], items[i+2]);
				exit(0);
			}
			* (int *) (Map[MapIndex].Address) = IntContent;
			sprintf(msg, "%s.", msg);
			break;

		case MAP_TO_S16:						// Numerical, signed short
			if (1 != sscanf(items[i+2], "%d", &IntContent)) {
				PRT_ERR("Parsing error: Expected numerical value for Parameter [%s], found [%s].\n",
					items[i], items[i+2]);
				exit(0);
			}
			* (s16 *) (Map[MapIndex].Address) = IntContent;
			sprintf(msg, "%s.", msg);
			break;

		case MAP_TO_S8:						// Numerical, signed char
			if (1 != sscanf(items[i+2], "%d", &IntContent)) {
				PRT_ERR("Parsing error: Expected numerical value for Parameter [%s], found [%s].\n",
					items[i], items[i+2]);
				exit(0);
			}
			* (s8 *) (Map[MapIndex].Address) = IntContent;
			sprintf(msg, "%s.", msg);
			break;

		case MAP_TO_DOUBLE:					// Numerical, double
			if (1 != sscanf(items[i+2], "%lf", &DoubleContent)) {
				PRT_ERR("Parsing error: Expected numerical value for Parameter [%s], found [%s].\n",
					items[i], items[i+2]);
				exit(0);
			}
			* (double *) (Map[MapIndex].Address) = DoubleContent;
			sprintf(msg, "%s.", msg);
			break;

		case MAP_TO_STRING:						// String
			memset((char *) Map[MapIndex].Address, 0, Map[MapIndex].StringLengthLimit);
			if (NULL != items[i+2]) {
				strncpy((char *) Map[MapIndex].Address, items[i+2], Map[MapIndex].StringLengthLimit - 1);
			} else {
				memset((char *) Map[MapIndex].Address, 0, Map[MapIndex].StringLengthLimit);
			}
			sprintf(msg, "%s.", msg);
			break;

		default:
			PRT_ERR("== parse_content == Unknown value type in the map definition!\n");
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
			PRT_ERR("== load_default_params == Unknown value type in the map definition!\n");
			exit(0);
		}
		++i;
	}

	return 0;
}

static int config_params(Mapping *Map, const char * content)
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
			PRT_ERR("== Output Parameters == Unknown value type in the map definition !!!\n");
			break;
		}
		++i;
	}
	return 0;
}

int send_text(u8 *pBuffer, u32 size)
{
    FUN_IN("content[%s]\n", pBuffer);
	int retv = send(sockfd2, pBuffer, size, MSG_NOSIGNAL);
	if ((u32)retv != size) {
		PRT_ERR("send() returns %d.", retv);
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
    FUN_OUT("content[%s]\n", pBuffer);
	return retv;
}

static int get_vinvout_param(char * section_name, u32 info)
{
	int streamID = 0, retv = 0;
    FUN_IN();
	PRT_DBG("Section [%s] setting:\n", section_name);
    vin_map.frame_rate = g_stEncodeInfo[streamID].framerate;
    FUN_OUT("frame_rate[%d]\n", vin_map.frame_rate);
	return retv;
}

static int set_vinvout_param(char * section_name)
{
	int streamID = 0, retv = 0;
	FUN_IN("Section [%s] setting:\n", section_name);
    g_stEncodeInfo[streamID].framerate = vin_map.frame_rate;
    FUN_OUT();
#if 0 //need to replace
	if (mw_disable_stream(STREAMS_MASK) < 0) {
		PRT_ERR("Cannot stop encoding streams!");
		return -1;
	}
	update_encode_info();

#ifndef CONFIG_AMBARELLA_IMAGE_SERVER_DAEMON
	// Disable preview and stop AAA when main buffer is changed
	if (send_image_signal(AAA_STOP) < 0) {
		PRT_ERR("REQ_AAA_STOP error");
		return -1;
	}
#endif
	// Disable preview
	vin_map.enable = 0;
	if (mw_enable_preview(vin_map.enable) < 0) {
		PRT_ERR("Disable preview");
		return -1;
	}
	// Set Vout Mode
	if (mw_set_vout_mode(&vout_map) < 0) {
		// Set parameter error, restore map parameter
		PRT_ERR("mw_set_vout_mode");
		memcpy(&vout_map, &g_mw_vout, sizeof(g_mw_vout));
		retv = -1;
	} else {
		memcpy(&g_mw_vout, &vout_map, sizeof(g_mw_vout));
	}
	if (config_encoder_params(&streamID) < 0) {
		PRT_ERR("config_encoder_params");
		return -1;
	}
	vin_map.enable = 1;
	if (mw_enable_preview(vin_map.enable) < 0) {
		PRT_ERR("Enable preview");
		return -1;
	}
#ifndef CONFIG_AMBARELLA_IMAGE_SERVER_DAEMON
	if (send_image_signal(AAA_START) < 0) {
		PRT_ERR("REQ_AAA_START error");
		return -1;
	}
#endif
	if (mw_enable_stream(streamID) < 0) {
		PRT_ERR("CANNOT start encoding streams [0x%x]!", streamID);
		return -1;
	}
	update_encode_info();
#endif

	return retv;
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
		PRT_DBG("===== Parameter Setting is : content[%s]\n", content);
	} else {
		PRT_DBG("===== Parameter Setting is : content[%s]\n", content);

        /*-----------------------------------------------------------------------------
         *  1.from cgi to map
         *  2.from map to board
         *-----------------------------------------------------------------------------*/
		retv = config_params(section->map, content);
		retv = (*section->set)(section->name);
		ack.result = ack.info = retv;
	}
	retv = send_text((u8 *)&ack, sizeof(ack));
	return retv;
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
//		do_change_br(req);
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

void * webserver_process(void * argv)
{
	if (create_server() < 0) {
		PRT_ERR("create_server");
		return (NULL);

	}
    while (1) {
        main_loop();
    }

    return (NULL);
}
