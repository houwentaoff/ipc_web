#include "include.h"

int close_connect (int sockfd)
{
	close(sockfd);
	return 0;
}

int Send_Msg (int sockfd, char* msg, int length)
{
	if (send(sockfd,msg, length,0) < 0) {
			return -1;
		}
	return 0;
}

int Recv_Msg (char* ack, int sockfd, int msg_Length)
{
	int  recvbytes;

	if ((recvbytes = recv(sockfd, ack, msg_Length, 0)) == -1) {
		//printf("recv errno=%d.", errno);
		//perror("recv");
		return -1;
	}
	//(*ack).info[(*ack).info_length] = '\0';
	return 0;
}

