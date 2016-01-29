#ifndef DA_SEARCH_H__
#define DA_SEARCH_H__

#include "Index_da.h"
#include "normalize.h"
#include "general.h"
#include "../segment/Segment.h"

using namespace std;
class DA_SEARCH{
	private:
		Normal Norm;
	public:
		DA_SEARCH();
		~DA_SEARCH();

		int32_t Init(const char* dir_base);
		int32_t Release();
		int32_t Release_();
		int32_t LoadDAs(const char* KEY_ANS_INDEX_FILE);
		int32_t DA_Search(string& dest_norm, map<string, string>& index_text, string& search_key);
		int32_t AbstractInfo(string values, string& singer, string& song, string& album);
		int32_t GetIndexResults(
			string& query,
			map<string, string>& index_text,
			vector<string>& seg_parts,
			SEGMENT_1 *seg);


	public:
		Darts::DoubleArray* DaSearch;

		char* Search_Buf;
		uint32_t Search_Buf_begin_end_len;
};


#endif
