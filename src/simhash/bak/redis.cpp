/***************************************************************************
 * 
 * Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
 *  
 ***************************************************************************/
/**
 * @file redisclient.cpp
 * @author lizhanping(lizhanping01@baidu.com)
 * @date 2012/10/10
 * @brief 
 * redis client,send msg to server
 */

#include "redis.h"
#include "client.h"
#include "sim_conf.h"
/*
 *type F:fulltext key  P:paragraph key  S:sentence key
 */
int make_rediskey(const char * userkey,enum pattern p,uint64_t sign,vector<string>& rediskey)
{
    string type;
    switch(p){
        case FULLTEXT:
             type = "F";
             break;
        case PARAGRAPH:
             type = "P";
             break;
        case SENTENCE:
             type = "S";
             break;
        default:
             UB_LOG_WARNING("pattren error");
             return -1;
    }

	int i = 0,j = 0;
	u_int tmpd[6],t;
	string tmpk;

	/*sign split:11 11 11 11 10 10*/
	for(i = 5;i >= 4;i --)
	{
		tmpd[i] = sign & 0x3ff;
		sign >>= 10;
	}
	for(;i >= 0;i --)
	{
		tmpd[i] = sign & 0x7ff;
		sign >>=11;
	}

    //for(i = 0;i < 6;i ++)
    //    UB_LOG_DEBUG("tmpd[%u]",tmpd[i]);

	char sy[10];

    /*key duplicate removal*/
    set <string> key_set;
    set <string> ::iterator sp;
    key_set.clear();
	for(i = 0;i < 5;i ++)
	{
		for(j = i+1;j < 6;j ++)
		{
            t = tmpd[i];
			if(j == 4 || j == 5)
				t = t << 10 | tmpd[j]; 
			else
				t = t << 11 | tmpd[j];
			tmpk.clear();
			tmpk.append(type);
			tmpk.append(":");
			if(userkey && strlen(userkey) > 0)
			{
				tmpk.append(userkey);
				tmpk.append(":");

			}
			sy[0] = '\0';
            //UB_LOG_DEBUG("t is [%u]",t);
			snprintf(sy,10,"%u",t);
			tmpk.append(sy);
            
            sp = key_set.find(tmpk);
            if(sp != key_set.end())
            {
                UB_LOG_DEBUG("key[%s] is reduplicate",tmpk.c_str());
                continue;
            }
            key_set.insert(tmpk);
			UB_LOG_DEBUG("redis key[%s]",tmpk.c_str());
			rediskey.push_back(tmpk);
		}
	}

	return 0;
}

int redis_cmd_send(const char *pid,nshead_t *req_head, ub_buff_t *req_buf, nshead_t *res_head, ub_buff_t *res_buf)
{
	if(!pid || !req_head || !req_buf || !res_head || !res_buf){
		UB_LOG_WARNING("parameter error");
		return -1;
	}

	char servername[128];/*servername:Redis_spam_simhash_pid*/
	snprintf(servername,128,"Redis_spam_simhash_%s",pid);

	int ret = client_nshead_talk(servername,req_head,req_buf,res_buf);
	if(ret == -1){
		UB_LOG_WARNING("redis cmd send fail");
	}

	return ret;
}

static inline int 
get_ubclient_buf(ub_buff_t* req_buf,ub_buff_t* res_buf)
{
    if(!req_buf || !res_buf){
        UB_LOG_WARNING("parameter error");
        return -1;
    }
    /*
	gdata_t * gdata = (gdata_t *)ub_server_get_user_data();
	if(!gdata){
		UB_LOG_FATAL("ub server get user data failed");
		return -1;
	}

	req_buf->buf = gdata->redis_req_buf;
    req_buf->size = sizeof(gdata->redis_req_buf); 
	res_buf->buf = gdata->redis_res_buf;
    res_buf->size = sizeof(gdata->redis_res_buf);*/
    thread_data * td = (thread_data *)pthread_getspecific(key);
	req_buf->buf = td->redis_req_buf;
    req_buf->size = td->reqbuf_size; 
	res_buf->buf = td->redis_res_buf;
    res_buf->size = td->resbuf_size;
    //UB_LOG_DEBUG("redis req buf size [%d],res buf size [%d]",req_buf->size,res_buf->size);

    return 0;
}

/*
 *redis:get value by key
 *put result into value
 */
int redis_get_value(sim_input_t *input,vector<string>& rediskey,vector<rpush_t>& redisvalue,u_int logid)
{
    if(input->strategy == APPEND)
    {
        /*APPEND操作发送一个截断命令，防止value过多*/
        redis_ltrim_ahead(input,rediskey,logid);
        UB_LOG_DEBUG("strategy[APPEND],do not need to get value");
        return 0;
    }
    int ret = 0;
	/*request buf*/
	nshead_t req_head;
	ub_buff_t req_buf;
	/*reponse buf*/
	nshead_t res_head;
	ub_buff_t res_buf;
    
    ret = get_ubclient_buf(&req_buf,&res_buf);
    if(ret != 0){
        UB_LOG_WARNING("get ub client buf failed");
        return -1;
    }
    nshead_copy(&req_head,logid);

    struct timeval end,start;
    time_t s1,s2;

    /*LRANGE CMD request msg pack*/
	if(lrange_msg_pack(&req_head,&req_buf,rediskey) != 0){
		return -1;
	}
    
    /*redis cmd send,response data is in res_buf*/
    /*some code just for debug*/
    gettimeofday(&start,NULL);
    //for(int i = 0;i < 10;i ++){
    if(redis_cmd_send(input->pid.c_str(),&req_head,&req_buf,&res_head,&res_buf)){
        return -1;
    }
    //}
    gettimeofday(&end,NULL);
    s1 = TIMEDIFF(start,end);
    //UB_LOG_DEBUG("LRANGE time[%lu]",TIMEDIFF(start,end)/10);

    rpush_t tmp;
    for(u_int i = 0;i < rediskey.size();i ++)
    {
        tmp.key = rediskey.at(i);
        redisvalue.push_back(tmp);
    }

    /*redis response msg unpack,obtain [key value-list]*/
    if(lrange_msg_unpack(&res_buf,redisvalue) != 0){
        UB_LOG_WARNING("redis cmd LRANGE execute failed");
        return -1;
    }
    gettimeofday(&start,NULL);
    s2 = TIMEDIFF(end,start);
    UB_LOG_DEBUG("lange[%lu] unpack[%lu]",s1,s2);
   
    /* 
    vector<value_t>* va;
    for(u_int i = 0;i < redisvalue.size();i ++)
    {
        UB_LOG_DEBUG("====================================");
        UB_LOG_DEBUG("key[%s]",redisvalue.at(i).key.c_str());
        va = &(redisvalue.at(i).value);
        for(u_int j = 0;j < va->size();j ++)
        {
            UB_LOG_DEBUG("simid[%lu]",va->at(j).simid);
            UB_LOG_DEBUG("docid[%lu]",va->at(j).docid);
            UB_LOG_DEBUG("timestamp[%u]",va->at(j).timestamp);
            UB_LOG_DEBUG("private data[%s]",va->at(j).userdata);
        }
        break;
    }*/

    return 0;
}

/*author shixinyue@baidu.com*/
/*
 * judge whether a key is permanent
 * 	start	*
 */ 
bool redis_key_is_permanent(string &userkey, proconf_t *pconf) {
	bool permanent = false;
	UB_LOG_DEBUG("user key is [%s]",userkey.c_str());
	vector<string> &permanentKeys = pconf->permanentKeys;
	for (uint32_t i = 0; i < permanentKeys.size();i ++) {	//use vector for the permanent keys are few
		UB_LOG_DEBUG("permanent key [%s}",permanentKeys[i].c_str());
		if ( userkey.compare(permanentKeys[i]) == 0) {
	 		permanent = true;
			UB_LOG_DEBUG("user  key is permanent");
			return permanent;
		}
	}
	return permanent;
}
/*	end	*/

/*
 *RPUSH,insert new key-value add the hit key-value into redis
 *if the hit key-value is inserted success,will delete the 
 *old data latter by LREM
 *if delres is -1,old key-value donot insert
 */
int redis_insert_value(vector<string>& rediskey,uint64_t sign,sim_input_t *input,
    sim_output_t* output,u_int logid,int delres,bool &existSameDoc)
{
	UB_LOG_NOTICE("INSERT VALUE NOW");
    int ret = 0;
	/*request buf*/
	nshead_t req_head;
	ub_buff_t req_buf;
	/*reponse buf*/
	nshead_t res_head;
	ub_buff_t res_buf;

    /*set logid and initialize*/
    nshead_copy(&req_head,logid);
    ret = get_ubclient_buf(&req_buf,&res_buf);
    if(ret != 0){
        UB_LOG_WARNING("get ub client buf failed");
        return -1;
    }

    /*construct key-value*/
    vector<rpush_t> kv;
    rpush_t vtmp;
    u_int i,j,k;
    value_t val;
   
    kv.clear();
    /*get timestamp*/
    time_t sec = time(NULL);
    uint32_t timestamp = sec; 
    /*get the hit value expire time*/
    proconf_t pconf;
    g_proconf[g_index].get(input->pid,&pconf); 
    bool keyIsPermanent = redis_key_is_permanent(input->key, &pconf);//whether the key is permanent
    uint32_t exptime = 0;
    switch(input->pattern){
        case FULLTEXT:
            exptime = pconf.fulltext_expire_time;
            break;
        case PARAGRAPH:
            exptime = pconf.paragraph_expire_time;
            break;
        case SENTENCE:
            exptime = pconf.sentence_expire_time;
            break;
        default:
            exptime = input->expiretime;
    }
    if (keyIsPermanent) {
   	input->deadline = 0;
    }else {
    	input->deadline = timestamp + input->expiretime;
    }
    if (keyIsPermanent && existSameDoc) {//same doc with the permanent key 
	    UB_LOG_DEBUG("they are the same docs, don't need to insert");
	    return -1;
    }
    /*rpush data,key-valuelist,the same key need to merge*/
    /*the submit document,new key-value data*/
    int insval_num = 0;/*insert value number,if number = 0,return*/
    for(i = 0;i < rediskey.size();i ++)
    {
        vtmp.key.clear();
        vtmp.value.clear();       
        vtmp.key = rediskey.at(i);
        /*the hit simdis or simrate key-value data*/
        vector<simval>* simv = NULL;
        k = 0;
        /* APPEND node,hited key-value do not need to insert,
         * hitdelay is false,do not need to insert,
         * k=DISTANCE+5,skip for*/
        if(input->strategy == APPEND || input->hitdelay == false || keyIsPermanent) {	//if the key is permanent, do not delay value timestamp
            //UB_LOG_DEBUG("APPEND mode or hitdelay is false,hited k-v donot need to insert");
            k = DISTANCE + 5;
        }
	 UB_LOG_DEBUG("for every rediskey[%d]",i);
 	for(;k <= DISTANCE;k ++)
        {
            if(output->counts[k] <= 0)
                continue;
            if(k == DISTANCE)/*simrate array*/
                simv = &(output->simrate);
            else/*simdis array*/
                simv = &(output->simdis[k]);
            for(j = 0;j < simv->size();j ++)
            {
                if(vtmp.key == simv->at(j).key){
                    val.simid = simv->at(j).val.simid;
                    val.docid = simv->at(j).val.docid;
		    if (keyIsPermanent) {
			val.timestamp = 0;
		    } else {
     			val.timestamp  = timestamp + exptime;
    		    }
                    val.datasize = simv->at(j).val.datasize;
                    if(val.datasize > 0 )
                        memcpy(val.userdata,simv->at(j).val.userdata,val.datasize);
                    vtmp.value.push_back(val);
                    insval_num ++;
                }
            }
        }
        /*the submit document,new key-value data*/
        /*compare mode,new key-value do not need to insert*/
        if(input->strategy == COMPARE)
        {
            UB_LOG_DEBUG("COMPARE mode,new k-v donot need to insert");
            kv.push_back(vtmp);
            continue;
        }
        val.simid = sign;
        val.docid = input->docid;
     	if (keyIsPermanent) {
		val.timestamp = 0;
	} else {
     		val.timestamp  = timestamp + input->expiretime;
	}
        val.datasize = input->raw_size;
        if(val.datasize > 0 )
            memcpy(val.userdata,input->privatedata,val.datasize);
        vtmp.value.push_back(val);
        insval_num ++;
        kv.push_back(vtmp);
    }

    if(kv.size() <= 0 || insval_num <= 0){
        UB_LOG_DEBUG("RPUSH,no k-v need to insert");
        return 0;
    }

    /*print data for debug*/
    for(i = 0;i < kv.size();i ++)
    {
        UB_LOG_DEBUG("RPUSH,key[%s] has [%lu]values need to insert",
            kv.at(i).key.c_str(),kv.at(i).value.size());
    }
    
    /*rpush request data pack*/
    if(rpush_msg_pack(&req_head,&req_buf,kv) != 0){
        return -1;
    }

    /*redis cmd send,response data is in res_buf*/
    if(redis_cmd_send(input->pid.c_str(),&req_head,&req_buf,&res_head,&res_buf)){
        return -1;
    }

    /*redis result check*/
    if(redis_other_cmd_unpack(&res_buf)){
        UB_LOG_WARNING("redis cmd RPUSH execute failed");
        return -1;
    }
    if((input->strategy == APPEND || input->strategy == APPCOM) && !keyIsPermanent){	    /*judge heather the key is need to be set expire time*/
	if(redis_expire_key(input,rediskey,logid) != 0){
		UB_LOG_WARNING("redis cmd EXPIRE execute failed");
	}
    }

    return 0;
}


/*
 *the document delete
 */
int redis_doc_del(sim_input_t *input,vector<string> & rediskey,u_int logid)
{
    if(!input){
        UB_LOG_WARNING("parameter error");
        return -1;
    }
    if(rediskey.size() <= 0){
        UB_LOG_WARNING("doc del,but redis key size less than 0");
        return -1;
    }
    
    int ret = 0;
	/*request buf*/
	nshead_t req_head;
	ub_buff_t req_buf;
	/*reponse buf*/
	nshead_t res_head;
	ub_buff_t res_buf;

    /*set logid and initialize*/
    nshead_copy(&req_head,logid);
    ret = get_ubclient_buf(&req_buf,&res_buf);
    if(ret != 0){
        UB_LOG_WARNING("get ub client buf failed");
        return -1;
    }

    /*construct key-value*/
    vector<lrem_t> kv;
    lrem_t vtmp;
    value_t *val = NULL;
    
    kv.clear();
    for(uint i = 0;i < rediskey.size();i ++){
        vtmp.key.clear();
        val = &(vtmp.value);
        vtmp.key  = rediskey.at(i);
        vtmp.count = 0;
        val->simid = input->sign;
        val->docid = input->docid;
        val->timestamp  = input->expiretime;
        val->datasize = input->raw_size;
        if(val->datasize > 0)
            memcpy(val->userdata,input->privatedata,input->raw_size);
        kv.push_back(vtmp);
    }
    
    /*rpush request data pack*/
    if(lrem_msg_pack(&req_head,&req_buf,kv) != 0){
        return -1;
    }

    /*redis cmd send,response data is in res_buf*/
    if(redis_cmd_send(input->pid.c_str(),&req_head,&req_buf,&res_head,&res_buf)){
        return -1;
    }

    /*redis result check*/
    if(redis_other_cmd_unpack(&res_buf)){
        UB_LOG_WARNING("redis cmd LREM execute failed");
        return -1;
    }

    return 0;
}


/*
 *the hit key-value is insert into the end of list,
 *need to delete the old key-value
 */
int redis_del_value(sim_input_t *input,sim_output_t* output,u_int logid)
{
    if(!input || !output){
        UB_LOG_WARNING("parameter error");
        return -1;
    }
    
    /*append mode,no hited k-v to del,return ahead*/
    if(input->strategy == APPEND || input->hitdelay == false){
        return 0;
    }

    int ret = 0;
	/*request buf*/
	nshead_t req_head;
	ub_buff_t req_buf;
	/*reponse buf*/
	nshead_t res_head;
	ub_buff_t res_buf;

    /*set logid and initialize*/
    nshead_copy(&req_head,logid);
    ret = get_ubclient_buf(&req_buf,&res_buf);
    if(ret != 0){
        UB_LOG_WARNING("get ub client buf failed");
        return -1;
    }

    /*construct key-value*/
    vector<lrem_t> kv;
    lrem_t vtmp;
    u_int i,j;
    value_t *val = NULL;

    kv.clear();
    /*the hit simdis key-value data*/
    vector<simval>* simv = NULL;
    for(i = 0;i < DISTANCE;i ++)
    {
        if(output->counts[i] <= 0)
            continue;
        simv = &(output->simdis[i]);
        for(j = 0;j < simv->size();j ++)
        {
            vtmp.key.clear();
            val = &(vtmp.value);
            vtmp.key  = simv->at(j).key;
            vtmp.count = 1;
            val->simid = simv->at(j).val.simid;
            val->docid = simv->at(j).val.docid;
            val->timestamp  = simv->at(j).val.timestamp;
            val->datasize = simv->at(j).val.datasize;
            if(val->datasize > 0 )
                memcpy(val->userdata,simv->at(j).val.userdata,val->datasize);
            kv.push_back(vtmp);
        }
    }

    /*the hit simrate key-value data,do not need temporarily*/
    if(output->counts[DISTANCE] > 0){
        ;//TODO
    }
    if(kv.size() <= 0){
        UB_LOG_DEBUG("redis no key-value need to delete");
        return 0;
    }
    /*rpush request data pack*/
    if(lrem_msg_pack(&req_head,&req_buf,kv) != 0){
        return -1;
    }

    /*redis cmd send,response data is in res_buf*/
    if(redis_cmd_send(input->pid.c_str(),&req_head,&req_buf,&res_head,&res_buf)){
        return -1;
    }

    /*redis result check*/
    if(redis_other_cmd_unpack(&res_buf)){
        UB_LOG_WARNING("redis cmd LREM execute failed");
        return -1;
    }

    return 0;
}


/*
 *set expire time to key
 */
int redis_expire_key(sim_input_t * input,vector<string>& rediskey,u_int logid)
{
    if(!input){
        UB_LOG_WARNING("parameter error");
        return -1;
    }

    u_int ksize = rediskey.size();
    if(ksize <= 0){
        UB_LOG_DEBUG("key num is zero,do not need to expire");
        return 0;
    }

    int ret = 0;
    vector<expire_t> expireval;
    expire_t tv;
    
    for(u_int i = 0;i < ksize;i ++)
    {
        tv.key = rediskey.at(i);
        tv.seconds = input->expiretime;
        expireval.push_back(tv);
    }

    if(expireval.size() <= 0){
        UB_LOG_DEBUG("key size is zero,do not need to set expire time");
        return 0;
    }

	/*request buf*/
	nshead_t req_head;
	ub_buff_t req_buf;
	/*reponse buf*/
	nshead_t res_head;
	ub_buff_t res_buf;

    /*set logid and initialize*/
    nshead_copy(&req_head,logid);
    ret = get_ubclient_buf(&req_buf,&res_buf);
    if(ret != 0){
        UB_LOG_WARNING("get ub client buf failed");
        return -1;
    }

    /*rpush request data pack*/
    if(expire_msg_pack(&req_head,&req_buf,expireval) != 0){
        return -1;
    }

    /*redis cmd send,response data is in res_buf*/
    if(redis_cmd_send(input->pid.c_str(),&req_head,&req_buf,&res_head,&res_buf)){
        return -1;
    }

    /*redis result check*/
    if(redis_other_cmd_unpack(&res_buf)){
        UB_LOG_WARNING("redis cmd LTRIM execute failed");
        return -1;
    }

    return 0;
}


/*
 *这仅仅是个ltrim操作，和下面的接口区别是：下面接口是ltrim+expire
 *这个接口使用于append操作
 */
int redis_ltrim_ahead(sim_input_t * input,vector<string>& rediskey,u_int logid)
{
    if(!input){
        UB_LOG_WARNING("parameter error");
        return -1;
    }

    u_int ksize = rediskey.size();
    if(ksize <= 0){
        UB_LOG_DEBUG("key num is zero,do not need to ltrim");
        return 0;
    }

    int ret = 0;
    proconf_t pconf;
    vector<ltrim_t> trimvalue;
    ltrim_t tv;

    g_proconf[g_index].get(input->pid,&pconf);
    uint32_t redis_max_list = pconf.max_redis_list;
    for(u_int i = 0;i < ksize;i ++)
    {
        tv.key = rediskey.at(i);
        tv.start = 0 - redis_max_list;
        if(tv.start >= 0){
            UB_LOG_WARNING("ltrim,but start position larger than zero");
            return -1;
        }
        trimvalue.push_back(tv);
    }

    if(trimvalue.size() <= 0){
        UB_LOG_DEBUG("redis expire or truncate check,no value need to delete");
        return 0;
    }

	/*request buf*/
	nshead_t req_head;
	ub_buff_t req_buf;
	/*reponse buf*/
	nshead_t res_head;
	ub_buff_t res_buf;

    /*set logid and initialize*/
    nshead_copy(&req_head,logid);
    ret = get_ubclient_buf(&req_buf,&res_buf);
    if(ret != 0){
        UB_LOG_WARNING("get ub client buf failed");
        return -1;
    }

    /*rpush request data pack*/
    if(ltrim_msg_pack(&req_head,&req_buf,trimvalue) != 0){
        return -1;
    }

    /*redis cmd send,response data is in res_buf*/
    if(redis_cmd_send(input->pid.c_str(),&req_head,&req_buf,&res_head,&res_buf)){
        return -1;
    }

    /*redis result check*/
    if(redis_other_cmd_unpack(&res_buf)){
        UB_LOG_WARNING("redis cmd LTRIM execute failed");
        return -1;
    }

    return 0;
}


/*
 *get rid of expired value
 *if number of value is more than threshold,truncate
 */
int redis_ltrim_value(sim_input_t * input,vector<rpush_t> & redisvalue,u_int logid)
{
    if(!input){
        UB_LOG_WARNING("parameter error");
        return -1;
    }

    if(input->strategy == APPEND){
        return 0;
    }

    u_int ksize = redisvalue.size();
    if(ksize <= 0){
        UB_LOG_DEBUG("key num is zero,do not need to truncate");
        return 0;
    }

    int ret = 0;
    int index = 0;
    proconf_t pconf;
    vector<ltrim_t> trimvalue;
    ltrim_t tv;
    rpush_t * val = NULL;
    time_t sec = time(NULL);
    uint32_t timestamp = sec;

    g_proconf[g_index].get(input->pid,&pconf);
    uint32_t redis_max_list = pconf.max_redis_list;
   
    bool truncate;/*if value is up to boundary,truncate is ture*/ 
    for(u_int i = 0;i < ksize;i ++)
    {
        truncate = false;
        index = 0;
        u_int st = 0;
        val = &(redisvalue.at(i));

        if(val->value.size() <= 0)
            continue;

        /*truncate*/
        if(redis_max_list > 0 && val->value.size() > redis_max_list){
            index = 0 - redis_max_list;/*reversed order to truncate*/
            st = val->value.size() - redis_max_list;
            truncate = true;
        }

        /*del expire value*/
        if(val->value.size() >= simconf.expire_value_check_num){
            for(;st < val->value.size();st ++)
            {
                if(val->value.at(st).timestamp <= timestamp && val->value.at(st).timestamp != 0) //if timestamp is 0, the value belongs to permanent key	
                    index++;
                else
                    break;
            }
            //all truncate
            if(truncate && index >= 0)
                index = val->value.size();
        }

        /*index is still original value,do not to ltrim*/
        if(index == 0)
            continue;
        /*reversed order ltrim*/
        if(index < 0)
            UB_LOG_WARNING("%s has %d value need to truncate",val->key.c_str(),
                (int)(val->value.size()) + index);
        /*order ltrim*/
        else if(index < (int)(val->value.size()))
        {
            UB_LOG_WARNING("%s has %d value expired,need to truncate",val->key.c_str(),index);
            index = index - val->value.size();
        }
        else
            UB_LOG_WARNING("%s all value[%lu] need to truncate",val->key.c_str(),val->value.size());
        tv.key = val->key;
        tv.start = index;
        trimvalue.push_back(tv);
    }

    if(trimvalue.size() <= 0){
        UB_LOG_DEBUG("redis expire or truncate check,no value need to delete");
        return 0;
    }

	/*request buf*/
	nshead_t req_head;
	ub_buff_t req_buf;
	/*reponse buf*/
	nshead_t res_head;
	ub_buff_t res_buf;

    /*set logid and initialize*/
    nshead_copy(&req_head,logid);
    ret = get_ubclient_buf(&req_buf,&res_buf);
    if(ret != 0){
        UB_LOG_WARNING("get ub client buf failed");
        return -1;
    }

    /*rpush request data pack*/
    if(ltrim_msg_pack(&req_head,&req_buf,trimvalue) != 0){
        return -1;
    }

    /*redis cmd send,response data is in res_buf*/
    if(redis_cmd_send(input->pid.c_str(),&req_head,&req_buf,&res_head,&res_buf)){
        return -1;
    }

    /*redis result check*/
    if(redis_other_cmd_unpack(&res_buf)){
        UB_LOG_WARNING("redis cmd LTRIM execute failed");
        return -1;
    }

    return 0;
}
