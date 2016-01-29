/*************************************************
 *
 * Author: lichao
 * Create time: 2014  2月 26 18时45分52秒
 * E-Mail: lichaotx020@sogou-inc.com
 * version 1.0
 *
*************************************************/


#ifndef PUBLIC_DEF_H_
#define PUBLIC_DEF_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>



// load index 锁
#define LOAD_LOCK()  pthread_mutex_lock(&m_load_mutex);       \
						LOG(INFO) << "LOAD LOCK!";   \
						m_allow_search = false;             \
					    while(atomic_read(&m_cur_search_num) > 0){  \
							usleep(5000);    \
						}    


#define LOAD_UNLOCK()  m_allow_search = true; \
						pthread_mutex_unlock(&m_load_mutex); \
						LOG(INFO) << "LOAD UNLOCK!"; 

// return status
enum ReturnStatus
{
	kFailFunc = -1,
	kSuccessFunc = 0
};

#define kMaxWordNum 64			// 源词的最大个数
#define kMaxOneHeapSize 16		// 标记堆的大小， 用来现在搜索空间
#define kMaxHeapNum 16			// 堆的个数， 也即原词的个数
#define kMaxPhraseLen 128
#define kMaxCandidateNum 8
#define kMaxCandidateNumPair 100
#define kMaxSenLen 256
#define kMaxWordLen 50
#define kMaxStepLen 8			// 遍历原串时最大看的phrase个数
#define kNeedNum 8			// 需要得到的候选结果数
#define kLmNeedMaxLevel 3		// 语言模型
#define kDumb 10000000
#define kMmap 0
#define kMinTransScore -30
#define kNoTransScore 0.0 
#define kMaxSubBaseInfoComp 5
#define kMaxNgramInfo 14

#define kDefaultPostMinScore (0.000001)
#define kMinPostFilterScore (0.001)
#define kLmDiffFinalFilter 20.0

#define kMultiBoost (100.0)

struct ResultNode
{
	std::string src;	// 源短语
	std::string dest; // 目标短语
	float cost;
	float lm_cost;
	int32_t qc_num;
	int32_t syn_type;
	int32_t qr_type;
	int32_t vr_type;

	int32_t num_replace;		// 发生替换的次数
	std::string word_before;	// 到当前节点之前的词
	float trans_score;
	ResultNode(
			std::string init_src,
			std::string init_dest,
			float init_cost,
			float init_lm_cost,
			int32_t init_qc_num,
			int32_t init_syn_type,
			int32_t init_qr_type,
			int32_t init_vr_type,
			int32_t init_num_replace,
			std::string init_word_before,
			float init_trans_score)
	{
		src = init_src;
		dest = init_dest;
		cost = init_cost;
		lm_cost = init_lm_cost;
		qc_num = init_qc_num;
		syn_type = init_syn_type;
		qr_type = init_qr_type;
		vr_type = init_vr_type;
		num_replace = init_num_replace;
		word_before = init_word_before;
		trans_score = init_trans_score;
	}
};

enum ReplaceTypeTag
{
	kReplaceTag = 0, // 可以替换
	kNotReplaceTag = 1, // 不可以替换
	kTailReplaceTag = 2, // 和前面集合可以替换，替换边界
};
struct PsmtInputNode
{
	PsmtInputNode()
	{
	}
	PsmtInputNode(
			const std::string & init_word,
			ReplaceTypeTag init_replace_tag,
			int32_t init_word_type,
			int32_t init_refer_index)
	{
		word = init_word;
		replace_tag = init_replace_tag;
		word_type = init_word_type;
		refer_index = init_refer_index;
	}
	std::string word;
	// 0 可以替换
	// 1 不可替换
	// 2 可以替换，但是需要和前面结合
	ReplaceTypeTag replace_tag;	// 是否能够替换
	int32_t word_type;		// 代表词的类型
	int32_t refer_index;	// 结合替换的索引
};

struct PsmtInput
{
	std::vector<PsmtInputNode> psmt_inputs;
};

struct PsmtResult
{
	std::string src_query;
	std::string dest_query;
	float cost;
	float lm_cost;
	float num_replace;
	std::vector<ResultNode> result_nodes;
};

struct StNodeSubBaseComp
{
	StNodeSubBaseComp(const std::string & init_src,
			const std::string & init_dest)
	{
		src = init_src;
		dest = init_dest;
	}
	std::string src;
	std::string dest;
};

struct PsmtOutput
{
	PsmtOutput()
	{
		psmt_outputs.clear();
		index_no_replace = -1;
	}
	std::vector<PsmtResult> psmt_outputs;
	int32_t index_no_replace;
};


struct QueryFormat
{
	std::string query_ori;	// ori query
	std::string query_norm;	// 归一化
	std::string query_ori_seg;	// 带空格的seg
	std::string query_wc;		// 全角query
	std::vector<std::string> query_seg_ori;
	std::vector<std::string> query_seg;
	PsmtInput psmt_input;
};

enum RewriteResultType
{
	kNull = 0,
	kCorrect = 1,
	kSyn = 2
};



struct RewriteResultNode
{
	RewriteResultNode()
	{
		result_str = "";
		result_seg.clear();
	}
	std::string result_str;
	RewriteResultType result_type;
	std::vector<std::string> result_seg;
};

struct TermNodeQR
{
	TermNodeQR(
			const std::string & init_src,
			const std::string & init_dest,
			int32_t init_qc_num,
			int32_t init_syn_type,
			int32_t init_qr_type,
			int32_t init_vr_type
		)
	{
		src = init_src;
		dest = init_dest;
		qc_num = init_qc_num;
		syn_type = init_syn_type;
		qr_type = init_qr_type;
		vr_type = init_vr_type;
	}
	std::string src;
	std::string dest;
	int32_t qc_num;
	int32_t syn_type;
	int32_t qr_type;
	int32_t vr_type;
};


struct RewriteFinalNode
{
	RewriteFinalNode(
			const std::string & init_dest_query,
			const std::string & init_replace_src,
			const std::string & init_replace_dest,
			float init_cost,
			float init_lm_cost,
			const std::vector<ResultNode> & init_terms,
			int32_t init_level,
			int32_t init_type)
	{
		dest_query = init_dest_query;
		replace_src = init_replace_src;
		replace_dest = init_replace_dest;
		cost = init_cost;
		lm_cost = init_lm_cost;
		terms = init_terms;
		level = init_level;
		type = init_type;
	}
	std::string dest_query;
	std::string replace_src;
	std::string replace_dest;
	float cost;
	float lm_cost;
	std::vector<ResultNode> terms;
	int32_t level;
	int32_t type;
};

struct RewriteResult
{
	QueryFormat query_format;
	PsmtOutput psmt_output;
	PsmtOutput psmt_output_filter;
	std::vector<RewriteResultNode> rewrite_results;
	std::vector<RewriteFinalNode> rewrite_final_nodes;
};



#endif // end define PUBLIC_DEF_H_
