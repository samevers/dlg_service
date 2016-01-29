/***************************************************************************
 *
 *Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
 *  
 ***************************************************************************/
/**
 * @file segmentor.h
 * @author lizhanping(lizhanping01@baidu.com)
 * @date 2012/09/27
 * @brief 
 * simhash first step:segment
 */
#ifndef __SEGMENTOR_H__
#define __SEGMENTOR_H__

#include "scwdef.h"
#include "isegment.h"
#include "sim_struct.h"


/*dict pointer*/
extern scw_worddict_t *pwdict;

extern int scw_get_token_1( scw_out_t* pout, int type, token_t result[], unsigned max );

/*segment global init*/
int segment_init(const char * scwconfile,const char * wordictpath);

/*
 *content segment,result store into features
 *param[in] len: lenght of content
 *return 0:success;-1:fail
 */
int segment_words(const char * content,enum wtype type,int len,bool stepword,vector<funit>& features);


/*
 * word segment,scw_out memory init
 * ret: -1 fail
 *       0 success
 */
int segment_memory_init(scw_out_t **pout);


#endif
