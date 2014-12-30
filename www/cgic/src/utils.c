/*
 ****************************************************************************
 *
 ** \file      utils.c
 **
 ** \version   $Id: utils.c 2523 2014-12-17 06:53:55Z houwentao $
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "utils.h"
#include "include.h"

#define ENC_PARAM_LEN 25
#define STREAM_PARAM_LEN 44
#define SHORTLEN 256

int Base_get_section_param (section_Param* section_param)
{
   	Transfer transfer;
	int ret = 0;
	transfer_init(&transfer);
	Message msg;
	Pack pack;
	pack_init(&pack);

	pack.pack_msg(0, section_param->sectionName, \
		section_param->extroInfo, &msg);
	ret = transfer.send_get_request(section_param, REQ_GET_PARAM, msg);
	if (ret < 0) {
		return -1;
	}
	return (GK_CGI_NO_ERROR);
}
int Base_set_section_param (section_Param* section_param)
{
	return (GK_CGI_NO_ERROR);
}

//Pack

int pack_init (Pack* pack)
{
	pack->pack_req = pack_req;
	pack->pack_msg = pack_msg;
	return (GK_CGI_NO_ERROR);
}

int parse_postSec (char url_string[], Message* postdata, int datanum)
{
	char cmpdata[SHORTLEN] = {0};
	sprintf(cmpdata, "sec%d",datanum);
	char* buffer = strstr(url_string, cmpdata);

	int i = 0;
	int j = 0;
	int flag = FALSE;

	if (buffer != NULL) {
		int size = strlen(buffer);
		for (i = 0; i < size; i++) {

			if (flag) {
				if ((*buffer) == '&') {
				break;
				}
				postdata->section_Name[j++] = (*buffer);
			}
			if ((*buffer) == '=') {
				flag = TRUE;
			}

			buffer++;
		}
		//postdata->msg[j] = '\0';

		return (-1);
	} else {
		return (GK_CGI_NO_ERROR);
	}
}

int parse_postData (char url_string[], Message* postdata, int datanum)
{
	char cmpdata[SHORTLEN] = {0};
	sprintf(cmpdata, "data%d",datanum);
	char* buffer = strstr(url_string, cmpdata);

	int i = 0;
	int j = 0;
	int flag = FALSE;

	if (buffer != NULL) {
		int size = strlen(buffer);
		for (i = 0; i < size; i++) {
			if (flag) {
				if ((*buffer) == '&') {
					break;
				}
				postdata->msg[j++] = (*buffer);
			}
			if ((*buffer) == '=') {
				flag = TRUE;
			}
			buffer++;
		}
		//postdata->msg[j] = '\0';
		return (-1);
	} else {
		return (GK_CGI_NO_ERROR);
	}
}

int url_decode (char string_buffer[], char* input_str, int str_length)
{
	int i,j = 0;
	int url_index,url_index_1;
	for (i = 0; i < str_length; i++) {
		if (input_str[i] == '%') {
			if (input_str[i+1] <= '9') {
				url_index = input_str[i+1] - '0';
			} else {
				url_index = input_str[i+1] - 'A' + 10; //10 A fixed
			}
			if (input_str[i+2] <= '9') {
				url_index_1 = input_str[i+2] - '0';
			} else {
 				url_index_1 = input_str[i+2] - 'A' + 10;\
			}
			if ((url_index * 16 + url_index_1) == 10) {
				string_buffer[j] = '\n';
			} else {
				if ((url_index * 16 + url_index_1) == ' ') { //16 fixed FF
					string_buffer[j] = ' ';
				} else {
					string_buffer[j] = url_index * 16 + url_index_1;
				}
			}
			j++;
			i = i + 2;
		}else
			string_buffer[j++] = input_str[i];
	}
	string_buffer[j] = '\0';
	return (GK_CGI_NO_ERROR);
}

int pack_req (req_Msg* req, int Id, int info, int infosize)
{
    if (!req)
    {
        PRT_ERR("req is null\n");
    }
    
	req->req_Id = (unsigned int)Id;
	req->info = (unsigned int)info;
	req->infoSize = (unsigned int)infosize;
	return (GK_CGI_NO_ERROR);
}

int pack_msg (int req_cnt, char * sectionName, char * extroInfo, Message * msg)
{
    if (!sectionName || !extroInfo || !msg)
    {
        PRT_ERR("sectionName & extroInfo & msg is null\n");
        return (-1);
    }

	msg->req_cnt = req_cnt;
	memset(msg->section_Name,0,sizeof(msg->section_Name));
	memset(msg->msg,0,sizeof(msg->msg));
	strcat(msg->section_Name, sectionName);
	strcat(msg->msg, extroInfo);
    PRT_DBG("extroInfo[%s]\n", extroInfo);

	return (GK_CGI_NO_ERROR);
}

int parse_sectionData (Message message, ParamData data[])
{
	return (GK_CGI_NO_ERROR);
}

int decode_Param (section_Param* section_param, char* recv)
{
	char param_value[PARAM_VALUE_LEN] = {0};
    char param_name[PARAM_NAME_LEN] = {0};
	int i = 0;
	int index = 0;
	int pageFlag = FALSE;
    
	char* buf = NULL;
    FUN_IN("recv[%s]\n", recv);
	for (i = 0; i < section_param->paramDataNum; i++ ) {
		index = 0;
 /* :TODO:2014/12/15 18:33:54:Sean:  fixed can not find same prefix in recv eg:s0_text = "aasd" \n s0_text_tmp = 12  */
        memset(param_name, 0, sizeof(param_name));
        strcpy(param_name, section_param->paramData[i].param_name);
        strcat(param_name, " ");//mark fixed.
 /* :TODO:End---  */
		//buf = strstr(recv,section_param->paramData[i].param_name);
		buf = strstr(recv, param_name);
		if (buf != NULL) {
			pageFlag = FALSE;
			memset(param_value,0,PARAM_VALUE_LEN);
			while (((*buf) != '\n') && ((*buf) != '\0' )) {
				if (pageFlag) {
					if ((*buf) != ' ') {
						param_value[index++] = (*buf);
					}
				}
				if ((*buf) == '=') {
					pageFlag = TRUE;
				}
				buf++;

			}

			param_value[index] = '\0';
			if (param_value[0] != '"') {
				section_param->paramData[i].value = atoi(param_value);
			} else {
				section_param->paramData[i].value = -100;
				memset(section_param->paramData[i].param_value, 0, PARAM_VALUE_LEN);/*128 byte? ->0 ?*/
				strcat(section_param->paramData[i].param_value, param_value);
                PRT_DBG("str : param_value [%s]\n", param_value);
			}
        }
    }

    FUN_OUT();
	return (GK_CGI_NO_ERROR);
}
//Socket

int socket_init (char* Host, int Port, Socket* Socket)
{
	memset(Socket->sk_member.host, 0, IP_ADDR_LEN);
	if (Host == NULL) {
		strcat(Socket->sk_member.host, HOST);
	} else {
		strcat(Socket->sk_member.host, Host);
	}
	if (Port == 0) {
		Socket->sk_member.Port = PORT;
	} else {
		Socket->sk_member.Port = Port;
	}

	Socket->close_connect = close_connect;
	Socket->socket_connect = socket_connect;
	Socket->Send_Msg = Send_Msg;
	Socket->Recv_Msg= Recv_Msg;
	return (GK_CGI_NO_ERROR);
}

int socket_connect (int Port, char* IP)
{
	int sockfd;
	struct sockaddr_in serv_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		return -1;
	}
	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(Port);
	serv_addr.sin_addr.s_addr = inet_addr(IP);

	if (connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) == -1) {
		return (-1);
	}
	return sockfd;
}

int close_connect (int sockfd)
{
	close(sockfd);
	return (GK_CGI_NO_ERROR);
}

int Send_Msg (int sockfd, char* msg, int length)
{
    FUN_IN("\n---------------send buf--------------\n"
            "[%s]\n"
            "size[%d]\n"
            "--------------------------------------\n", msg, length);
	if (send(sockfd,msg, length,0) < 0) {
			return -1;
		}
	return 0;
}

int Recv_Msg (char* ack, int sockfd, int msg_Length)
{
	int  recvbytes;
    if (-1 == msg_Length)
    {
        PRT_ERR("msg_Length[%d]\n", msg_Length);
    }
	if ((recvbytes = recv(sockfd, ack, msg_Length, 0)) == -1) {
		PRT_ERR("msg_Length[%d] recv errno=%d.\n", msg_Length, errno);
		//perror("recv");
		return (-1);
	}
	//(*ack).info[(*ack).info_length] = '\0';
	return (GK_CGI_NO_ERROR);
}


//Tranfer

int transfer_init (Transfer* transfer)
{
	transfer->send_fly_request = send_fly_request;
	transfer->send_get_request = send_get_request;
	transfer->send_set_request = send_set_request;
	return (GK_CGI_NO_ERROR);
}




int send_get_request (section_Param* section_param, int RequestId, Message Msg)
{
	Socket conn;
	conn.sk_member.sockfd  = 0;
	socket_init(HOST, section_param->sectionPort, &conn);
	conn.sk_member.sockfd = conn.socket_connect(conn.sk_member.Port, conn.sk_member.host);
	Pack pack;
	pack_init(&pack);
	req_Msg req;

	int _msg = atoi(Msg.msg);//maybe stream id
	pack.pack_req(&req,RequestId, _msg, strlen(Msg.section_Name));

	if (conn.sk_member.sockfd != -1) {
		conn.Send_Msg(conn.sk_member.sockfd, (char *)&req, sizeof(req));
		conn.Send_Msg(conn.sk_member.sockfd,Msg.section_Name,strlen(Msg.section_Name));
	}
	ack_Msg ack;
	memset(&ack,0,sizeof(ack_Msg));
	conn.Recv_Msg((char*)(&ack),conn.sk_member.sockfd,sizeof(ack_Msg));

	if (ack.result == 0) {
        char* ackmsg = NULL;
        ackmsg = (char *)malloc(MSG_INFO_LEN);
        if (ackmsg == NULL) {
            PRT_DBG("send get request error");
            return (-1);
        }
        memset(ackmsg, 0, MSG_INFO_LEN);
        conn.Recv_Msg(ackmsg,conn.sk_member.sockfd,ack.info_length);

        conn.close_connect(conn.sk_member.sockfd);

        if ((ack.result == 0) && ((strlen(ackmsg)) > 0)) {
            if(ack.info_length == strlen(ackmsg)) {
                decode_Param(section_param, ackmsg);
            } else {
                PRT_DBG("recv error");
            }
        } else {
            PRT_DBG("ack.result != 0\n");
            free(ackmsg);
            ackmsg = NULL;
            return -1;
        }
        free(ackmsg);
        ackmsg = NULL;
    }
    FUN_OUT();
	return (GK_CGI_NO_ERROR);
}


int send_set_request (int RequestId, int SectionPort, Message Msg)
{
	Socket conn;
	conn.sk_member.sockfd  = 0;
	socket_init(HOST, SectionPort, &conn);
	conn.sk_member.sockfd = conn.socket_connect(conn.sk_member.Port, conn.sk_member.host);
	Pack pack;
	pack_init(&pack);
	req_Msg req;

	pack.pack_req(&req, RequestId, strlen(Msg.msg) , strlen(Msg.section_Name));
	if (conn.sk_member.sockfd != -1) {
		conn.Send_Msg(conn.sk_member.sockfd, (char *)&req, sizeof(req));
		conn.Send_Msg(conn.sk_member.sockfd,Msg.section_Name,strlen(Msg.section_Name));
	}
	ack_Msg ack;
	memset(&ack,0,sizeof(ack_Msg));
	conn.Recv_Msg((char*)(&ack),conn.sk_member.sockfd,sizeof(ack_Msg));

	if (ack.result == 0) {
		conn.Send_Msg(conn.sk_member.sockfd,Msg.msg,strlen(Msg.msg));
		conn.Recv_Msg((char*)(&ack),conn.sk_member.sockfd,sizeof(ack_Msg));
		conn.close_connect(conn.sk_member.sockfd);
		return (int)ack.result;
	}
	return (-1);
}


int send_fly_request (int RequestId, int Info, int data)
{
    FUN_IN("RequestId[%d]data[%d]\n", RequestId, data);

	Socket conn;
	conn.sk_member.sockfd  = 0;
	socket_init(HOST, PORT,&conn);
	conn.sk_member.sockfd = conn.socket_connect(conn.sk_member.Port, conn.sk_member.host);
	Pack pack;
	pack_init(&pack);
	req_Msg req;

	pack.pack_req(&req, RequestId, Info, data);
	if(conn.sk_member.sockfd != -1) {
		conn.Send_Msg(conn.sk_member.sockfd, (char *)&req, sizeof(req));
	}
	ack_Msg ack;
	memset(&ack,0,sizeof(ack_Msg));
	conn.Recv_Msg((char*)(&ack),conn.sk_member.sockfd,sizeof(ack_Msg));
	conn.close_connect(conn.sk_member.sockfd);
	return (int)ack.result;
}
