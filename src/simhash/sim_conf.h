/***************************************************************************
 * 
 * Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
 *  
 ***************************************************************************/
/**
 * @file sim_conf.h
 * @author lizhanping(lizhanping01@baidu.com)
 * @date 2012/09/26
 * @brief 
 * read product configuration
 */

#ifndef __SIM_CONF_H__
#define __SIM_CONF_H__

#include "sim_struct.h"
#include "comdef.h"

/*each product configure*/
typedef struct _proconf_t{
	enum pattern pattern;
	uint32_t fulltext_expire_time;
	uint32_t paragraph_expire_time;
	uint32_t sentence_expire_time;
	uint32_t private_data_len;	/*the size of private bound*/
	uint32_t max_redis_list;	/*max length of the redis list*/
	uint32_t user_key_len;		/*user key max len*/
	vector<string> permanentKeys;	/*store all permanentKeys*/
}proconf_t;

/*simhash global conf*/
typedef struct _simconf_t{
	int conf_reload_time;	/*unit:minute*/
	char conf_dir[MAX_PATH_LEN];	/*product configuration dir*/
	char conf_file[MAX_PATH_LEN];	/*product configuration file*/
	char scw_conf_file[MAX_PATH_LEN];	/*wordseg scw.conf file fullpath*/
	char wordict_path[MAX_PATH_LEN];	/*word dict path,not fullpath*/
	//int client_talk_read_time;	/*the time client wait server response*/
    uint32_t expire_value_check_num;/*threshold num to get rid of expire value*/
    uint32_t redis_write_mcpack_size;/*redis request mcpack data size*/
    uint32_t redis_read_mcpack_size;/*redis response mcpack data size*/
    uint32_t gbk2utf_tmpbuf_size;
    char redis_uname[REDIS_UNAME_SIZE];/*redis access uname*/
    char redis_tk[REDIS_TK_SIZE];/*redis access password*/
}simconf_t;

/*
 * index: which product hashmap to be loaded
 */
int load_product_conf(int index);

/**
 * product configuration hashmap init
 * @return :int
 * success :0
 * fail:-1
 */
int product_map_init();

int load_simhash_conf(ub_conf_data_t *conf);

/*
 *product configuration hot reload daemon
 */
void * conf_ontime_reload(void *);


extern bsl::hashmap <string,proconf_t> g_proconf[2]; 
extern int g_index;
extern pthread_key_t key;/*come from sim_process.cpp*/
extern simconf_t simconf;

#endif
