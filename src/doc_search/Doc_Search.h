#ifndef DOC_SEARCH_H__
#define DOC_SEARCH_H__

#include "normalize.h"
#include "general.h"
#include "../segment/Segment.h"
#include "doc_index.h"

using namespace std;
class DOC_SEARCH{
	private:
		Normal Norm;
		CDlgIndex doc_index;
		//CDlgIndex query_doc_index;
	public:
		DOC_SEARCH();
		~DOC_SEARCH();

		int32_t Init(const char* dir_base);
		int32_t Release();
		int32_t LoadDOCs(const char* KEY_ANS_INDEX_FILE);
		int32_t DOC_Search(string& dest_norm, DlgResult& doc_result,SEGMENT_1* seg);
		int32_t Query_DOC_Search(string& dest_norm, DlgResult& doc_result);
		int32_t GetIndexResults(
			string& query,
			DlgResult& doc_result,
			SEGMENT_1* seg);


	public:

};


#endif
