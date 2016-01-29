/**
 * @file hashsign.cpp
 * @date 2012/09/28
 * @brief 
 * calculate 64 bit signature by a series of features
 */

#include <string.h>
#include "hashsign.h"
// #include "ul_sign.h"

/*
 *if bit=1 add weight
 *if bit=0 reduce weight
 */
static inline void update_hist(float *hist,uint64_t token,float weight)
{
	int i;
	for(i = 0;i < 64;i ++)
	{
		if(token & 1)
			*hist += weight;
		else
			*hist -= weight;
		hist++;
		token >>= 1;
	}
	return;
}


/*
 * * Calculates the similarity hash
 * return :64 bit signature
 */
uint64_t simhash_sign(vector<funit>& features)
{
	uint64_t sign = 0;/*64 bit signature*/
	uint64_t token = 0;/*64 bit hash tmp value*/
	float hist[64];
	memset(hist,0,sizeof(hist));
	int slen = 0;/*feature string length*/
	char *str = NULL;
	funit tmp;
	int i;/*for index*/
	unsigned int *sp;

	/*get each feature 64 bit hash*/
	for(i = 0;i < (int)features.size();i++)
	{
		tmp = features.at(i);
		str = (char *)tmp.feature.c_str();
		slen = strlen(str);
		if(slen <= 0)
			continue;
		sp = (unsigned int *)&token;
		if(creat_sign_mds64(str,slen,sp,sp+1) == -1){
			//UB_LOG_WARNING("creat_sign_f64 fail");
			continue;
		}

		//UB_LOG_DEBUG("[%s] 64 bit hash [%016lx]",str,token);
		update_hist(hist,token,tmp.weight);
	}
	
	/*get 64 bit simhash*/
	char binsig[66];/*this is just for debug*/
	for(i = 0;i < 64;i ++)
	{
		if(hist[i] > 0)
			binsig[63-i] = '1';
		else
			binsig[63-i] = '0';
		sign |= (uint64_t)(hist[i] > 0 ? 1 :0) << i;
	}
	binsig[64] = '\0';
	//UB_LOG_DEBUG("64 bit simhash [%s]",binsig);
	return sign;
}

//static inline int hamming_dist( uint64_t t1,uint64_t t2)
int hamming_dist( uint64_t t1,uint64_t t2)
{
	 int dist = 0;
	 uint64_t val = t1 ^ t2;
	 
	 // Count the number of set bits
	 while(val)
	 {
		 ++dist; 
		 val &= val - 1;
	 }
	 return dist;
}

/*the same docid do not participate in hamming dis calculate,
 *need to replace,but this not realize now
 */
int get_similar_doc(vector<rpush_t>& redisvalue,uint64_t sign,sim_output_t* output,sim_input_t * input,bool &existSameDoc)
{
    if(!output){
        return -1;
    }
    /*append,do not need to compare*/
    if(input->strategy == APPEND){
        return 0;
    }
    int d;
    rpush_t * val = NULL;
    simval tmp;
    time_t sec = time(NULL);
    uint32_t timestamp = sec;
    if(input->pattern == FULLTEXT){
        memset(output->counts,0,sizeof(output->counts));
        for(uint i = 0;i < redisvalue.size();i ++)
        {
            val = &(redisvalue.at(i));
            for(uint j = 0;j < val->value.size();j ++)
            {
                /*the same docid,ignore*/
                if(input->docid == val->value.at(j).docid) {
		    existSameDoc = true;
                    continue;
		}
                /*the value is expired*/
                if(val->value.at(j).timestamp <= timestamp && val->value.at(j).timestamp != 0)// if timestamp = 0    it belongs to permanent key
                    continue;
                d = hamming_dist(val->value.at(j).simid,sign);
                if(d <= input->hamdis){
                    memset(&(tmp.val),0,sizeof(tmp.val));
                    tmp.rate = 0.0;
                    tmp.key.clear();
                    tmp.key = val->key;
                    tmp.val.docid = val->value.at(j).docid;
                    tmp.val.simid = val->value.at(j).simid;
                    tmp.val.datasize = val->value.at(j).datasize;
                    tmp.val.timestamp = val->value.at(j).timestamp;
                    if(tmp.val.datasize > 0)
                        memcpy(tmp.val.userdata,val->value.at(j).userdata,tmp.val.datasize);
                    output->simdis[d].push_back(tmp);
                    output->counts[d] ++;
                }
            }
        }
    }
    else if(input->pattern == PARAGRAPH){
    }
    else if (input->pattern == SENTENCE){
    }
    else{
        //UB_LOG_WARNING("pattren error [%d]",input->pattern);
        return -1;
    }
    return 0;
}


