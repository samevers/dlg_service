/***************************************************************************
 * 
 * Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
 *  
 ***************************************************************************/
/**
 * @file redis.h
 * @author lizhanping(lizhanping01@baidu.com)
 * @date 2012/10/11
 * @brief 
 * redis client,send msg to server
 */

#ifndef __REDIS_H__
#define __REDIS_H__

#include "sim_include.h"
#include "comdef.h"
#include "sim_struct.h"

/*LTRIM key start stop=-1*/
typedef struct _ltrim_t{
	string key;/*LTRIM CMD:key*/
	int start;/*LTRIM CMD:start*/
}ltrim_t;

/*EXPIRE key seconds*/
typedef struct _expire_t{
	string key;/*EXPIRE CMD:key*/
	int seconds;/*EXPIRE CMD:second*/
}expire_t;

/*RPUSH key value [value ...]*/
typedef struct _rpush_t{
	string key;
	vector<value_t> value;
}rpush_t;

/* LREM key count value */
typedef struct _lrem_t{
	string key;
	value_t value;
	int count;
}lrem_t;

/*
 * fulltext pattren,make 15 redis key by 64 bit simhash sign
 */
int make_rediskey(const char * userkey,enum pattern pat,uint64_t sign,vector<string>& rediskey);

/*
 *redis:get value by key
 */
int redis_get_value(sim_input_t *input,vector<string>& rediskey,vector<rpush_t>& redisvalue,u_int logid);

/*
 *RPUSH,insert new key-value add the hit key-value into redis
 *if the hit key-value is inserted success,will delete the 
 *old data latter by LREM
 */
int redis_insert_value(vector<string>& rediskey,uint64_t sign,sim_input_t *input,sim_output_t* output,u_int logid,int delres,bool &existSameDoc);

/*
 *document delete
 */
int redis_doc_del(sim_input_t *input,vector<string> & rediskey,u_int logid);

/*
 *the hit key-value is insert into the end of list,
 *need to delete the old key-value
 */
int redis_del_value(sim_input_t * input,sim_output_t* output,u_int logid);

/*
 *这仅仅是个ltrim操作，和下面的接口区别是：下面接口是ltrim+expire
 *这个接口使用于append操作
 */
int redis_ltrim_ahead(sim_input_t * input,vector<string>& rediskey,u_int logid);

/*
 *get rid of expired value
 *if number of value is more than threshold,truncate
 */
int redis_ltrim_value(sim_input_t * input,vector<rpush_t> & redisvalue,u_int logid);


/*
 *set expire time to key
 */
int redis_expire_key(sim_input_t * input,vector<string>& rediskey,u_int logid);

#endif
