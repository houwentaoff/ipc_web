/*
****************************************************************************
*
** \file      cmd.c
**
** \version   $Id: cmd.c 2615 2015-01-23 09:01:49Z houwentao $
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
#include <include.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define TMP_PATH "/tmp/tmp.bmp"

int cgiMain() {
    char cmd[256];
    FILE *fp;
    char *line=NULL;
    ssize_t read;
//    int old_sockfd=STDOUT_FILENO;
    size_t len;
    
    setenv("PATH", "/bin:/sbin:/usr/bin:/usr/sbin:/usr/bin/X11:/usr/local/bin", 1);
	cgiHeaderContentType("text/html");
    cgiFormString("cmd", cmd, sizeof(cmd));
    strcat(cmd, " >" TMP_PATH " 2>&1");
//    int tty_fd = open("/dev/tty", O_RDWR);
//    dup2(tty_fd, STDOUT_FILENO);
    PRT_DBG("cmd[%s]\n", cmd);
    if (-1 == system(cmd))
    {
        PRT_DBG("system cmd[error]\n");
    }
//    dup2(STDOUT_FILENO, old_sockfd);
#if 1
    if (!(fp = fopen(TMP_PATH, "r")))
    {
        return (-1);
    }
    while ((read = getline(&line, &len, fp)) != -1)
    {
        fprintf(cgiOut, "%s<br />", line);
    }
    free (line);
    remove(TMP_PATH);
#endif    
	return (GK_CGI_NO_ERROR);
}

