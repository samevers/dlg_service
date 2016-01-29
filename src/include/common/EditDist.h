#ifndef EDIT_HEAD_H__
#define EDIT_HEAD_H__
#include "general.h"
#include "segment/Segment.h"

class EditDist{

	private:
	public:
		EditDist();
		~EditDist();
		int edit(std::vector<std::string>& str1, std::vector<std::string>& str2);
		int32_t Calculate(std::string str1, std::string str2, SEGMENT_1* seg);
		int32_t EditSort(std::string str1, std::vector<std::string>& cadidate, std::map<int, std::string>& cadidate_sort, std::vector<int>& sortED, SEGMENT_1* seg);
};


#endif
