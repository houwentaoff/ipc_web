/*
 ****************************************************************************
 *
 ** \file      base_page.c
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
#include "include.h"

#define SHORTLEN 32


#if 0
#define MAXINPUTLEN 2048
char postInfo[MAXINPUTLEN] = {0};

int get_value (char* text,char* cmp_Text)
{
    char* buffer = strstr(text, cmp_Text);
    char value[SHORTLEN] = {0};
    int i = 0;
    int j = 0;
    int flag = FALSE;

    memset(value, 0, SHORTLEN);
    if (buffer != NULL) {
        int len = strlen(buffer);
        for (i = 0; i < len; i++) {
            if (flag) {
                if ((*buffer) == '&') {
                    break;
                }
                value[j++] = (*buffer);
            }
            if ((*buffer) == '=') {
                flag = TRUE;
            }
            buffer++;
        }
        value[j] = '\0';
        return atoi(value);
    } 
    else
    {
        return 0;
    }
}
#endif

int CamBasicPage_process_PostData () 
{
    int vin_fps_changed = 0;
    int vv_changed = 0;
    Transfer transfer;
    Message msg;
    transfer_init(&transfer);

    Message image_data;
    Message vv_data;

    char sec_name[SHORTLEN];
    char data_name[SHORTLEN];
    int req_cnt=-1 ;

    cgiFormInteger("req_cnt", &req_cnt, 0);
    FUN_IN("req_cnt[%d]\n", req_cnt);
    
    if (req_cnt>0)
    {
        int count = 0;
        do
        {
            memset(&msg, 0, sizeof(Message));
            sprintf(sec_name, "sec%d", count);
            cgiFormString(sec_name, msg.section_Name, MSG_NAME_LEN);
            sprintf(data_name, "data%d", count);
            cgiFormString(data_name, msg.msg, MSG_INFO_LEN);

            if (strcmp(msg.section_Name, "IMAGE") == 0) 
            {
                memset(&image_data, 0, sizeof(Message));
                vin_fps_changed = 1;
                strncat(image_data.section_Name, msg.section_Name, strlen(msg.section_Name));
                strncat(image_data.msg, msg.msg, strlen(msg.msg));

            }else if (strcmp(msg.section_Name, "VINVOUT") == 0)
            {
                memset(&vv_data, 0, sizeof(Message));
                vv_changed = 1;
                strncat(vv_data.section_Name, msg.section_Name, strlen(msg.section_Name));
                strncat(vv_data.msg, msg.msg, strlen(msg.msg));
                PRT_DBG("vv_data.msg[%s]\n", msg.msg);
            }
            count++;
        }while (count<req_cnt);

#if 0
    if ((strstr(postInfo, "req_cnt") != NULL) 
            && (strstr(postInfo, "sec") != NULL)
            && (strstr(postInfo, "data") != NULL)) 
    {
        char* string_buffer = NULL;
        string_buffer = (char*)malloc(MSG_INFO_LEN);
        if (string_buffer == NULL) 
        {
            PRT_DBG("CamBasic post data error");
            return -1;
        }
        memset(string_buffer, 0, MSG_INFO_LEN);
        url_decode(string_buffer, postInfo, strlen(postInfo));
        for(i = 0; i < req_cnt; i++) 
        {

            memset(&msg, 0, sizeof(Message));

            parse_postSec(string_buffer,&msg ,i);
            parse_postData(string_buffer,&msg ,i);

            if (strcmp(msg.section_Name, "IMAGE") == 0) 
            {
                memset(&image_data, 0, sizeof(Message));
                vin_fps_changed = 1;
                strncat(image_data.section_Name, msg.section_Name, strlen(msg.section_Name));
                strncat(image_data.msg, msg.msg, strlen(msg.msg));

            }else if (strcmp(msg.section_Name, "VINVOUT") == 0)
            {
                memset(&vv_data, 0, sizeof(Message));
                vv_changed = 1;
                strncat(vv_data.section_Name, msg.section_Name, strlen(msg.section_Name));
                strncat(vv_data.msg, msg.msg, strlen(msg.msg));

            }
        }
        free(string_buffer);
        string_buffer = NULL;
#endif
        PRT_DBG("vin_fps_changed[%d]vv_changed[%d]\n",
                vin_fps_changed, vv_changed);
#if 0
        if (vin_fps_changed) 
        {
            memset(&msg, 0, sizeof(Message));
            strcat(msg.section_Name, "IMAGE");
            strcat(msg.msg, "slow_shutter = 0");

            if ((transfer.send_set_request(REQ_SET_PARAM, IMAGE, msg) < 0)) 
            {
                PRT_DBG();
                return -1;
            }
        }
#endif
        if (vv_changed) {
            if ((transfer.send_set_request(REQ_SET_PARAM, VINVOUT, vv_data)) < 0) 
            {
                PRT_DBG();
                return -1;
            }
        }
        else 
        {
            return -1;
        }
#if 0
        if (vin_fps_changed) 
        {
            strcat(image_data.msg, "slow_shutter = 1");
            if ((transfer.send_set_request(REQ_SET_PARAM, IMAGE, image_data)) < 0) 
            {
                PRT_DBG();
                return -1;
            }
        }
#endif
        PRT_DBG();
        FUN_OUT();
        return 0;
    }
    else 
    {
        PRT_DBG();
        FUN_OUT();
        return 1;
    }
}

