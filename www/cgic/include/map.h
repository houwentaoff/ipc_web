/*
****************************************************************************
*
** \file      map.h
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
#ifndef __MAP__H__
#define __MAP__H__

#include <stdio.h>
#include "basetypes.h"

typedef int (*callback)();

typedef struct {
    char * tokenname;
    int (*page)(callback);
    int (*get_param)();
    int (*set_param)();
}PageMapping;

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
} Mapping;

typedef struct {
	char *		name;
	Mapping *	map;
//	get_func		get;
//	set_func		set;
} Section;

typedef struct {
	u32				mirror_pattern;
	u32				bayer_pattern;
} mw_mirror_mode;


typedef struct {
	u32				enable;
	u32				mode;
	u32				frame_rate;
	mw_mirror_mode	mirror_mode;
} mw_vin_mode;


typedef struct {
	u32				mode;
	u32				type;
} mw_vout_mode;

#endif
