/***************************************************************************
 * 
 * Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
 *  
 ***************************************************************************/
/**
 * @file sim_conf.cpp
 * @author lizhanping(lizhanping01@baidu.com)
 * @date 2012/09/26
 * @brief 
 * read product configuration
 */

#include "sim_conf.h"
#include "Configure.h"
#include "comdef.h"
#include "utils.h"
#include "sim_include.h"
#include "client.h"

using namespace comcfg;

/*global product configurator*/
bsl::hashmap <string,proconf_t> g_proconf[2];

/*two hashmap,g_index indicate which map is being used*/
int g_index = 0;

simconf_t simconf;

int 
load_product_conf(int index)
{
	const char *dir = simconf.conf_dir;
	const char *file = simconf.conf_file;
    int ret = -1;
	/*get config*/
	string pid;
	uint32_t cnum;
	proconf_t ctmp;


	Configure * conf = new Configure();
    if(!conf){
        //UB_LOG_FATAL("configure new failed");
        return -1;
    }
	if(conf->load(dir,file) != 0){
		//UB_LOG_FATAL("load product configurator fail,path[%s] file[%s]",dir,file);
		goto out;
	}
	if(g_proconf[index].clear() != 0){
		//UB_LOG_FATAL("product hash map destroy failed");
		goto out;
	}

	/*get product number*/
	int num = (*conf)["product"]["restrict"].size();
	if(num <= 0){
		//UB_LOG_FATAL("no conf for [product->restrict]");
		goto out;
	}


	for(int i = 0;i < num;i ++)
	{
		pid = (*conf)["product"]["restrict"][i]["pid"].to_cstr();
		cnum = (*conf)["product"]["restrict"][i]["pattern"].to_uint32(); 
		if(cnum > SENTENCE){
			//UB_LOG_FATAL("pattren[%d] is error",cnum);
			goto out;
		}
		ctmp.pattern = cnum == 0 ? FULLTEXT : cnum == 1 ? PARAGRAPH : SENTENCE;
		ctmp.fulltext_expire_time = (*conf)["product"]["restrict"][i]["fullTextExpireTime"].to_uint32();
		ctmp.paragraph_expire_time = (*conf)["product"]["restrict"][i]["paragraphExpireTime"].to_uint32();
		ctmp.sentence_expire_time = (*conf)["product"]["restrict"][i]["sentenceExpireTime"].to_uint32();
		ctmp.private_data_len = (*conf)["product"]["restrict"][i]["privateDataLen"].to_uint32();
		ctmp.max_redis_list = (*conf)["product"]["restrict"][i]["maxRedisList"].to_uint32();
		ctmp.user_key_len = (*conf)["product"]["restrict"][i]["userKeyMaxLen"].to_uint32();

		/*read the permanent key from configure, store in  vector<string> permanentKeys*/
		int tnum = (*conf)["product"]["restrict"][i]["permanentkey"].size();
		string pk;	
		//UB_LOG_DEBUG("user key num is [%d]",tnum);
		if (tnum > 0) {
			for(int j=0; j<tnum;j ++) {
				pk.clear();	//clear string
			       	pk = (*conf)["product"]["restrict"][i]["permanentkey"][j]["key"].to_cstr();	//get permanent key
				ctmp.permanentKeys.push_back(pk);	//put permanent key into 
				if (pk.empty()) {
					//UB_LOG_WARNING("permanent key string is null");
				}
				else {
					//UB_LOG_DEBUG("permanent key[%d} is %s",j,pk.c_str());	//output permanent key 
				}
			}
		}
		/*author shixinyue@baidu.com*/


		ret = g_proconf[index].set(pid,ctmp);
		if(bsl::HASH_INSERT_SEC != ret)
		{
			//UB_LOG_FATAL("hashmap set failed");
			goto out;
		}

		//UB_LOG_NOTICE("pid[%s] [%d][%u][%u][%u][%u][%u][%u]",pid.c_str(),ctmp.pattern,
				ctmp.fulltext_expire_time,ctmp.paragraph_expire_time,
				ctmp.sentence_expire_time,ctmp.private_data_len,
				ctmp.max_redis_list,ctmp.user_key_len);
	}
    ret = 0;
out:
    delete conf;
	return ret;
}


/*just for convenience,client.conf reload is realized here*/
void * conf_ontime_reload(void *)
{
	time_t conf_mtime;
	bzero(&conf_mtime, sizeof(time_t));
	time_t client_conf_mtime;
	bzero(&client_conf_mtime,sizeof(time_t));

	char conf_path[256];
	snprintf(conf_path,sizeof(conf_path),"%s/%s",simconf.conf_dir,simconf.conf_file);
	file_ismodify(conf_path,conf_mtime,&conf_mtime);
    file_ismodify("./conf/client.conf",client_conf_mtime,&client_conf_mtime);
	//UB_LOG_DEBUG("conf ontime reload deamon");
	while(1){
		sleep(simconf.conf_reload_time);
		if(file_ismodify(conf_path,conf_mtime,&conf_mtime)){
			int index = 1 - g_index;
			if(load_product_conf(index) == 0){
				g_index = index;
				sleep(5);
			}
			//UB_LOG_TRACE("current conf map index[%d]",g_index);
		}

		if(file_ismodify("./conf/client.conf",client_conf_mtime,&client_conf_mtime)){
			int index = 1 - ubmgr_index;
			if(client_init(index) == 0){
				ubmgr_index = index;
				sleep(5);
			}
			//UB_LOG_TRACE("current client index[%d]",ubmgr_index);
		}
	}
	return NULL;
}



int product_map_init()
{
	for(int i = 0;i < 2;i ++)
	{
		if(g_proconf[i].create(MAX_PRODUCT_NUM) != 0){
			//UB_LOG_FATAL("product hashmap init failed");
			return -1;
		}
	}
	return 0;
}


int load_simhash_conf(ub_conf_data_t *conf)
{
	if(conf == NULL)
	{
		//UB_LOG_FATAL("load simhash conf file failed");
		return -1;
	}

	/*
	UB_CONF_GETINT(conf,"ontime_reload",&(simconf.conf_reload_time),NULL);
	UB_CONF_GETNSTR(conf,"conf_dir",simconf.conf_dir,MAX_PATH_LEN,"conf path");
	UB_CONF_GETNSTR(conf,"conf_file",simconf.conf_file,MAX_PATH_LEN,"conf filename");
	UB_CONF_GETNSTR(conf,"scw_conf_file",simconf.scw_conf_file,MAX_PATH_LEN,"scw_conf_file");
	UB_CONF_GETNSTR(conf,"wordict_path",simconf.wordict_path,MAX_PATH_LEN,"wordict_path");
    UB_CONF_GETUINT(conf,"expire_value_check_num",&(simconf.expire_value_check_num),NULL);
    UB_CONF_GETUINT(conf,"redis_write_mcpack_size",&(simconf.redis_write_mcpack_size),NULL);
    UB_CONF_GETUINT(conf,"redis_read_mcpack_size",&(simconf.redis_read_mcpack_size),NULL);
    UB_CONF_GETUINT(conf,"gbk2utf_tmpbuf_size",&(simconf.gbk2utf_tmpbuf_size),NULL);
    UB_CONF_GETNSTR(conf,"redis_uname",simconf.redis_uname,REDIS_UNAME_SIZE,"redis_uname");
	UB_CONF_GETNSTR(conf,"redis_tk",simconf.redis_tk,REDIS_TK_SIZE,"redis_tk");
	*/
	ub_conf_close(conf);
	conf = NULL;
	return 0;
}
