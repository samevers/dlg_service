#ifndef DIALOGUE_H_
#define DIALOGUE_H_

#include "general.h"
#include "../segment/Segment.h"
#include "Index_da.h"
#include "../index/inv_index/doc_index.h"

using namespace WHITE_BLACK_DA;

class DIALOGUE{
public:
	DIALOGUE();
	~DIALOGUE();

	int Init(const char* dir_base);
	int Release();
	int Normalize_(const char* query);
	int IndexOutcome(const char* query);
	int MakeResponse(const char* query, const char* anwser);
private:
	CDlgIndex doc_index;
	int loadDic(const char* dir_base);
	std::map<std::string, int> Hash_punc;
};



#endif
