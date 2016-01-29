/* queryclass
*  @date: 2015.4.21
*  @author: litao207264
*/

#ifndef __SOGOU_QUERYMATCH_H
#define __SOGOU_QUERYMATCH_H

#include <set>
#include <map>
#include <vector>
#include <string>
#include <bitset>
#include "Platform/encoding/support.h"
#include "Platform/encoding/EncodingConvertor.h"
#include "Platform/encoding/stringmatch/AhoCorasick.h"
#include "Platform/encoding.h"
#include "Platform/gchar.h"
#include "WordSegmentor4/IQSegmentor/IndexSegmentor.h"
#include "WordSegmentor4/IQSegmentor/QuerySegmentor.h"
#include "WordSegmentor4/IQSegmentor/IQSegmentor.h"
#include <regex.h>
#include "WordSegmentor.h"
#include "GraphSegmentor.h"
#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <ace/Task.h>
#include <ext/hash_map>
#include <sys/stat.h>
#include <sys/types.h>
#include "../vr_data_engine/include/darts.h"
#include "WordListSignle.h"

using std::string;

const static int MAXKEYWORDSIZE=1024;
const static int MAXSBIT=512;

//const static string key_word_tag[MAXSTRLEN] = {"", "stop_word", "number_key", "phonenumber_key", "phonenumber_show", "expressnumber_key", "novel_key", "novel_show"};

// 
struct MarkInfo{
	string text;
	int start;
	int end;
	//int type;
	bitset<MAXSBIT> bit;
};


struct TagNode{
	string text;
	int start;
	int end;
};

struct PreInfo{
	string text;
	string tag;
};

struct PosType{
	set<string> type;
	map<string, vector<TagNode> >  posTag;
};

struct ResultData{
	string type; // 内部vr的类别名称
	string query;
	string data;
	string level;// vr_level
	string black;
	string white;
	string isConvert;// 全半角
};


class QueryMatch
{
public:
	static QueryMatch* getInstance();
	
	bool open(const char *dataPrefix="");
	static void releaseInstance();
	bool classifyQuery(const char *oriQuery,const char* query, int querylen, vector<ResultData> &vecType, long long request_id, analysis::QuerySegmentor *wordseg ,analysis::TGraph *annseg, int modeFlag=0);
	~QueryMatch();
private:
	QueryMatch();
	QueryMatch(const QueryMatch&);
	QueryMatch& operator = (const QueryMatch&);

private:
	bool MakeTrie(map<string, bitset<MAXSBIT> > &keyMapBitTmp);
	bool bj2qj(string & strTmp);
	bool loadRegexPattern(const char* filename, string& regexString);
	bool LoadWordList(const char *dataPrefix="");
	void preconditioning(string &oriQuery ,vector<PreInfo> &numberType);
	
	void BasePosComRule(string &tag , int index, map<string, string> &comRuleMapString, vector<PosType> &vetPosType, string ruleTag, string patQuery, int flag, string &oriQuery, bool disregard=false);
	void docRulesSelect(string &ruleSplit, string &querySplit, string &keyTag, map<string, string> &selectResult);

	EncodingConvertor* ec;
	analysis::DicTree* dictree;
	analysis::IQDic* iqdic;

	WordSegmentor* segmentor;
	GraphSegmentor* graphSeg;

	static QueryMatch *ddinstance;

	map<string, string> ruleMapRes;

	set<string> allshowKey;
	map<string, vector<string> > keyMapRule;


	Darts_1::DoubleArray daKeyWorList;
	map<string, bitset<MAXSBIT> > keyMapBit;
	map<string,int> tagKeyMapIndex;
	vector< bitset<MAXSBIT> > vecBit;

	string numreg;
	string phoneNumreg;
	string expressNumreg;

	string key_word_tag[MAXKEYWORDSIZE];

	
};

#endif
