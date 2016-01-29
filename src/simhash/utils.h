/***************************************************************************
 * 
 * Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
 *  
 ***************************************************************************/
/**
 * @file utils.h
 * @author lizhanping(lizhanping01@baidu.com)
 * @date 2012/09/25
 * @brief 
 * define some common functions
 */

#ifndef __UTILS_H__
#define __UTILS_H__

/*
 * file is modified ?
 */
bool file_ismodify(const char *strfile, time_t cur_mtime,time_t * pret_mtime);

/*
 *filter the first and last space specail punc
 */
void trim_string(char * pstr);

#endif
