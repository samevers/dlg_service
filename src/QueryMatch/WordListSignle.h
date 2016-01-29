#ifndef _WORDLISTSIGNLE_H_
#define _WORDLISTSIGNLE_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <boost/algorithm/string.hpp>
#include <Platform/sstypes.h>
#include <Platform/encoding.h>
#include "Platform/encoding/EncodingConvertor.h"

#include <assert.h>

#define MAXWORDLEN 1024
static const unsigned short PhraseSepS = 0xa1a1;


class WordListSignle {
public:
	WordListSignle()
	{
		Release();
	}

	virtual ~WordListSignle() {
		Release();
	}
	
	static inline void toDBS(const std::string &oriStr, std::string &dstStr)
	{
		dstStr = "";
		unsigned char tmp, tmp1;
		for(unsigned int i = 0; i < oriStr.length(); i++)
		{
			tmp = oriStr[i];
			tmp1 = oriStr[i + 1];
			if(tmp == 163)
			{
				dstStr += (unsigned char)oriStr[i + 1] - 128;
				i++;
				continue;
			}
			else if(tmp > 163)
			{
				dstStr += tmp ;
				dstStr += tmp1;
				i++;
				continue;
			}
			else if(tmp == 161 && tmp1 == 161)
			{
				dstStr += "";
				i++;
			}
			else
			{
				dstStr += tmp;
			}
		}
	}
	static inline void toQBS(const std::string &oriStr, std::string &dstStr)
	{
		dstStr = "";
		for(unsigned int i = 0; i < oriStr.length(); i++)
		{
			if(oriStr[i] <= 127 && oriStr[i] > 32)
			{
				dstStr += (char)163;
				dstStr += (char)(oriStr[i] + 128);
			}
			else
			{
				dstStr += oriStr[i];
			}
		}
	}

	static inline bool ispunct_gchar(const gchar_t ch)
	{
		const static std::set<gchar_t> puncts({44195 /*，*/, 41889 /*。*/, 41379 /*！*/, 49059 /*？*/,
				 48035 /*；*/, 47779 /*：*/, 42915 /*＇*/, 41635 /*＂*/, 41633 /*、*/, 43171 /*（*/,
				 43427 /*）*/, 44451 /*－*/, 48291 /*＜*/, 48803 /*＞*/, 46753 /*《*/, 47009 /*》*/,
				 44707 /*．*/, 64419 /*｛*/, 64931 /*｝*/, 56227 /*［*/, 56739 /*］*/, 48801 /*【*/,
				 49057 /*】*/, 43937 /*～*/});
		return puncts.find(ch) != puncts.end();
	}
	static inline bool isalpha_gchar(const gchar_t ch)
	{
		return ((ch & 0xff) == 0xa3 &&
						( ((ch & 0xff00) >= 0xe100 && (ch & 0xff00) <= 0xfa00) ||       // ａ-ｚ
						  ((ch & 0xff00) >= 0xc100 && (ch & 0xff00) <= 0xda00) ));      // Ａ-Ｚ
	}
	
	
	static inline bool isdigit_gchar(const gchar_t ch)
	{
		return ((ch & 0xff) == 0xa3 && (ch & 0xff00) >= 0xb000 && (ch & 0xff00) <= 0xb900);
	}
	
	
	static inline bool isspace_gchar(const gchar_t ch)
	{
		return (ch == 0xa1a1);
	}
	
	
	
	// 删除标点，除非标点前后都是数字字符。输入编码是gchar
	// 如果 delSpace==true，则也删除空格，除非空格前后都是英文字符
	// delNumberSign: 设为false时，保留'#'，用于vrqo的逐条可有可无词归一化 
	// 返回结果字符串的长度，单位是gchar，不包含结尾的0
	static size_t PuncNormalize_gchar(gchar_t *str, bool delSpace = false, bool delNumberSign = true)
	{
        enum {AD_NORM, AD_ALPHA, AD_DIGIT} adstate = AD_NORM;
        gchar_t *out = str, *in = str;
        for( ; *in; ++in)
        {
                if(isdigit_gchar(*in))
                {
                        adstate = AD_DIGIT;
                        *out++ = *in;
                }
                else if(isalpha_gchar(*in) || (!delNumberSign && *in == '#'))
                {
                        adstate = AD_ALPHA;
                        *out++ = *in;
                }
                else if(ispunct_gchar(*in))
                {
                        if(adstate == AD_DIGIT)
                        {
                                // we should look forward beyond all puncts, if also isdigit, keep those puncts
                                uint16_t *outt = out;   // keep start pos so that we may revert
                                *out++ = *in++;
                                while(*in && (ispunct_gchar(*in) || (delSpace && isspace_gchar(*in))))
                                        *out++ = *in++;
                                // now *in is the first char beyond all puncts
                                if(!isdigit_gchar(*in))
                                        out = outt;     // still remove the puncts
                                --in;   // because the outer "for" will ++in;
                        }
                        // if not after a digit, the digit can be safely removed, so nothing needs to be done
                }
                else if(delSpace && isspace_gchar(*in))
                {
                        if(adstate == AD_ALPHA)
                        {
                                // we should look forward beyond all spaces, if also isalpha, keep those spaces
                                uint16_t *outt = out;   // keep start pos so that we may revert
                                *out++ = *in++;
                                while(*in && (isspace_gchar(*in) || ispunct_gchar(*in)))
                                        ++in;   // first space is already kept before this "while", so later spaces can be removed
                                // now *in is the first char beyond all puncts
                                if(!isalpha_gchar(*in))
                                        out = outt;     // still remove the space
                                --in;   // because the outer "for" will ++in;
                        }
                        else if(adstate == AD_DIGIT)
                        {
                                // we should look forward beyond all puncts, if also isdigit, keep those puncts
                                uint16_t *outt = out;   // keep start pos so that we may revert
                                *out++ = *in++;
                                while(*in && (ispunct_gchar(*in) || isspace_gchar(*in)))
                                        *out++ = *in++;
                                // now *in is the first char beyond all puncts
                                if(!isdigit_gchar(*in))
                                        out = outt;     // still remove the puncts
                                --in;   // because the outer "for" will ++in;
                        }
                        // if not after an alpha, the space can be safely removed, so nothing needs to be done
                }
                else    // non-alphanum and non-punct
                {
                        *out++ = *in;
                        adstate = AD_NORM;
                }
        }
        if(out != str)
                *out = 0;
        else
                out = in;       // if out == str, then the whole string was deleted. revert to the original string in this case

        return out - str;
	}

	static int NormalizeWord(const char* SrcWord, char* DstWord, const int DstSize) {
		gchar_t conv[DstSize/2];
		if(EncodingConvertor::getInstance()->dbc2gchar(SrcWord, conv, DstSize/2, true) <= 0) {
			//std::cerr << "Error when converting word\n";
			return -1;
		}
		
		size_t rt = PuncNormalize_gchar(conv, true, false);
		if (rt <= 0){
			//std::cerr << "PuncNormalize_gchar word err\n";
			return -1;
		}
		conv[rt] = '\0';
		//fprintf(stderr, "[TEST] enter NormalizedWord\n");
		SBCSpace2PhrazeSep(conv);
		memcpy(DstWord, conv, DstSize);
		DstWord[DstSize - 1] = 0;
		return 0;
	}

protected:
	int Release() {						//	释放资源
		return 0;
	}

	static void SBCSpace2PhrazeSep(gchar_t* keyword) {
		gchar_t buff[MAXWORDLEN/2];
		memcpy(buff, keyword, MAXWORDLEN);
		size_t length = strlen((const char*)buff) / 2;
		for (int p = 0; p < length; p++) {
			int tmp = 0xa1a1;
			void *ptmp = (void *)&tmp;
			if (memcmp(buff + p, "　", sizeof(gchar_t)) == 0 ||
				memcmp(buff + p, ptmp, sizeof(gchar_t)) == 0 ||
				memcmp(buff + p, "，", sizeof(gchar_t)) == 0 || 
				memcmp(buff + p, "。", sizeof(gchar_t)) == 0) 
			{
				buff[p] = PhraseSepS;
			}
		}
		int state = 0;
		size_t p = 0;
		for (size_t i = 0; i <= length; i++) {
			//WEBSEARCH_DEBUG((LM_DEBUG, "query = %s, state = %d\n", buff, state));
			switch (state) {
				case 0://"(  )abc  de  "
					if (PhraseSepS != buff[i]) {
						keyword[p++] = buff[i];
						state = 1;
					}
					break;
				case 1://"  (abc)  de  "
					if (PhraseSepS == buff[i]) {
						state = 2;
					}
					else if (0 == buff[i]) {
						keyword[p] = 0;
						return;
					}
					else {
						keyword[p] = buff[i];
						p++;
					}
					break;
				case 2://"  abc(  )de  "
					if (PhraseSepS == buff[i]) {}
					else if (0 == buff[i]) {
						keyword[p] = 0;
						return;
					}
					else {
						keyword[p++] = PhraseSepS;
						keyword[p++] = buff[i];
						state = 1;
					}
					break;
				default:
					//WEBSEARCH_DEBUG((LM_DEBUG, "illegal state = %d\n", state));
					return;
			}
		}
	}
};

#endif
