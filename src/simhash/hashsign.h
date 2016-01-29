/***************************************************************************
 * 
 *  
 ***************************************************************************/
/**
 * @file hashsign.h
 * @date 2012/09/28
 * @brief 
 * calculate 64 bit signature by a series of features
 */

#ifndef __HASHSIGN_H__
#define __HASHSIGN_H__

#include "sim_struct.h"
#include "sim_include.h"
#include "redis.h"

/*
 * * Calculates the similarity hash
 * return :64 bit signature
 */
uint64_t simhash_sign(vector<funit>& features);
/*sv add*/
int hamming_dist( uint64_t t1,uint64_t t2);

int get_similar_doc(vector<rpush_t>& redisvalue,uint64_t sign,sim_output_t* output,sim_input_t *input,bool &existSameDoc);

#endif
