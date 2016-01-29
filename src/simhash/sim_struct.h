/***************************************************************************
 * 
 * Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
 *  
 ***************************************************************************/
/**
 * @file sim_struct.h
 * @author lizhanping(lizhanping01@baidu.com)
 * @date 2012/09/25
 * @brief 
 * define some common struct
 */

#ifndef __SIM_STRUCT_H__
#define __SIM_STRUCT_H__

#include "sim_include.h"
#include "comdef.h"
#include "scwdef.h"

enum strategy{
	APPEND = 0,
	COMPARE = 1,
	APPCOM	= 2,
    DELETE = 3
};

enum wtype{
	DEGREE = 0,
	WORDRANK = 1
};



/*simhash signatures*/
enum pattern{
	FULLTEXT = 0,
	PARAGRAPH = 1,
	SENTENCE = 2
};

/*user-defined features*/
 typedef struct _funit{
	float weight;
	string feature;
}funit;

/*request msg struct*/
typedef struct _sim_input_t{
	string pid;
	string key;	/*user key*/
	mc_uint64_t docid;	/*document id*/
	string encoding;
	string content;
    enum wtype weight;/*term weight calc method*/
    bool stepword;/*step word delete label*/
    bool hitdelay;/*does hit k-v need to delay expire time*/
	enum pattern pattern;	/*fulltext paragraph sentence*/
	float simrate;	/*according to simrate,two doc can be judged as similar or not*/
	int hamdis;	/*Hamming distance*/
	uint32_t expiretime;	/*doc expiration time*/
	enum strategy strategy;/*strategy:append compare appcom*/
	vector<funit> features;/*user-defined features*/
	char privatedata[RAW_DATA_MAX_SIZE + 1];/*user private data*/
	uint32_t raw_size;	/*private data size*/
    mc_uint64_t sign;/*if strategy=delete,sign if doc 64 bit signature*/
    uint32_t deadline;/*deadline = currtime + expiretime*/
}sim_input_t;


/*value{simid:uint64,docid:uint64,timestamp:uint32,private:raw}*/
/*please do not define variable between simid and userdata*/
typedef struct _value_t{
	uint64_t simid;/*64 bit simhash sign*/
	uint64_t docid;/*document id*/
	uint32_t timestamp;/*the time value expire*/
	char userdata[RAW_DATA_MAX_SIZE + 1];/*user data private data*/
	u_int datasize;/*user data real size*/
}value_t;

typedef struct _simval{
    string key;/*redis key*/
    float rate;
    value_t val;
}simval;

/*response msg struct*/
typedef struct _sim_output_t{
    vector<simval> simdis[DISTANCE];
    vector<simval> simrate;
    int counts[DISTANCE + 1];/*the simdis and simrate vector num*/
	int err_no;	/*err number 0:success -1:error*/
	string err_msg;	/*if err_no is -1,err_msg describe detail info*/
}sim_output_t;


/*this struct has no use*/
typedef struct _gdata_t{
	char redis_req_buf[1048576];/*buf to store redis request data,1M*/
	char redis_res_buf[1048576];/*buf to store redis response data,1M*/
}gdata_t;

/*thread specific data,thread private data*/
typedef struct _thread_data{
    char * redis_req_buf;/*buf to store redis request data*/
    char * redis_res_buf;/*buf to store redis response data*/
    char * gbk2utf_tmpbuf;
    u_int reqbuf_size;
    u_int resbuf_size;
    u_int gbk2utf_tmpbuf_size;
    scw_out_t * pout;/*word segment result buffer,each thread has one individual buffer*/
}thread_data;

#endif
