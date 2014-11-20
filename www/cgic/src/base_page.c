/*
 ****************************************************************************
 *
 ** \file      base_page.c
 **
 ** \version   $Id: base_page.c 2417 2014-11-19 09:11:29Z houwentao $
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
#define MAXSTRLEN           (256)               /*错误通常由此引起 */

char *do_strchr(const char *s, int c)
{
    for (; (!(isdigit(*s))) && *s!=(char)c; s++)
    {
        if (*s == '\0')
            return NULL;
    }
    return (char *)s;
}
int change_js_var(char *strings, unsigned int replace)
{
//    int str_in = false;
    char *var;
//find '0' find '\0'
    if (!strings || replace>9)
    {
        return (GK_CGI_ARGV_ERROR);
    }

    while (*strings)
    {
        var = do_strchr(strings, '\0');
        if (isdigit(*var))
        {
            PRT_DBG("var[%s]\n", var);
            if (*var == ('0' + replace))
            {
                return (GK_CGI_NO_ERROR);
            }
            *var = '0' + replace;
        }
        strings = var;//mark
        strings++;
    }
    
    return (GK_CGI_NO_ERROR);
}
int create_div_input(char * text, div_input_t * div_input)
{
//	char* string_format = "<label for=\"%s\" id=\"%s_l\">%s</label>\n
//						<input type=\"text\" class=\"textinput\" id=\"%s\" value=\"%d\" maxlength=%d %s />\n";
	char* string_format = "<div id=\"%s_l\">\n\
						<input type=\"text\" class=\"textinput\" value=\"%d\" maxlength=%d %s /></div>\n";
	char text_buffer[MAXSTRLEN] = {0};
	sprintf(text_buffer, string_format, \
		div_input->id, div_input->value, div_input->maxlen, div_input->ro);
	strncat(text, text_buffer, strlen(text_buffer));
    return (GK_CGI_NO_ERROR);
}

/**
 * @brief 
 *
 * @param text
 * @param select_label
 *
 * @return 
 */
int create_select_label(char * text, select_Label_t* select_label)
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
    return (GK_CGI_NO_ERROR);
}

int create_div_select(char * text, div_select_t * div_select)
{
    int num = div_select->option_num;
    char div_string[MAXSTRLEN] = {0};
    char* option_string_format = "<option value=%d%s>%s</option>\n";
    char option_string[MAXSTRLEN] = {0};
    int i = 0;

    sprintf(div_string, "<div id=\"%s\" onchange=\"%s\"><label>%s</label><select>", div_select->id, div_select->action, div_select->label);

//    strcat(div_string, ">\n");

    strncat(text, div_string, strlen(div_string));
//    PRT_DBG("div_string[%s]\n", div_string);

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

    return (GK_CGI_NO_ERROR);
}

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
    int view_changed = 0;
    Transfer transfer;
    Message msg;
    transfer_init(&transfer);

    Message image_data;
    Message vv_data;
    Message view_data;

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
            else if (strcmp(msg.section_Name, "REQ_CHANGE_BR") == 0)
            {
                memset(&view_data, 0, sizeof(Message));
                view_changed = 1;
                strncat(view_data.section_Name, msg.section_Name, strlen(msg.section_Name));
                strncat(view_data.msg, msg.msg, strlen(msg.msg));
                PRT_DBG("view_data.msg[%s]\n", msg.msg);
            }
            count++;
        }while (count<req_cnt);


        PRT_DBG("vin_fps_changed[%d]vv_changed[%d]view_changed[%d]\n",
                vin_fps_changed, vv_changed, view_changed);
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
        if (view_changed)
        {
           if ((transfer.send_set_request(REQ_SET_PARAM, LIVE, view_data)) < 0) 
            {
                PRT_DBG();
                return -1;
            }
        }
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
        return (GK_CGI_NO_ERROR);
    }
    else 
    {
        PRT_DBG();
        FUN_OUT();
        return (GK_CGI_ERROR);
    }
}
int fly_process_PostData ()
{
    int req_cnt;
	char req[MSG_NAME_LEN];
	int info;
	int data;
	char msg[64];
	int req_cnt_int = 0;
	int req_send = 0;
    int stream_id=0;
	Transfer transfer;
	transfer_init(&transfer);
    
    FUN_IN();
	int i = 0;
	int ret;
    cgiFormInteger("req_cnt", &req_cnt, 0);
    cgiFormInteger("stream_Id", &stream_id, 0);
	req_cnt_int = req_cnt;
	if (req_cnt_int != 0) {
		for(i = 0; i < req_cnt_int; i++) {
            PRT_DBG("i[%d]\n", i);
			memset(msg, 0, 64);
			sprintf(msg, "info%d", i);
			cgiFormInteger(msg, &info, 0);
            
            info |= (stream_id<<STREAM_ID_OFFSET);

			memset(msg, 0, 64);
			sprintf(msg, "data%d", i);
			cgiFormInteger(msg, &data, 0);
            PRT_DBG("info[%d]data[%d]\n", info, data);

			if (info || data) {
				memset(msg, 0, 64);
				sprintf(msg, "req%d", i);
                cgiFormString(msg, req, MSG_INFO_LEN);
                PRT_DBG("req[%s]\n", req);
				if (strcmp(req, "REQ_SET_FORCEIDR") == 0) {
					req_send = REQ_SET_FORCEIDR;
				} else if (strcmp(req, "REQ_CHANGE_BR") == 0){
					req_send = REQ_CHANGE_BR;
				} else if (strcmp(req, "REQ_CHANGE_FR") == 0) {
					req_send = REQ_CHANGE_FR;
				}
				ret = transfer.send_fly_request(req_send, info, data);
				return ret;
			} else {
				return (GK_CGI_ERROR);
			}
		}
	}

    return (GK_CGI_NO_ERROR);
}


