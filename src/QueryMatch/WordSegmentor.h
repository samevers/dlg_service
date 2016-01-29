#ifndef _WORDSEGMENTOR_H_
#define _WORDSEGMENTOR_H_

#include "Platform/encoding.h"
#include "Platform/encoding/EncodingConvertor.h"
#include "Platform/gchar.h"
#include "WordSegmentor4/IQSegmentor/QuerySegmentor.h"
#include "Platform/encoding/support.h"
#include "Platform/encoding/EncodingConvertor.h"

class WordSegmentor {
private:
	static WordSegmentor *ddinstance;
	EncodingConvertor* ec;
        analysis::DicTree* dictree;
        analysis::IQDic* iqdic;

public:
	static WordSegmentor* getInstance();
	static void releaseInstance();
        WordSegmentor();
        ~WordSegmentor();

        int Segment(analysis::QuerySegmentor *Wordseg, const char* Query, analysis::WordSegmentorInfo& Request, analysis::WordSegmentorInfo& Parsed, std::vector<std::string>& TermList);

private:
	std::string GetTermText(const analysis::WordSegmentorInfo& Request,const analysis::WordSegmentorInfo& Parsed,int Num);
	void gbk_utf16le(char* inbuf, size_t inlen, char* outbuf) const;
	void utf16le_gbk(char* inbuf, size_t inlen, char* outbuf) const;
	bool haveSecond(uint32_t termID);
	bool isSecondTerm(uint32_t termID);	

};

#endif
