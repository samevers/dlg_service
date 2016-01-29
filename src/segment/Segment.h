/************************************************
 * @yonghuahu 									*
*************************************************/
#ifndef SEGMENT_HEAD_H__
#define SEGMENT_HEAD_H__

#include "general.h"
#include "gary_common/gary_string.h"
#include "string_utils.h"
#include "WordSegmentor4/Segmentor/Graph.h"
#include "Platform/bchar.h" 
#include "Platform/encoding.h" 
#include "Platform/encoding/support.h"
#include "Platform/encoding/EncodingConvertor.h"
#include "Platform/encoding/stringmatch/AhoCorasick.h"
#include "Platform/gchar.h"
#include "service_log.hpp"
#include <boost/algorithm/string.hpp>
#include "common_index.h"

class SEGMENT_1{
	private:
		analysis::TGraph m_graph_segment;
		static SEGMENT_1 *ddinstance;
	public:
		SEGMENT_1();
		~SEGMENT_1();
		static SEGMENT_1* getInstance();
		static void releaseInstance();
	public:
		int32_t Segment_(const std::string& query, std::vector<std::string>& parts);
		int32_t Init();
		int32_t Release();
};


#endif
