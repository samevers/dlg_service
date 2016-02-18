/*************************************************
 * @yonghuahu
*************************************************/

#ifndef _DLG_INDEX_H
#define _DLG_INDEX_H

#include "common_index.h"
#include "../../segment/Segment.h"
#include "Index_da.h"
#include "normalize.h"

//Doc结构
struct DLG_DOC_TYPE {
	int64_t word_offset; //doc在word buf中的偏移
	float weight;   //这个doc的静态权重

	DLG_DOC_TYPE & operator = (const DLG_DOC_TYPE &m) {
		word_offset = m.word_offset;
		weight = m.weight;
		return *this;
	}
};

//倒排结构
struct DLG_INV_TYPE {
	int32_t occ;     //当前词在doc中出现的位置
	int32_t occ_len; //当前词在doc中的长度
	int32_t occ_filed; //当前词在倒排中出现的域
					   //目前考虑这种情况: 17.5影城(五棵松店)
					   //如果命中影城则 值为0，
					   //五棵松则值为 1，依次类推
					   //
	int32_t doc_id;    //这个倒排项所指的doc id

	DLG_INV_TYPE & operator = (const DLG_INV_TYPE &m) {
		occ = m.occ;
		occ_len = m.occ_len;
		occ_filed = m.occ_filed;
		doc_id = m.doc_id;
		return *this;
	}
};

//Key结构
struct DLG_KEY_TYPE {

	int32_t begin; //在倒排表中的开头 左闭
	int32_t end;   //在倒排表中的结尾 右开

	int64_t word_offset;  //key word在word buf中的偏移 

	DLG_KEY_TYPE & operator = (const DLG_KEY_TYPE &m) {
		begin = m.begin;
		end = m.end;
		word_offset = word_offset;
		return *this;
	}
};

//检索出来的倒排链结构
struct InvResult{
	DLG_INV_TYPE* inv;//倒排链起始位置
	int inv_num;//倒排链长度

	InvResult()
	{
		inv = NULL;
		inv_num = 0;
	}
};
struct InvNumInfo{
	int min_inv_size;// = INT_MAX;
	int max_inv_size; //= 0;
	int min_index; //= 0;
	int max_index; // = 0;

	InvNumInfo()
	{
		min_inv_size = INT_MAX;
		max_inv_size = 0;
		min_index = 0;
		max_index = 0;
	}
};

struct DlgResult{
	std::string query;
	//std::vector<std::string> anwser;
	std::vector<std::string> data;

	DlgResult()
	{
		query = "";
		data.clear();
		//anwser.clear();
	}

};

class CDlgIndex{
public:
	CDlgIndex();
	~CDlgIndex();
	
	int32_t Init(const char* dir_base);
	int32_t MakeIndex(
			const char* file_in,
			const char* file_out);
	int32_t LoadIndex(
			const char* file_index);
	int32_t LoadStopword(
			const char* file_stopword);
	int32_t LoadBlackword(
			const char* file_black);
	//加载后缀词，用于判断需求
	int32_t LoadPostfixWord(
			const char* file_postfix);
	int32_t Release();

	int32_t GetUnionResults(
			const std::string& query,
			std::vector<DLG_DOC_TYPE*>& docs,
			std::vector<std::string>& parts,
			std::string& query_reform);
	int32_t GetUnionResults(
			const std::string& query,
			std::vector<std::string>& docs_text,
			//std::vector<std::string>& ans_text,
			std::vector<std::string>& parts);
	int32_t GetDlgResult(
			const std::string& query,
			DlgResult& dlg_result,
			SEGMENT_1* seg);

	int32_t GraphSegment(
			analysis::TGraph& graph,
			const std::string& query,
			std::vector<std::string>& parts);
private:
	int32_t GetDlgResultLongQuery(
			const std::string& query,
			const std::vector<std::string>& docs,
			DlgResult& dlg_result);
	int32_t GetDlgResultPostfix(
			const std::string& query,
			const std::vector<std::string>& docs,
			const std::vector<std::string>& seg_parts,
			DlgResult& dlg_result);
private:
	int32_t GetInvResults(
			const std::vector<std::string>& parts,
			InvNumInfo& num_info,
			std::vector<InvResult>& inv_results
			);
	int32_t UnionInvResult(
			const std::vector<InvResult>& inv_results,
			const InvNumInfo& num_info,
			std::vector<DLG_DOC_TYPE*>& docs);
	int32_t SearchDoc(
			const int doc_id,
			const std::vector<InvResult>& inv_results,
			const int min_index,
			std::vector<int>& index_begins,
			std::vector<DLG_INV_TYPE*>& inv_vector);
	int32_t BinarySearch(
			const int doc_id,
			DLG_INV_TYPE* inv,
			int offset,
			int max_pos);

private:
	CommonIndex<DLG_DOC_TYPE,DLG_INV_TYPE,DLG_KEY_TYPE> m_doc_index;

	std::set<std::string> m_stopword;
	std::set<std::string> m_postfix_word;
	std::set<std::string> m_black;
	
	int32_t Release_();
	Normal Norm;
private:
	analysis::TGraph m_graph_segment;
};


#endif
