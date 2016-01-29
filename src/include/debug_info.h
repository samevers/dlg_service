/*************************************************
 *
 * Author: lichao
 * Create time: 2015  4ÔÂ 17 11Ê±05·Ö56Ãë
 * E-Mail: lichaotx020@sogou-inc.com
 * version 1.0
 *
*************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

#define DEBUG_STATUS 1

static int DEBUG_INFO(const char *fmt, ...) 
{
	if (DEBUG_STATUS != 1) 
		return 1;                                                                                                     
	    
	va_list argp;
	fprintf(stderr, "[DEBUG]:");
	va_start(argp, fmt);
	vfprintf(stderr, fmt, argp);
	va_end(argp);
	fprintf(stderr, "\n");
	return 0;
}

