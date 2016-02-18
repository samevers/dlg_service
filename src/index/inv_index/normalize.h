#ifndef NORMALIZE_HEAD_H__
#define NORMALIZE_HEAD_H__

#include "general.h"
#include "Segment.h"

class Normal{
	private:
		std::map<std::string, std::vector<std::string> > hash_syn;
	public:
		Normal();
		~Normal();

		int32_t Init();
		int32_t Segment_(
				analysis::TGraph& graph,
				const std::string& query,
				std::vector<std::string>& parts);
	
		int32_t loadSyn();
		int32_t Normalize_(const std::string& line, std::string& segment, std::string& norm_,SEGMENT_1* seg);
		int32_t Normalize_seg(const std::string& line, std::string& norm_);
};

#endif
