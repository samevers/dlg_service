/***************************************************************************
 * 
 *  
 ***************************************************************************/
/**
 * @file comdef.h
 * @date 2012/09/25
 * @brief 
 * define 
 */

#ifndef __COMDEF_H__
#define __COMDEF_H__

/*default simrate*/
#define SIMRATE		0.6

/*mcpack temp buffer len*/
#define MAX_PACK_BUF_LEN	2048

/*Hamming distance*/
#define DISTANCE	5

/*product max number*/
#define MAX_PRODUCT_NUM	200

/*file or dir max length*/
#define MAX_PATH_LEN	256

/*redis cmd num*/
#define NUM_REDIS_CMD	5

/*user private data max size byte*/
#define RAW_DATA_MAX_SIZE	64

/*redis uname and tk size*/
#define REDIS_UNAME_SIZE    256

#define REDIS_TK_SIZE   REDIS_UNAME_SIZE

#define TIMEDIFF(s, e)  ((e.tv_sec-s.tv_sec)*1000000 + e.tv_usec - s.tv_usec)

#endif
