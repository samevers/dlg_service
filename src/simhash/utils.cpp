/***************************************************************************
 * 
 * Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
 *  
 ***************************************************************************/
/**
 * @file utils.cpp
 * @author lizhanping(lizhanping01@baidu.com)
 * @date 2012/09/25
 * @brief 
 * define some common functions
 */


#include "sim_include.h"

/*
 * file is modified ?
 * return bool
 * true: file is modified
 * false:file is not modified
 */
bool file_ismodify(const char *strfile, time_t cur_mtime, time_t * pret_mtime)
{
	struct stat status;
	memset(&status, 0, sizeof(status));
	if (stat(strfile, &status) != 0)	
	{
		FILE *pfile = fopen(strfile, "w+");
		if (pfile) 
		{
			fclose(pfile);
		}
		else 
		{
			//UB_LOG_FATAL("open file:%s, failed", strfile);
			exit(1);
		}
		return false;
	}
	if (status.st_mtime == cur_mtime)
		return false;
	*pret_mtime = status.st_mtime;
	return true;
}


/*
 *filter the first and last space specail punc
 */
void trim_string(char * pstr)
{
	char *ptmp = pstr;
	while ((*ptmp == ' ' || *ptmp == '\t' || *ptmp == '\r'
		|| *ptmp == '\n' || *ptmp == '\v' || *ptmp == '\b')
	       && '\0' != *ptmp)
		ptmp++;
	/*if pstr == ptmp,do not need to copy*/
	if(pstr != ptmp)
		memmove(pstr, ptmp, strlen(ptmp) + 1);
	if (strlen(pstr) <= 0)
		return;
	char *pend = pstr + strlen(pstr) - 1;
	while ((*pend == ' ' || *pend == '\t' || *pend == '\r'
		|| *pend == '\n' || *pend == '\v' || *pend == '\b')
	       && '\0' != *pend)
		*pend-- = 0;
}

