/***************************************************************************
 * 
 * Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
 *  
 ***************************************************************************/
/**
 * @file sim_process.h
 * @author lizhanping(lizhanping01@baidu.com)
 * @date 2012/09/26
 * @brief 
 * simhash control module,call other functions to 
 * complete task
 */

#include "sim_process.h"
#include "sim_struct.h"
#include "msgpack.h"
#include "segmentor.h"
#include "hashsign.h"
#include "redis.h"
#include "sim_conf.h"
#include "normalize.h"

pthread_key_t key;
pthread_once_t once_mark = PTHREAD_ONCE_INIT;


static void sim_output_init(sim_output_t * output)
{
    for(int i = 0;i < DISTANCE;i ++)
        output->simdis[i].clear();
    output->simrate.clear();
    return;
}

static int
doc_delete(sim_input_t *input,vector<string> & rediskey,u_int logid)
{
    if(!input){
        UB_LOG_WARNING("parameter error");
        return -1;
    }
    make_rediskey(input->key.c_str(),input->pattern,input->sign,rediskey);
    int ret = redis_doc_del(input,rediskey,logid);
    return ret;
}
static int
sim_process(nshead_t *req_head,ub_buff_t *req_buf,nshead_t *res_head,ub_buff_t *res_buf)
{
	int res = 0;	/*function call result*/
	int ret = -1;	/*return value*/
	sim_input_t input; /*store request msg*/
	sim_output_t output;/*store response msg*/
	/*resolve request msg to input*/
	output.err_no = 0;
    u_int logid = req_head->log_id;
    vector<string> rediskey;/*store redis key*/
    vector<rpush_t> redisvalue;/*store LRANGE value from redis*/

    struct timeval e,s;
    time_t s1,s2,s3,s4,s5,s6;
    bool existSameDoc = false;
    s1 = s2 = s3 = s4 = s5 = s6 = 0;

    /*clear output 's vector*/
    sim_output_init(&output);

	res = reqmsg_unpack(req_head,req_buf,&input,output.err_msg);
	if(res != 0){
		/*request msg unpack fail*/
		output.err_no = -1;
		UB_LOG_WARNING("request msg unpack failed");
		goto out;
	}
    
    /*doc delete*/
    if(input.strategy == DELETE){
        input.deadline = 0;
        if(doc_delete(&input,rediskey,logid) < 0){
            output.err_no = -1;
            output.err_msg = "doc delete failed";
        }
        else
            ret = 0;
        goto out;
    }

	/*word segment*/
    gettimeofday(&s,NULL);
	if(input.features.size() <= 0){

        /////////////////////////////////////////////////////////
        /*tieba hard code*/
        if(input.pid == "forum"){
            int bufsize = input.content.length() + 5;
            char * destbuff = (char *)malloc(bufsize);
            if(!destbuff){
                output.err_no = -1;
                output.err_msg = "inner server error(malloc)";
                goto out;
            }
            int asize = text_normalize(input.content.c_str(),destbuff,bufsize);
            if(asize <= 0 || asize > bufsize){
                output.err_no = 0;
                output.err_msg = "normalize failed,or text size is zero after normalize";
                UB_LOG_WARNING("%s[buf=%s, bufseize=%d, asize=%d]",output.err_msg.c_str(), destbuff, bufsize, asize);
                free(destbuff);
                goto out;
            }
            destbuff[asize] = '\0';
            UB_LOG_DEBUG("after normalize:%s",destbuff);
            res = segment_words(destbuff,input.weight,asize,input.stepword,input.features);
            free(destbuff);
        }
        /////////////////////////////////////////////////////////
        else
		    res = segment_words(input.content.c_str(),input.weight,input.content.length(),input.stepword,input.features);
		if(res != 0){
			output.err_no = -1;
			output.err_msg = "inner server error(word segment error)";
			goto out;
		}
	}
    gettimeofday(&e,NULL);
    s1 = TIMEDIFF(s,e);

	/*calculat 64 bit simhash signature*/
	uint64_t sign = simhash_sign(input.features);
    input.sign = sign;
    if(sign == 0){
        output.err_msg = "64 bit simhash sign is zero";
        output.err_no = 0;
        goto out;
    }

    /*make redis key by 64bit simhash sign*/
    make_rediskey(input.key.c_str(),input.pattern,sign,rediskey);

    /*get values from redis by keys*/
    gettimeofday(&s,NULL);
    if(redis_get_value(&input,rediskey,redisvalue,logid) != 0){
        output.err_no = -1;
        output.err_msg = "redis get value by key failed";
        goto out;
    }
    gettimeofday(&e,NULL);
    s2 = TIMEDIFF(s,e);

    /*get similar documet,result is stored into ouput*/
    if(get_similar_doc(redisvalue,sign,&output,&input,existSameDoc) != 0){
        output.err_no = -1;
        output.err_msg = "get similar document error";
        goto out;
    }
    gettimeofday(&s,NULL);
    s3 = TIMEDIFF(e,s);

    /*the hit key-value is insert into the end of list,
     *need to delete the old key-value
     */
    res = redis_del_value(&input,&output,logid);
    gettimeofday(&e,NULL);
    s4 = TIMEDIFF(s,e);

    /*rpush,insert new and hited key-value into redis,
     *and set new expire time to key
     */
    if(redis_insert_value(rediskey,sign,&input,&output,logid,res,existSameDoc) != 0){
        ;
    }
    gettimeofday(&s,NULL);
    s5 = TIMEDIFF(e,s);


    /*truncate and get rid of expired value*/
    redis_ltrim_value(&input,redisvalue,logid);
    gettimeofday(&e,NULL);
    s6 = TIMEDIFF(s,e);

	ret = 0;
out:
	/*result msg pack to res_buf*/
	res = resmsg_pack(res_head,res_buf,&output,&input,logid);
	if(res !=0){
		/*pack fail*/
		UB_LOG_WARNING("result pack to res_buf failed");
	}
    UB_LOG_NOTICE("pid[%s] expire[%u] docid[%lld] wordseg[%lu] LRANGE[%lu] simdoc[%lu] LREM[%lu] RPUSH[%lu] \
        LTRIM[%lu]",input.pid.c_str(),input.deadline,input.docid,s1,s2,s3,s4,s5,s6);
	return ret;
}

/*
 *init thread specific data
 */
static void tsd_free_callback(void *p)
{
    thread_data * td = (thread_data *)p;
    if(td){
        if(td->redis_req_buf)
            free(td->redis_req_buf);
        if(td->redis_res_buf)
            free(td->redis_res_buf);
        if(td->pout)
            scw_destroy_out(td->pout);
        free(td);
    }
}

static void init_once(void)
{
    pthread_key_create(&key,tsd_free_callback);
}

static int init_thread_data()
{
    pthread_once(&once_mark,init_once);
    if(pthread_getspecific(key) == NULL){
        UB_LOG_DEBUG("init thread[%lu] memory",pthread_self());
        thread_data * td = (thread_data *)malloc(sizeof(thread_data));
        if(!td){
            UB_LOG_FATAL("thread specific data,malloc failed");
            exit(0);
        }
        td->reqbuf_size = simconf.redis_write_mcpack_size;
        td->resbuf_size = simconf.redis_read_mcpack_size;
        td->gbk2utf_tmpbuf_size = simconf.gbk2utf_tmpbuf_size;
        
        td->redis_req_buf = (char *)malloc(td->reqbuf_size);
        td->redis_res_buf = (char *)malloc(td->resbuf_size);
        td->gbk2utf_tmpbuf = (char *)malloc(td->gbk2utf_tmpbuf_size);
        
        if(!td->redis_req_buf || !td->redis_res_buf || !td->gbk2utf_tmpbuf){
            UB_LOG_FATAL("thread specific data,malloc failed");
            exit(0);
        }
        td->pout = NULL;
        if(segment_memory_init(&(td->pout)) != 0){
            UB_LOG_FATAL("word segment result buffer init failed");
            exit(0);
        }
        if(pthread_setspecific(key,(void *)td) != 0){
            UB_LOG_FATAL("pthread set dpecific key failed");
            exit(0);
        }
    }
    return 0;
}

/**
 *@simhash server callback
**/
int 
simhash_server_callback()
{
	nshead_t *req_head = (nshead_t *) ub_server_get_read_buf();
	nshead_t *res_head = (nshead_t *) ub_server_get_write_buf();
	
	int cmd_no = 0;

	if(req_head == NULL || res_head == NULL)
	{
		UB_LOG_WARNING("simhash callback get nshead failed");
		return -1;
	}
	
	ub_buff_t req_buf;
	ub_buff_t res_buf;
	req_buf.buf = (char *)(req_head+1);
	req_buf.size = ub_server_get_read_size() - sizeof(nshead_t);
	res_buf.buf = (char *)(res_head+1);
	res_buf.size = ub_server_get_write_size() - sizeof(nshead_t);
	
	ub_log_setbasic(UB_LOG_REQSVR, "%s", req_head->provider);
	ub_log_setbasic(UB_LOG_SVRNAME, "%s", "simhash");
	ub_log_setbasic(UB_LOG_CMDNO, "%d", cmd_no);

    if(init_thread_data() != 0){
        UB_LOG_WARNING("init thread specific data failed");
        return -1;
    }

	/*similar document identification*/
	sim_process(req_head,&req_buf,res_head,&res_buf);

	return 0;
}

