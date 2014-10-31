/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Goke.
 *       Filename:  page.h
 *
 *    Description:  
 *         Others:
 *   
 *        Version:  1.0
 *        Date:  2014/8/26 19:34:15
 *       Revision:  none
 *       Compiler:  xxx-gcc
 *
 *         Author:  Sean Hou , houwentaoff@gmail.com
 *   Organization:  Goke
 *        History:   Created by housir
 *
 * =====================================================================================
 */
#ifndef  __PAGE_H__
#define  __PAGE_H__

#define LABEL_OPTION_LEN 32

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

extern int   view_page();
extern int   enc_page();
extern int   pm_page();
extern int   osd_page();
extern int   sys_page();
extern int   vivo_page();


#endif//__PAGE_H__

