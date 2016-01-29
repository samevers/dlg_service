/*************************************************
 *
*************************************************/

#ifndef _DLG_LOG_INDEX_H
#define _DLG_LOG_INDEX_H

#include "../inv_index/doc_index.h"

struct DlgLogResult{
	std::string query;
	std::string area;
	std::string city;
	std::string device;
	int weight;

	DlgLogResult()
	{
		query = "";
		area = "";
		city = "";
		device = "";
		weight = 0;
	}

	bool operator < (const DlgLogResult& rp)const
	{//ÊµÏÖÄæÐòÅÅÐò
		return this->weight > rp.weight;
	}
};

class CDlgLogIndex{
public:
	CDlgLogIndex();
	~CDlgLogIndex();

	static int32_t Init();
	static int32_t MakeIndex(
			const char* file_in,
			const char* file_out);
	static int32_t LoadIndex(
			const char* file_index);
	static int32_t Release();

	int32_t GetUnionResults(
			const std::string& query,
			std::vector<DLG_DOC_TYPE*>& docs,
			std::vector<std::string>& seg_parts);
	int32_t GetUnionResults(
			const std::string& query,
			std::vector<DlgLogResult>& log_docs,
			std::vector<std::string>& seg_parts);


	static int32_t GraphSegment(
			analysis::TGraph& graph,
			const std::string& query,
			std::vector<std::string>& parts);
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
	static CommonIndex<DLG_DOC_TYPE,DLG_INV_TYPE,DLG_KEY_TYPE> m_dlg_index;

private:
	analysis::TGraph m_graph_segment;
};


#endif
