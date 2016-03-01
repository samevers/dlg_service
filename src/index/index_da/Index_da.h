#ifndef TRANSLATION_TABLE_H_
#define TRANSLATION_TABLE_H_

#include "darts.h"
#include "darts-clone.h"
#include "general.h"
#include "../../segment/Segment.h"
namespace WHITE_BLACK_DA{

	
struct ST{

	std::string query;
	std::string extend;
	
	ST(){}
	ST(std::string q,std::string e)
	{
		query = q;
		extend = e;
	}
};

inline bool comp(ST v1, ST v2)
{
	return v1.query < v2.query;
}


struct BEGIN_END
{
	uint32_t begin;
	uint32_t end;
	
	BEGIN_END(){}
	BEGIN_END(int64_t begin_, int64_t end_)
	{
		begin = begin_;
		end = end_;
	}
};

// sam
struct Pos_Reform
{
	std::string vr;
	std::string label;
	std::string pos_type;	// whole,part,pre,sub
	std::string reform; 	// reform content
	std::string item_query; // item query
	std::string data;		// data
	std::string iswappc;			// is wap or pc
	int query_len; // 

	Pos_Reform(){}
	Pos_Reform(const char*  vr_, const char* label_, const char* pos_type_, const char* reform_, const char* itemquery, const char* data_, const char* iswappc_,int query_len_) 
	{
		vr = vr_;
		label = label_;
		pos_type = pos_type_;
		reform = reform_;
		item_query = itemquery;
		data = data_;
		iswappc = iswappc_;
		query_len = query_len_;
	}
};

// sam
struct WhiteBlackList_TMP
{
	std::string query;
	std::vector<Pos_Reform> vr_pos_reform;

	bool operator < (const WhiteBlackList_TMP &other) const
	{
		return query.compare(other.query) < 0;
	}
};

inline bool comp2(WhiteBlackList_TMP v1, WhiteBlackList_TMP v2)
{
	return v1.query < v2.query;
}

int DeleteSpace(std::string & input,
		std::string & output, char del = ' ');

		// White_Black List
class WhiteBlackList_ 
{
public:

	WhiteBlackList_();
	~WhiteBlackList_();

	// 初始化
	int32_t Init(const char *lm_index_file, 
			const char *cp_index_file);

	// 释放资源
	int32_t Release();

	// 对原始数据建立索引
	int32_t MakeIndex(const char * file, 
			const char * index_file);
	// 读入索引
	int32_t LoadIndex(const char *index_file, 
				Darts::DoubleArray* &da_, 
				BEGIN_END * &begin_end_vr, 
				BEGIN_END * &begin_end_label, 
				BEGIN_END * &begin_end_type, 
				BEGIN_END * &begin_end_reform, 
				BEGIN_END * &begin_end_itemquery, 
				BEGIN_END * &begin_end_data, 
				BEGIN_END * &begin_end_iswappc, 
				uint32_t &begin_end_length, 
				char* &buf_vr, 
				char* &buf_label, 
				char* &buf_type, 
				char* &buf_reform, 
				char* &buf_itemquery, 
				char* &buf_data, 
				char* &buf_iswappc, 
				int* &buf_querylen, 
				uint32_t &m_len_, 
				uint32_t &buf_vr_len, 
				uint32_t &buf_label_len, 
				uint32_t &buf_type_len, 
				uint32_t &buf_reform_len,
				uint32_t &buf_itemquery_len, 
				uint32_t &buf_data_len, 
				uint32_t &buf_iswappc_len, 
				uint32_t &buf_querylen_len);
	int change2qj(std::string query, char* &buffer);
	int32_t MakeIndex(const char * file, 
			const char * index_file, const char* tag);
	int32_t OutputIndexFile(std::vector<ST> & vect,const char * index_file);
	int32_t OutputIndexFile(std::vector<ST> & vect,const char * index_file, std::string isUniq);
	int32_t OutputIndexFile_common(std::vector<ST> & vect,const char * index_file, std::string isUniq);
	int32_t LoadIndex(const char *index_file, 
									Darts::DoubleArray* &da_, 
									char* &buf_extend,
									uint32_t& buf_extend_len);

	// 清除索引
	int32_t ClearIndex();
	EncodingConvertor* ec;
private:

	// 输出index文件
	int32_t OutputIndexFile(
			std::vector<WhiteBlackList_TMP> & WhiteBlackList_vect,
				const char * index_file);

    Darts::DoubleArray white_black_da; 					 //	DA 词典
	Darts::DoubleArray extend_da; 					 //	DA 词典
	char * m_char_buf;									 //	存放buf 数据
    uint32_t m_len_char;
    uint32_t m_len_vr;
    uint32_t m_len_type;
    uint32_t m_len_reform;
    uint32_t m_len_struct;
	uint32_t begin_end_len;
	//TranslationOption * m_trans_option_buf;
	//int64_t m_num_trans_option;
	//PhraseTranslationCandidate * m_phrase_candidate_buf;
	//int64_t m_num_phrase_candidate;
	//static const float kMultiBoost = 100.0;

	SEGMENT_1* seg;	
};

};
#endif


