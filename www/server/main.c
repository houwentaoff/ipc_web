/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Goke C&S.
 *       Filename:  main.c
 *
 *    Description:  测试webserver.c
 *         Others:
 *   
 *        Version:  1.0
 *        Date:  Monday, November 03, 2014 10:16:36 HKT
 *       Revision:  none
 *       Compiler:  arm-gcc
 *
 *         Author:  Sean , houwentaoff@gmail.com
 *   Organization:  Goke
 *        History:  Monday, November 03, 2014 Created by SeanHou
 *
 * =====================================================================================
 */


#include <stdlib.h>
#include <stdio.h>

#include "webservice.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] )
{
    webserver_process(*argv);

    return EXIT_SUCCESS;
}
