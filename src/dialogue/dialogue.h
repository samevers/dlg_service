#ifndef DIALOGUE_H_
#define DIALOGUE_H_

#include "general.h"
#include "../segment/Segment.h"
#include "Index_da.h"
#include "../index/inv_index/doc_index.h"
#include "Doc_Search.h"
#include "Da_Search.h"

using namespace WHITE_BLACK_DA;

class DIALOGUE{
public:
	DIALOGUE();
	~DIALOGUE();

	int Init(const char* dir_base);
	int Release();
	int Normalize_(const char* query);
	int IndexOutcome(const char* query);//, std::ofstream& FILE_TEACHER);
	int MakeResponse(const char* query, const char* anwser);
	int show_results(size_t query_length, map<int, string>& da_vec);
public:
	SEGMENT_1 * seg;
private:
	//CDlgIndex doc_search;
	DOC_SEARCH doc_search;
	DA_SEARCH *da_index;
	int loadDic(const char* dir_base);
	std::map<std::string, int> Hash_punc;
};



#endif
