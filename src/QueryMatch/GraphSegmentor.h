#ifndef _GRAPHSEGMENTOR_H_
#define _GRAPHSEGMENTOR_H_

#include "Platform/encoding.h"
#include "Platform/encoding/EncodingConvertor.h"
#include "WordSegmentor4/Segmentor/Graph.h"
#include "Platform/bchar.h"
#include <iostream>
#include "iconv.h"

class GraphSegmentor {
private:
	static GraphSegmentor *ddinstance;
	EncodingConvertor* ec;
	analysis::DicTree* dictree;

public:
	static GraphSegmentor* getInstance();
	static void releaseInstance();
    GraphSegmentor();
    ~GraphSegmentor();

	void gbk_utf16le(char* inbuf, size_t inlen, char* outbuf);
	void utf16le_gbk(char* inbuf, size_t inlen, char* outbuf);
	int Segment(analysis::TGraph* segmentor, const char* Query, analysis::ResultInfo* Info, int Length, char* Query_utf16);
	std::string GetTermText(char* Query_utf16, analysis::ResultInfo* Info, const int Num);	

};

#endif
