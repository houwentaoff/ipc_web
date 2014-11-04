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

#define MAXSTRLEN           (256)               /*错误通常由此引起 */
#define SMALLHTML           (6*1024)            /*  */
#define DIV_SELECT_SIZE     (512)            /* tmp, div select size*/
static ParamData vinvout_params[VINVOUT_PARAMS_NUM];

static int create_div_select(char * text, div_select_t * div_select)
{
    int num = div_select->option_num;
    char div_string[MAXSTRLEN] = {0};
    char* option_string_format = "<option value=%d%s>%s</option>\n";
    char option_string[MAXSTRLEN] = {0};
    int i = 0;

    sprintf(div_string, "<div id=\"%s\"><label>\"%s\"</label><select>", div_select->id, div_select->label);

    strcat(div_string, ">\n");

    strncat(text, div_string, strlen(div_string));
    PRT_DBG("div_string[%s]\n", div_string);

    for (i = 0; i < num; i++)   {
        memset(option_string, 0, sizeof(option_string));
        if (div_select->options[i].value == div_select->selected) {
            sprintf(option_string, option_string_format, div_select->options[i].value, \
                    " selected", div_select->options[i].label);
        } else {
            sprintf(option_string, option_string_format, div_select->options[i].value, \
                    " ", div_select->options[i].label);
        }
        strncat(text, option_string, strlen(option_string));
    }
    char* div_format_end = "</select></div>\n";
    strncat(text, div_format_end, strlen(div_format_end));

    return (0);
}
/**
 * @brief 
 *
 * @param text
 * @param select_label
 *
 * @return 
 */
static int create_select_label(char * text, select_Label_t* select_label)
{
    char* label_format_title = "<label for=\"%s\">%s</label><select id=\"%s\" ";
    int num = select_label->option_num;
    char label_string[MAXSTRLEN] = {0};

//    FUN_IN();     

    sprintf(label_string, label_format_title, \
            select_label->bind.name, select_label->label, select_label->bind.id);
    if (select_label->action)
    {
        sprintf(label_string + strlen(label_string), "onchange =\"%s\"", select_label->action);
    }
    strcat(label_string, ">\n");

    strncat(text, label_string, strlen(label_string));

    char* option_string_format = "<option value=%d%s>%s</option>\n";
    char option_string[MAXSTRLEN] = {0};
    int i = 0;
    for (i = 0; i < num; i++)   {
        memset(option_string, 0, sizeof(option_string));
        if (select_label->options[i].value == select_label->selected) {
            sprintf(option_string, option_string_format, select_label->options[i].value, \
                    " selected", select_label->options[i].label);
        } else {
            sprintf(option_string, option_string_format, select_label->options[i].value, \
                    " ", select_label->options[i].label);
        }
        strncat(text, option_string, strlen(option_string));
    }
    char* label_format_end = "</select>\n";
    strncat(text, label_format_end, strlen(label_format_end));

//    FUN_OUT();     
    return (0);
}

int Ambavivo_page_get_params()
{
    	int ret = 0;
//	char name[NAME_LEN] = {0};
//	int i;
//	unsigned int stream_port[] = {STREAM0,STREAM1,STREAM2,STREAM3};

//	ret = (&virtual_PageOps)->process_PostData(currentPage);
	section_Param section_param;

    FUN_IN();
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
                if (AmbaBase_get_section_param(&section_param)==-1)
                {
                    return (-2);
                }
                int i=0;
                while (i< section_param.paramDataNum)
                {
                    PRT_DBG("name[%s] param_value[%s] value[%d]\n",
                            section_param.paramData[i].param_name,
                            section_param.paramData[i].param_value,
                            section_param.paramData[i].value);
                    i++;
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

int vivo_page()
{
    char *text=NULL;
    div_select_t select_label[5] =
    {
        {
            .label      = "Resolution :",    
            .id         = "vin_Resolution",
            .options    = NULL,
            .option_num = 3,
            .selected   = 2,
            .action     = NULL,
        },
        {
            .id         = "vin_fps",
            .label      = "Frame Rate (fps) :",    
            .options    = NULL,
            .option_num = 3,
            .selected   = 2,
            .action     = NULL,
        },
        NULL,
    };
    char *vin_Resolution[]={"Auto", "1080P", "720P", NULL};
    char *vin_fps[]={"5", "6", "7", NULL};
    char **divs[]={vin_Resolution, vin_fps};
    char div_text[DIV_SELECT_SIZE]={0};
    int i,j;

    FUN_IN();

    text = (char *)malloc(SMALLHTML);/*6k may much small*/
    if (!text)
    {
        PRT_ERR("text is null\n");
        return (GK_MEM_ERROR);
    }

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

    sprintf(text, "%s", head_html);
    strncat(text, nav, strlen(nav));
    //get msg from ctrl server
    //get 分辨率
    //get fps
    Ambavivo_page_get_params();

    i =0;
    while (i<2)/**/
    {
        if (i == 3)
        {
            strncat(div_text, "<br><br>", strlen("<br><br>"));//????
        }
        create_div_select(div_text, &select_label[i]);
        i++;
    }
    
    char *pos=text;
    pos += strlen(text);

    sprintf(pos, vivocontent, div_text);
    PRT_DBG("divtext[%s]\n", div_text);

    fprintf(cgiOut, "%s", text);    
    FUN_OUT();

    return (0);
}

int   view_page()
{
    char *text=NULL;
    char *load_activeX = "onload=\"javascript: OnLoadActiveX('192.168.103.47', 0, 1, 0, 1);\"";/*Sean Hou: ip need to replace by js fun */
    FUN_IN();
    /*init display*/
    text = (char *)malloc(SMALLHTML);/*6k may much small*/
    if (!text)
    {
        PRT_ERR("text is null\n");
        return (GK_MEM_ERROR);
    }        

    sprintf(text, body_html, load_activeX);
    strncat(text, video_html, strlen(video_html));

    fprintf(cgiOut, "%s", text);

    PRT_DBG("size[%d]text[%s]\n",strlen(text), text);
   
    free(text);

    FUN_OUT();

    return (0);
}


int   enc_page()
{
    int j,i;
    char *text=NULL;
    char *enc_mode[]      = {"Normal mode", "High Mega-pixel mode", "Low delay mode"};
    char *s0_type[]       = {"OFF", "H.264", "MJPEG"};
    char *s0_enc_fps[]    = {"60", "30", "25", "20", "15", "10", "6", "5", "4", "3", "2", "1"};
    char *s0_dptz[]       = {"Disable", "Enable"};
    char *s0_resolution[] = {"1920 x1080", "1440 x1080", "1280 x1024", "1280 x 960",
        "1280 x 720", "800 x 600", "720 x 576", "720 x 480", "640 x 480",
        "352 x 288", "352 x 240", "320 x 240", "176 x 144", "176 x 120",
        "160 x 120",
    };
    char *s0_flip_rotate[] = {"Normal", "Horizontal Flip", "Vertical Flip", "Rotate Clockwise 90", "Rotate 180", "Rotate Clockwise 270"};

    char **labels[]={s0_type, s0_enc_fps, s0_dptz, s0_resolution, s0_flip_rotate};
    
    /*-----------------------------------------------------------------------------
     *  select
     *-----------------------------------------------------------------------------*/
    select_Label_t select_labeltop    =
    {
        .label      = "Encode Mode :",    
        .bind.id    = "enc_mode",
        .options    = NULL,
        .option_num = 3,
        .selected   = 2,
        .action     = "setEncodeMode(this.options[this.selectedIndex].value)",
    };//top
    select_Label_t select_label[5] =
    {
        {
            .label      = "Type :",    
            .bind.id    = "s0_type",
            .options    = NULL,
            .option_num = 3,
            .selected   = 2,
            .action     = NULL,
        },
        {
            .label      = "Encode FPS :",    
            .bind.id    = "s0_enc_fps",
            .options    = NULL,
            .option_num = 12,
            .selected   = 2,
            .action     = NULL,
        },        
        {
            .label      = "DPTZ Type :",    
            .bind.id    = "s0_dptz",
            .options    = NULL,
            .option_num = 2,
            .selected   = 2,
            .action     = NULL,
        },
        {
            .label      = "Resolution :",    
            .bind.id    = "s0_resolution",
            .options    = NULL,
            .option_num = 15,
            .selected   = 2,
            .action     = NULL,
        },            
        {
            .label      = "Flip & Rotate :",    
            .bind.id    = "s0_flip_rotate",
            .options    = NULL,
            .option_num = 6,
            .selected   = 2,
            .action     = NULL,
        },                       
    };
    char* fieldset_begin = "<fieldset><legend>%s</legend><br>\n";
    char* fieldset_end   = "<br><br></fieldset><br>\n";
    
    FUN_IN();     

    /*init select*/
    text = (char *)malloc(SMALLHTML);
    if (!text)
    {
        PRT_ERR("text is null\n");
        return (GK_MEM_ERROR);
    }    
    select_labeltop.options = (option_t *)malloc(select_labeltop.option_num * sizeof(option_t));
    /*top  enc_mode*/
    for (i = 0; i<select_labeltop.option_num;i++)
    {
        (select_labeltop.options+i)->value = i;
        (select_labeltop.options+i)->label = enc_mode[i];/*mark*/
    }
    create_select_label(text, &select_labeltop);

    /*other  select*/
    for (j = 0;j<5;j++)
    {
        select_label[j].options = (option_t *)malloc(select_label[j].option_num * sizeof(option_t));
        for (i = 0; i<select_label[j].option_num;i++)
        {
            (select_label[j].options+i)->value = i;
            (select_label[j].options+i)->label = labels[j][i];/*mark*/
        }
    }

    /*stream 0*/
    strncat(text, "<br><br>", strlen("<br><br>"));
    sprintf(text+strlen(text), fieldset_begin, "Stream 0");
    
    for (i=0;i<5;i++)
    {
        if (i == 3)
        {
            strncat(text, "<br><br>", strlen("<br><br>"));
        }
        create_select_label(text, &select_label[i]);
    }
    strncat(text, fieldset_end, strlen(fieldset_end));
    /*stream 0 over*/

    /*stream 1*/
    strncat(text, "<br><br>", strlen("<br><br>"));
    sprintf(text+strlen(text), fieldset_begin, "Stream 1");
    
    for (i=0;i<5;i++)
    {
        if (i == 3)
        {
            strncat(text, "<br><br>", strlen("<br><br>"));
        }
        create_select_label(text, &select_label[i]);
    }
    strncat(text, fieldset_end, strlen(fieldset_end));
    /*stream 1 over*/    
#if 1
    /*stream 2*/
    strncat(text, "<br><br>", strlen("<br><br>"));
    sprintf(text+strlen(text), fieldset_begin, "Stream 2");
    
    for (i=0;i<5;i++)
    {
        if (i == 3)
        {
            strncat(text, "<br><br>", strlen("<br><br>"));
        }
        create_select_label(text, &select_label[i]);
    }
    strncat(text, fieldset_end, strlen(fieldset_end));
    /*stream 2 over*/
#endif
   char *button_buf = "<p align=\"center\" > \
<input type=\"button\" value=\"Apply\" onclick = \"javascript:setEnc()\"/>&nbsp; &nbsp; \
<input type=\"button\" value=\"Cancel\" onclick = \"javascript:showPage('enc')\"/>";
    strncat(text, button_buf, strlen(button_buf));
    fprintf(cgiOut, "%s", text);
    PRT_DBG("size[%d]text[%s]\n",strlen(text), text);
    free(text);

    FUN_OUT();

    return (0);
}
int   pm_page()
{

    return (0);
}
int   osd_page()
{

    return (0);
}
int   sys_page()
{

    return (0);
}

