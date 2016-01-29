/***************************************************************************
 *
 *Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
 *  
 ***************************************************************************/
/**
 * @file segmentor.cpp
 * @author lizhanping(lizhanping01@baidu.com)
 * @date 2012/09/27
 * @brief 
 * simhash first step:segment
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "isegment.h"
#include "segmentor.h"
#include "utils.h"
#include "sim_conf.h"
#include "termrate.h"

scw_worddict_t * pwdict = NULL;

/*
 * word segment init,dict and memory
 **/
int segment_init(const char * scwconfile,const char * wordictpath)
{
	pgconf = scw_load_conf(scwconfile);
	if(pgconf == NULL){
		UB_LOG_FATAL("scw conf load failed");
		return -1;
	}
	UB_LOG_DEBUG("scw load conf success");

	if((pwdict=scw_load_worddict(wordictpath))==NULL)
	{
		UB_LOG_FATAL("error: loading wordict failed: %s",wordictpath);
		return -1;
	}

	return 0;
}

/*word segment,scw_out memory init*/
int segment_memory_init(scw_out_t **pout)
{
    if(*pout){
        UB_LOG_FATAL("word segment memory pout init again");
        scw_destroy_out(*pout);
    }

	u_int scw_out_flag;
	scw_out_flag = SCW_OUT_WPCOMP | SCW_OUT_PROP;
	if((*pout=scw_create_out(10000, scw_out_flag))==NULL)
	{
		UB_LOG_FATAL("error: initializing the output buffer error");
		return -1;
	}
    return 0;
}

int segment_words(const char * content,enum wtype type,int len,bool stepword,vector<funit>& features)
{
	if(!content || len <= 0){
		UB_LOG_WARNING("segment words,parameter error");
		return -1;
	}

    /*get word segment result buffer*/
    thread_data * tsd = (thread_data *)pthread_getspecific(key);
    if(!tsd){
        UB_LOG_FATAL("thread special data is null");
        exit(0);
    }
    scw_out_t *pout = tsd->pout;

	/*word segment*/
	if(scw_segment_words(pwdict,pout,content,len,LANGTYPE_SIMP_CHINESE,NULL) == -1){
		UB_LOG_WARNING("scw segment words failed");
		return -1;
	}

	/*get result to vectore features*/
	int i,count;
	token_t tokens[1024];
	funit tmp;
    
    /*word type,we just need SCW_OUT_WPCOMP*/
	u_int tsco[5] = {SCW_OUT_WPCOMP,SCW_OUT_BASIC,SCW_OUT_SUBPH,
		SCW_OUT_HUMANNAME,SCW_OUT_BOOKNAME};
    
	/*just SCW_OUT_WPCOMP mode,so j < 1*/
	for(int j = 0;j < 1;j ++)
	{
		count = scw_get_token_1(pout,tsco[j],tokens,1024);
		for(i = 0;i < count;i ++)
		{
			/*filter space and special punc*/
			trim_string(tokens[i].buffer);
			if(strlen(tokens[i].buffer) <= 1)
				continue;
            
            tmp.feature = tokens[i].buffer;
			tmp.weight = 1;
			features.push_back(tmp);
		}
	}

    /*get weight*/
    feature_weight(features,type);

	/*output result for debug*/
	for(i = 0;i < (int)features.size();i++)
	{
		tmp = features.at(i);
		UB_LOG_DEBUG("word[%s] weight[%f]",tmp.feature.c_str(),tmp.weight);
	}

	return 0;
}
