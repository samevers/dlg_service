/*************************************************
 * @yonghuahu
*************************************************/

#ifndef _DLG_INDEX_H
#define _DLG_INDEX_H

#include "common_index.h"
#include "../../segment/Segment.h"
#include "Index_da.h"
#include "normalize.h"

//Doc�ṹ
struct DLG_DOC_TYPE {
	int64_t word_offset; //doc��word buf�е�ƫ��
	float weight;   //���doc�ľ�̬Ȩ��

	DLG_DOC_TYPE & operator = (const DLG_DOC_TYPE &m) {
		word_offset = m.word_offset;
		weight = m.weight;
		return *this;
	}
};

//���Žṹ
struct DLG_INV_TYPE {
	int32_t occ;     //��ǰ����doc�г��ֵ�λ��
	int32_t occ_len; //��ǰ����doc�еĳ���
	int32_t occ_filed; //��ǰ���ڵ����г��ֵ���
					   //Ŀǰ�����������: 17.5Ӱ��(����ɵ�)
					   //�������Ӱ���� ֵΪ0��
					   //�������ֵΪ 1����������
					   //
	int32_t doc_id;    //�����������ָ��doc id

	DLG_INV_TYPE & operator = (const DLG_INV_TYPE &m) {
		occ = m.occ;
		occ_len = m.occ_len;
		occ_filed = m.occ_filed;
		doc_id = m.doc_id;
		return *this;
	}
};

//Key�ṹ
struct DLG_KEY_TYPE {

	int32_t begin; //�ڵ��ű��еĿ�ͷ ���
	int32_t end;   //�ڵ��ű��еĽ�β �ҿ�

	int64_t word_offset;  //key word��word buf�е�ƫ�� 

	DLG_KEY_TYPE & operator = (const DLG_KEY_TYPE &m) {
		begin = m.begin;
		end = m.end;
		word_offset = word_offset;
		return *this;
	}
};

//���������ĵ������ṹ
struct InvResult{
	DLG_INV_TYPE* inv;//��������ʼλ��
	int inv_num;//����������

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
	//���غ�׺�ʣ������ж�����
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
