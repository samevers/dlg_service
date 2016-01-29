#include "QueryMatch.h"
#include <stdlib.h>
#include <fstream>
#include <iterator>
#include <iostream>
#include <sstream>
#include <cmath>
#include <boost/algorithm/string.hpp>
#include <regex.h>
#include <boost/regex.hpp>
#include <boost/exception/all.hpp>

#define DEBUG_ON 0
#define debug_sam 1
#define DEBUG_LSK 0

using namespace std;
using namespace boost; 

const static int MAXSTRLEN=1024;
const static int MAXQUERYLEN=40;
//const static int MAXSBIT=64;

const static string QUERYMATCH_DIR = "./data/base/querymatch";
const static string ALL_WORDLIST_FILE="vrqo_VRwordlist";// queryMatch 所有词典文件
const static string ALL_RULE_FILE="vrqo.rule";
const static string NUMBER_REG_FILE="number.reg";
const static string NUMBER_EXPRESS_REG_FILE="expressNumber.reg";
const static string NUMBER_PHONE_REG_FILE="phoneNumber.reg";
const static string KEY_TAG_FILE="tag.name";

QueryMatch* QueryMatch::ddinstance;


QueryMatch* QueryMatch::getInstance(){
	if(!ddinstance){
		ddinstance=new QueryMatch();
	}
	return ddinstance;
}

void QueryMatch::releaseInstance(){
	if(ddinstance){
		delete ddinstance;
		ddinstance=NULL;
	}
}

QueryMatch::QueryMatch(){
	ec = EncodingConvertor::getInstance();
	dictree = analysis::DicTree::instance();
	iqdic = analysis::IQDic::instance();
	segmentor = WordSegmentor::getInstance();
	graphSeg = GraphSegmentor::getInstance();


	//LoadWordList();
}

QueryMatch::~QueryMatch(){
	WordSegmentor::releaseInstance();

}

bool QueryMatch::open(const char *allWordlist)
{
	bool ret;
	ostringstream oss;
	oss.str("");
	oss << QUERYMATCH_DIR << "/" << NUMBER_REG_FILE; 
	string regFile;
	regFile = oss.str().c_str();
	ret = loadRegexPattern(regFile.c_str(), numreg); // number.reg
	if (!ret)
	{
		return false;
	}
	oss.clear();
	oss.str("");
	oss <<QUERYMATCH_DIR << "/" << NUMBER_EXPRESS_REG_FILE; 
	regFile = oss.str().c_str();
	ret = loadRegexPattern(regFile.c_str(), expressNumreg);// expressnumber.reg
	if (!ret)
	{
		return false;
	}
	oss.clear();
	oss.str("");
	oss <<QUERYMATCH_DIR << "/" << NUMBER_PHONE_REG_FILE; 
	regFile = oss.str().c_str();
	ret = loadRegexPattern(regFile.c_str(), phoneNumreg);// phoneNumber.reg
	if (!ret)
	{
		return false;
	}
	oss.clear();
	oss.str("");
	ret = LoadWordList(allWordlist);// load word files.
	if (!ret)
	{
		cerr << "load base dic failed!" << endl;
		return false;
	}
	return true;
}


bool QueryMatch::loadRegexPattern(const char* filename, string& regexString) {
	regexString = "";
	fstream fin(filename);
	if (!fin)
	{
		cerr<<"MSG_INFO: no such the file["<<filename<<"]!!"<<endl;
		return false;
	}

	cerr<<"Begin to load file:["<<filename<<"]..."<<endl;

	long nCnt = 0;
	string line;
	while(getline(fin,line))
	{
		if (line == "")
			continue;

		if (nCnt==0)
			regexString += ("(" + line + ")");
		else
			regexString += ("|(" + line + ")");
		++nCnt;
	}
	fin.close();

	return true;

}

bool QueryMatch::LoadWordList(const char *allWordlist)
{
	ostringstream oss;
	oss.str("");
	oss <<QUERYMATCH_DIR << "/" << ALL_WORDLIST_FILE; // drugname.word | expressnumber.word | map.word | noveldetail.word | show.word
	string allWordlistDir;
	allWordlistDir = oss.str().c_str();
	std::ifstream fin(allWordlistDir.c_str());
	oss.clear();
	oss.str("");
	if(!fin)
	{
		cerr<<"can't open map lexion: "<<allWordlistDir<<endl;
		return false;
	}
	string line;
	long int index = 4; 
	key_word_tag[0] = "number_key_0";
	key_word_tag[1] = "phonenumber_key_0";
	key_word_tag[2] = "expressnumber_key_0";
	tagKeyMapIndex["number_key_0"] = 0;
	tagKeyMapIndex["phonenumber_key_0"] = 1;
	tagKeyMapIndex["expressnumber_key_0"] = 2;
	vector<const char *> da_key_vect;
	vector<long int> da_value_vect;
	while(getline(fin,line)){
		trim(line);
		oss.clear();
		oss.str("");
		oss << QUERYMATCH_DIR << "/" << line;
		string oneFilename;
		oneFilename = oss.str().c_str();
		std::ifstream finOne(oneFilename.c_str());// 顺序打开*.word 文件
		if(!finOne)
		{
			cerr<<"can't open map lexion: " << oneFilename << endl;
		}
		else
		{
			string keyLine;
			while(getline(finOne, keyLine)){
				trim(keyLine);
				string::size_type pos = keyLine.find_first_of('\t');
				if(string::npos == pos){	// *.word 词表里没有\t 的部分
					vector<string> tagKeyVec;
					boost::algorithm::split(tagKeyVec, keyLine, boost::is_any_of("="));// 但是有=的部分；这是可有可无词
					if(2 != tagKeyVec.size())
					{
						cerr << "ERROR: "<< oneFilename  << " bad format! data:" << keyLine << "\t" << tagKeyVec.size()  << endl;
						continue;
					}
					string tempStr = tagKeyVec[0];// tag
					
					trim(tempStr);
					string tag = tempStr;	
					if (tagKeyMapIndex.find(tempStr) == tagKeyMapIndex.end())
					{
						tagKeyMapIndex[tempStr] = index; // tagKeyMapIndex
						key_word_tag[index] = tempStr;	// key_word_tag
						index = index + 1;
					}
					vector<string> tagShowVec;			// show.word 类文件中的后面的 key word
					boost::algorithm::split(tagShowVec, tagKeyVec[1], boost::is_any_of("|"));
					for(int i = 0; i < tagShowVec.size();i++)// 对于每个key word
					{
						string keyword = tagShowVec[i];
						trim(keyword);
						if(!bj2qj(keyword))
						{
							continue;
						}
						if (keyMapBit.find(keyword) == keyMapBit.end())
						{
							bitset<MAXSBIT> bitTagTmp;
							bitTagTmp.set(tagKeyMapIndex[tag]);
							keyMapBit[keyword] = bitTagTmp;// keyMapBit<string, bitset<> >: keyword,
															// 存放每个key word 对应的tag的集合
															// 该集合用bitset 存放
						}
						else
						{
							keyMapBit[keyword].set(tagKeyMapIndex[tag]);
						}
					}
				}
				else			// 对于*.word 文件，存在\t 的
				{
					string keyword = keyLine.substr(0, pos);// key
					trim(keyword);
					string tag = keyLine.substr(pos+1);		// tag
					trim(tag);
					string tempStr = tag;
					if (tagKeyMapIndex.find(tempStr) == tagKeyMapIndex.end())// 同样放到map结构：tagKeyMapIndex 中
					{
						tagKeyMapIndex[tempStr] = index;	// tagKeyMapIndex
						key_word_tag[index] = tempStr;		// key_word_tag
						index = index + 1;
					}

					if(!bj2qj(keyword))
					{
						continue;
					}

					if (keyMapBit.find(keyword) == keyMapBit.end())// 同样将keyword:tag 放到map 结构keyMapBit 中；
					{
						bitset<MAXSBIT> bitTagTmp;
						bitTagTmp.set(tagKeyMapIndex[tag]);
						keyMapBit[keyword] = bitTagTmp;
					}
					else
					{
						keyMapBit[keyword].set(tagKeyMapIndex[tag]);
					}
				}
			}
		}
		finOne.close();
	}
	fin.close();
	oss.clear();
	oss.str("");
	if(!MakeTrie(keyMapBit))			// MakeTrie
	{
		cerr << "tire tree build failse;" << endl;
		return false;
	}
	oss <<QUERYMATCH_DIR << "/" << ALL_RULE_FILE;
	string allRuleDir;
	allRuleDir= oss.str().c_str();
	std::ifstream finRule(allRuleDir.c_str());							// 加载 vrqo.rule 文件，
	if(!finRule)
	{
		cerr<<"can't open map lexion: "<<allWordlistDir<<endl;
		return false;
	}
	while(getline(finRule,line)){
		trim(line);
		string::size_type pos = line.find_first_of('\t');
		if(string::npos == pos){
			cerr << "ERROR: "<< allRuleDir  << " bad format! data:" << line << endl;
			continue;
		}
												// line : keyword + tagComData
		string keyword = line.substr(0, pos);	// keyword
		trim(keyword);
		string tagComData = line.substr(pos+1);	// tagComData
		trim(tagComData);
		ruleMapRes[keyword] = tagComData;		// keyword: tagComData
		vector<string> eleTag;
		boost::algorithm::split(eleTag, tagComData, boost::is_any_of( "|"));
		if(7 != eleTag.size())
			continue;
		string tag = eleTag[0];
		string tagTmp = tag + "_key";			// 如，measureexchange_key、noveldetail_key 等

		if(keyword.find("*") != string::npos)	// : * 符号的处理;目前还没有这个数据；
		{
			allshowKey.insert(tag);				// set: allshowKey[tag]
		}

		if(keyMapRule.find(tagTmp) != keyMapRule.end())
		{
			keyMapRule[tagTmp].push_back(keyword);// map<string, vector>: keyMapRule: tagTmp: <***_key>
												  // keyMapRule：每个tag 对应着哪些 rule 规则
		}else
		{
			vector<string> vecTmp;
			vecTmp.push_back(keyword);
			keyMapRule[tagTmp] = vecTmp;
		}
	}
	finRule.close();
	return true;
}

bool QueryMatch::MakeTrie(map<string, bitset<MAXSBIT> > &keyMapBitTmp)// keyMapBitTmp, 存放的是 keyword：tag 的 bitset 内容
{
	vector<const char *> da_key_vect;
	vector<int32_t> da_value_vect;
	map< string , int32_t> mapBitTmp;
	int32_t index = 0;
	map<string, bitset<MAXSBIT> >::iterator mit;
	for (mit = keyMapBitTmp.begin(); mit != keyMapBitTmp.end(); mit++)
	{
		string key = mit->first;				// key
		bitset<MAXSBIT> bitTagTmp = mit->second;// tags, bitset
		da_key_vect.push_back(key.c_str());							// da_key_vect, 存放key
		string tempStr = bitTagTmp.to_string();						// tags, 转换成字符串
		if (mapBitTmp.find(tempStr) == mapBitTmp.end())
		{
			vecBit.push_back(bitTagTmp);		// vecBit, 存放bitset:tags
			mapBitTmp[tempStr] = index;
			index = index + 1;
		}
		da_value_vect.push_back(mapBitTmp[tempStr]);				// da_value_vect, 存放bitset:tags 在vecBit 中的位置
	}
	if(daKeyWorList.build(da_key_vect.size(),&da_key_vect[0], 0, &da_value_vect[0], NULL) != 0)// 建 DATire
	{
		fprintf(stderr, "build da error!\n");
		return false;
	}
	return true;
}

bool QueryMatch::classifyQuery(const char *oriQuery, const char* query, int querylen, vector<ResultData> &vecType, long long request_id, analysis::QuerySegmentor *wordseg , analysis::TGraph *annseg, int modeFlag)
{
	
	if(!query || querylen < 1 || querylen > MAXQUERYLEN){return false;}
	string tmpQuery = "";
	for(int i=0;i<querylen;i++)
	{
		if(query[i]!=' ')
		{
			tmpQuery += query[i]; 
		}
	}
	trim(tmpQuery);
	vector<PreInfo> numberType;
	preconditioning(tmpQuery, numberType);

	query = tmpQuery.c_str();
	char buffer[MAXSTRLEN];
	if(WordListSignle::NormalizeWord(query, buffer, MAXSTRLEN) != 0)
	{
		return false;
	}
	string gQuery(buffer);
	int glen = strlen(buffer) / 2;

	analysis::WordSegmentorInfo request;
	analysis::WordSegmentorInfo parsed;
	vector<MarkInfo> vecMarkInfo;

	vector<PreInfo>::iterator vitP;
	vector<PosType> vetPosType;
	map<string,int> keySetType;
	map<string , vector<TagNode> > tagMapNodes;

	for(int i=0; i < gQuery.length()/2; ++i)
	{
		PosType tmpPosType;
		vetPosType.push_back(tmpPosType);
	}

	for(vitP = numberType.begin(); vitP != numberType.end(); vitP++)
	{
		//string allNumStr = vitP->text;
		char bufferTmp[MAXSTRLEN];
		//memset(bufferTmp, 0, MAXSTRLEN);
		//int lTmp = ec->t2sgchar(vitP->text.c_str(), (gchar_t*)bufferTmp, MAXSTRLEN/2, true);
		//if(lTmp >= MAXSTRLEN/2 || lTmp < 0)
		//	continue;
		//bufferTmp[len*2] = '\0';
		if(WordListSignle::NormalizeWord(vitP->text.c_str(), bufferTmp, MAXSTRLEN) != 0)
		{
			continue;
		}
		string allNumStr(bufferTmp);
		string::size_type pos = gQuery.find(allNumStr);

		MarkInfo tmpMInfo;
		tmpMInfo.text = allNumStr;
		tmpMInfo.start = pos;
		tmpMInfo.end = pos + allNumStr.size();

		TagNode tmpTagNode;
		tmpTagNode.text = allNumStr;
		tmpTagNode.start = pos;
		tmpTagNode.end = tmpMInfo.end;

		bitset<MAXSBIT> bitTagTmp;
		if(vitP->tag == "number_key_0")
		{
			bitTagTmp.set(0);
			for(int i = tmpMInfo.start/2; i < tmpMInfo.end/2; i++)
			{
				vetPosType[i].type.insert("number");
			}
			if(tagMapNodes.find("number_key_0") == tagMapNodes.end())
			{
				vector<TagNode> tmpVecTagNode;
				tmpVecTagNode.push_back(tmpTagNode);
				tagMapNodes["number_key_0"] = tmpVecTagNode;
			}
			else
			{
				tagMapNodes["number_key_0"].push_back(tmpTagNode);
			}
			keySetType["number"] = 1;

			map<string, vector<TagNode> >  posTagTmp = vetPosType[tmpMInfo.start/2].posTag;
			if(posTagTmp.find("number_key_0") == posTagTmp.end())
			{
				vector<TagNode> tmpVecTagNode;
				tmpVecTagNode.push_back(tmpTagNode);
				vetPosType[tmpMInfo.start/2].posTag["number_key_0"] = tmpVecTagNode;
			}
			else
			{
				vetPosType[tmpMInfo.start/2].posTag["number_key_0"].push_back(tmpTagNode);
			}
			
		}
		if(vitP->tag == "phonenumber_key_0")
		{
			bitTagTmp.set(1);
			for(int i = tmpMInfo.start/2; i < tmpMInfo.end/2; i++)
			{
				vetPosType[i].type.insert("phonenumber");
			}
			if(tagMapNodes.find("phonenumber_key_0") == tagMapNodes.end())
			{
				vector<TagNode> tmpVecTagNode;
				tmpVecTagNode.push_back(tmpTagNode);
				tagMapNodes["phonenumber_key_0"] = tmpVecTagNode;
			}
			else
			{
				tagMapNodes["phonenumber_key_0"].push_back(tmpTagNode);
			}
			keySetType["phonenumber"] = 1;

			map<string, vector<TagNode> >  posTagTmp = vetPosType[tmpMInfo.start/2].posTag;
			if(posTagTmp.find("phonenumber_key_0") == posTagTmp.end())
			{
				vector<TagNode> tmpVecTagNode;
				tmpVecTagNode.push_back(tmpTagNode);
				vetPosType[tmpMInfo.start/2].posTag["phonenumber_key_0"] = tmpVecTagNode;
			}
			else
			{
				vetPosType[tmpMInfo.start/2].posTag["phonenumber_key_0"].push_back(tmpTagNode);
			}
		}
		if(vitP->tag == "expressnumber_key_0")
		{
			bitTagTmp.set(2);
			for(int i = tmpMInfo.start/2; i < tmpMInfo.end/2; i++)
			{
				vetPosType[i].type.insert("expressnumber");
			}
			if(tagMapNodes.find("expressnumber_key_0") == tagMapNodes.end())
			{
				vector<TagNode> tmpVecTagNode;
				tmpVecTagNode.push_back(tmpTagNode);
				tagMapNodes["expressnumber_key_0"] = tmpVecTagNode;
			}
			else
			{
				tagMapNodes["expressnumber_key_0"].push_back(tmpTagNode);
			}
			keySetType["expressnumber"] = 1;

			map<string, vector<TagNode> >  posTagTmp = vetPosType[tmpMInfo.start/2].posTag;
			if(posTagTmp.find("expressnumber_key_0") == posTagTmp.end())
			{
				vector<TagNode> tmpVecTagNode;
				tmpVecTagNode.push_back(tmpTagNode);
				vetPosType[tmpMInfo.start/2].posTag["expressnumber_key_0"] = tmpVecTagNode;
			}
			else
			{
				vetPosType[tmpMInfo.start/2].posTag["expressnumber_key_0"].push_back(tmpTagNode);
			}
		}
		tmpMInfo.bit = bitTagTmp;
		vecMarkInfo.push_back(tmpMInfo);
	}


	if (modeFlag == 0)
	{
		for(int i=0; i < gQuery.length()/2; ++i)
		{
			string strTmp = gQuery.substr(i*2);
			//termList.push_back(strTmp);
			Darts_1::DoubleArray::result_type ret_value;
			Darts_1::DoubleArray::result_pair_type vResult[MAXSBIT];
			size_t resultLen = MAXSBIT;
			size_t reLen = daKeyWorList.commonPrefixSearch(strTmp.c_str(), vResult, resultLen);
			if (reLen > 0)
			{
				for (int j = 0; j < reLen; j++)
				{
					MarkInfo tmpMInfo;
					tmpMInfo.text = strTmp.substr(0, vResult[j].length);
					tmpMInfo.start = i*2;
					tmpMInfo.end = i*2 + vResult[j].length;
					tmpMInfo.bit = vecBit[vResult[j].value];
					vecMarkInfo.push_back(tmpMInfo);

					TagNode tmpTagNode;
					tmpTagNode.text = tmpMInfo.text;
					tmpTagNode.start = tmpMInfo.start;
					tmpTagNode.end = tmpMInfo.end;

					size_t sz = tmpMInfo.bit.size();
					for (size_t i = 0; i < sz; i++)
					{
						if(tmpMInfo.bit.test(i))
						{
							string tmpStrTag = key_word_tag[i];
							vector<string> tmpStrVecKeys;
							boost::algorithm::split(tmpStrVecKeys, tmpStrTag, boost::is_any_of("_"));

							for(int i = tmpMInfo.start/2; i < tmpMInfo.end/2; i++)
							{
								vetPosType[i].type.insert(tmpStrVecKeys[0]);
							}

							if(tagMapNodes.find(tmpStrTag) == tagMapNodes.end())
							{
								vector<TagNode> tmpVecTagNode;
								tmpVecTagNode.push_back(tmpTagNode);
								tagMapNodes[tmpStrTag] = tmpVecTagNode;
							}
							else
							{
								tagMapNodes[tmpStrTag].push_back(tmpTagNode);
							}

							if(tmpStrVecKeys[1] == "key")
							{
								keySetType[tmpStrVecKeys[0]] = 1;
							}

							map<string, vector<TagNode> >  posTagTmp = vetPosType[tmpMInfo.start/2].posTag;
							if(posTagTmp.find(tmpStrTag) == posTagTmp.end())
							{
								vector<TagNode> tmpVecTagNode;
								tmpVecTagNode.push_back(tmpTagNode);
								vetPosType[tmpMInfo.start/2].posTag[tmpStrTag] = tmpVecTagNode;
							}
							else
							{
								vetPosType[tmpMInfo.start/2].posTag[tmpStrTag].push_back(tmpTagNode);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		vector<string> termList;
		segmentor->Segment(wordseg, buffer, request, parsed, termList);
		vector<string>::iterator it;
		for (it = termList.begin(); it != termList.end(); it++)
		{
			Darts_1::DoubleArray::result_type ret_value;
			Darts_1::DoubleArray::result_pair_type vResult[MAXSBIT];
			size_t pos = gQuery.find(*it);
			string strTmp = gQuery.substr(pos);
			size_t resultLen = MAXSBIT;
			size_t reLen = daKeyWorList.commonPrefixSearch(strTmp.c_str(), vResult, resultLen);
			if (reLen > 0)
			{
				for (int j = 0; j < reLen; j++)
				{
					MarkInfo tmpMInfo;
					tmpMInfo.text = strTmp.substr(0, vResult[j].length);
					tmpMInfo.start = pos;
					tmpMInfo.end = pos + vResult[j].length;
					tmpMInfo.bit = vecBit[vResult[j].value];
					vecMarkInfo.push_back(tmpMInfo);

					TagNode tmpTagNode;
					tmpTagNode.text = tmpMInfo.text;
					tmpTagNode.start = tmpMInfo.start;
					tmpTagNode.end = tmpMInfo.end;

					size_t sz = tmpMInfo.bit.size();
					for (size_t i = 0; i < sz; i++)
					{
						if(tmpMInfo.bit.test(i))
						{
							string tmpStrTag = key_word_tag[i];
							vector<string> tmpStrVecKeys;
							boost::algorithm::split(tmpStrVecKeys, tmpStrTag, boost::is_any_of("_"));

							for(int i = tmpMInfo.start/2; i < tmpMInfo.end/2; i++)
							{
								vetPosType[i].type.insert(tmpStrVecKeys[0]);
							}

							if(tagMapNodes.find(tmpStrTag) == tagMapNodes.end())
							{
								vector<TagNode> tmpVecTagNode;
								tmpVecTagNode.push_back(tmpTagNode);
								tagMapNodes[tmpStrTag] = tmpVecTagNode;
							}
							else
							{
								tagMapNodes[tmpStrTag].push_back(tmpTagNode);
							}

							if(tmpStrVecKeys[1] == "key")
							{
								keySetType[tmpStrVecKeys[0]] = 1;
							}

							map<string, vector<TagNode> >  posTagTmp = vetPosType[tmpMInfo.start/2].posTag;
							if(posTagTmp.find(tmpStrTag) == posTagTmp.end())
							{
								vector<TagNode> tmpVecTagNode;
								tmpVecTagNode.push_back(tmpTagNode);
								vetPosType[tmpMInfo.start/2].posTag[tmpStrTag] = tmpVecTagNode;
							}
							else
							{
								vetPosType[tmpMInfo.start/2].posTag[tmpStrTag].push_back(tmpTagNode);
							}
						}
					}
				}
			}
		}
	}
	if(keySetType.size() > 1)
	{
		keySetType["number"] = 0;
	}
	map<string, int>::iterator svit;
	for(svit = keySetType.begin(); svit != keySetType.end(); svit++)
	{
		map<string, string> comRuleMapString;
		if(allshowKey.find(svit->first) != allshowKey.end())
		{
			string ruleTag, patQuery;
			string tagTmp = svit->first;
			BasePosComRule(tagTmp, 0, comRuleMapString, vetPosType, ruleTag, patQuery, 1, gQuery);
		}
		else
		{
			vector<PosType>::iterator vitPT;
			for(vitPT = vetPosType.begin(); vitPT != vetPosType.end(); vitPT++)
			{
				set<string> tmpPosType;
				tmpPosType = vitPT->type;
				set<string>::iterator vitSet;
				if(tmpPosType.find(svit->first) == tmpPosType.end() && tmpPosType.find("number") == tmpPosType.end())
					break;
			}
			if(vitPT != vetPosType.end())
			{
				keySetType[svit->first] = 0;
			}
			else
			{
				string ruleTag, patQuery;
				string tagTmp = svit->first;
				BasePosComRule(tagTmp, 0, comRuleMapString, vetPosType, ruleTag, patQuery, 0, gQuery);
			}
		}
		map<string, string>::iterator mssIt;
		for(mssIt=comRuleMapString.begin(); mssIt != comRuleMapString.end(); mssIt++)
		{
			//cout << mssIt->first << "\t" << mssIt->second << "\ttaoli1" << endl;
			string relucom = mssIt->first;
			string querySplit =  mssIt->second;
			string keyTag = svit->first;
			map<string, string> selectResult;
			docRulesSelect(relucom, querySplit, keyTag, selectResult);
			for(map<string, string>::iterator mssRit = selectResult.begin(); mssRit != selectResult.end(); mssRit++)
			//if(ruleMapRes.find(relucom) != ruleMapRes.end())
			{
				relucom = mssRit->first;
				querySplit = mssRit->second;

				//cout << relucom << "\t" << ruleMapRes[relucom] << "\t" << querySplit << "\ttaoli2" << endl;
				ResultData tmpResultData;
				vector<string> eleTag;
				boost::algorithm::split(eleTag, ruleMapRes[relucom], boost::is_any_of( "|"));
				if(eleTag.size() != 7)
				{
					continue;
				}
				vector<string> queryEle, dataEle, patQueryVec;
				tmpResultData.type = eleTag[0];
				tmpResultData.level = eleTag[3];
				tmpResultData.black = eleTag[4];
				tmpResultData.white = eleTag[5];
				if(tmpResultData.type == "expressnumber")
					tmpResultData.type = "expresszonghe";
				boost::algorithm::split(queryEle, eleTag[1], boost::is_any_of("&"));
				boost::algorithm::split(dataEle, eleTag[2], boost::is_any_of("&"));
				//boost::algorithm::split(patQueryVec, mssIt->second, boost::is_any_of( "|"));
				boost::algorithm::split(patQueryVec, querySplit, boost::is_any_of( "|"));
				if(patQueryVec.size() == 4 && eleTag[0] == "noveldetail" && patQueryVec[1].size() == 0 && patQueryVec[2].size() < 4)
				{
					continue;
				}
				for(vector<string>::iterator vvit = queryEle.begin(); vvit != queryEle.end(); vvit++)
				{
					if((*vvit).find("$") == 0)
					{
						string strInd = (*vvit).substr(1,(*vvit).size());
						try
						{
							int arrIndex = atoi(strInd.c_str());
							if(arrIndex == 0)
							{
								tmpResultData.query = tmpResultData.query + oriQuery;
							}
							else if(arrIndex <= patQueryVec.size() && arrIndex > 0)
							{
								if(eleTag[6] == "0")
								{
									tmpResultData.query = tmpResultData.query + patQueryVec[arrIndex-1];
								}
								else
								{
									string tmpStr = "";
									WordListSignle::toDBS(patQueryVec[arrIndex-1], tmpStr);
									tmpResultData.query = tmpResultData.query + tmpStr;
								}
							}
							else
							{
								tmpResultData.query = tmpResultData.query + *vvit;
							}
						}
						catch(boost::exception & err)
						{
							//cerr << err.what() << endl;
							tmpResultData.query = tmpResultData.query + *vvit;
							cerr << "rule write failed!" << endl;
						}
					}
					else
					{
						tmpResultData.query = tmpResultData.query + *vvit;
					}
				}

				for(vector<string>::iterator vvit = dataEle.begin(); vvit != dataEle.end(); vvit++)
				{
					if((*vvit).find("$") == 0)
					{
						string strInd = (*vvit).substr(1,(*vvit).size());
						try
						{
							int arrIndex = atoi(strInd.c_str());
							if(arrIndex == 0)
							{
								tmpResultData.data = tmpResultData.data + oriQuery;
							}
							else if(arrIndex <= patQueryVec.size() && arrIndex > 0)
							{
								if(eleTag[6] == "0")
								{
									tmpResultData.data = tmpResultData.data + patQueryVec[arrIndex-1];
								}
								else
								{
									string tmpStr = "";
									WordListSignle::toDBS(patQueryVec[arrIndex-1], tmpStr);
									tmpResultData.data = tmpResultData.data + tmpStr;
								}
							}
							else
							{
								tmpResultData.data = tmpResultData.data + *vvit;
							}
						}
						catch(boost::exception & err)
						{
							//cerr << err.what() << endl;
							tmpResultData.data = tmpResultData.data + *vvit;
							cerr << "rule write failed!" << endl;
						}
					}
					else
					{
						tmpResultData.data = tmpResultData.data + *vvit;
					}
				}
				
				vecType.push_back(tmpResultData);
			}
		}
	}
	return true;
}


bool QueryMatch::bj2qj(string & strTmp)
{
	if (strTmp.size() < 1 || strTmp.size() > MAXQUERYLEN)
	{
		return false;
	}
	char buffer[MAXSTRLEN];
	if(WordListSignle::NormalizeWord(strTmp.c_str(), buffer, MAXSTRLEN) != 0)
	{
		return false;
	}
	string gStr(buffer);
	strTmp = gStr;
	return true;
}

void QueryMatch::preconditioning(string &oriQuery, vector<PreInfo> &numberType)
{
	boost::smatch what;
	boost::smatch what2;
	boost::regex *fPhoneNumberpattern;
	boost::regex *fNumberpattern;
	boost::regex *fExpressNumberPattern;
	fNumberpattern = new boost::regex(numreg);
	string pnumber;
	string::const_iterator start, end;
	start = oriQuery.begin();
	end = oriQuery.end();
	boost::match_results<string::const_iterator> what1;
	boost::match_flag_type flags = boost::match_default;
	int number_min_size = 10, number_max_size = 0, isFlag = 0;
	while(regex_search(start, end, what1, *fNumberpattern, flags))
	{
		pnumber  = string(what1[0].first,what1[0].second) ;
		PreInfo tempPre;
		int tmpSize = pnumber.size();
		if(tmpSize < number_min_size)
		{
			number_min_size = tmpSize;
		}
		if(tmpSize > number_max_size)
		{
			number_max_size = tmpSize;
		}
		tempPre.text = pnumber;
		tempPre.tag = "number_key_0";
		numberType.push_back(tempPre);
		if(numberType.size() > 5)
		{
			isFlag = 1;
			break;
		}
		start = what1[0].second;
	}

	if(numberType.size()>0 && numberType.size() <= 5 && number_min_size >= 3 && number_max_size <= 28)
	{
		if(number_min_size >= 1 && number_max_size <= 12)
		{
			fPhoneNumberpattern = new boost::regex(phoneNumreg);
			bool search_flag_phonenumber = boost::regex_search(oriQuery, what, *fPhoneNumberpattern);
			if(search_flag_phonenumber)
			{
				pnumber = what[0];
				PreInfo tempPre;
				tempPre.text = pnumber;
				tempPre.tag = "phonenumber_key_0";
				numberType.push_back(tempPre);
			}
			delete fPhoneNumberpattern;
			fPhoneNumberpattern = NULL;
		}
		if(number_min_size >= 8 && number_max_size <= 28)
		{
			fExpressNumberPattern = new boost::regex(expressNumreg);
			bool search_flag_expressnumber = boost::regex_search(oriQuery, what2, *fExpressNumberPattern);


			if(search_flag_expressnumber)
			{
				pnumber = what2[0];
				PreInfo tempPre;
				tempPre.text = pnumber;
				tempPre.tag = "expressnumber_key_0";
				numberType.push_back(tempPre);
			}

			delete fExpressNumberPattern;
			fExpressNumberPattern = NULL;
		}
	}
	if(isFlag == 1)
	{
		numberType.clear();
	}

	delete fNumberpattern;
	fNumberpattern = NULL;
}

void QueryMatch::BasePosComRule(string &tag , int index, map<string, string> &comRuleMapString, vector<PosType> &vetPosType, string ruleTag, string patQuery, int flag, string &oriQuery, bool disregard)
{
	if(index == vetPosType.size())
	{
		if(disregard)
		{
			ruleTag = ruleTag + "*|";
			patQuery = patQuery + "|";
		}
		ruleTag = ruleTag.substr(0, ruleTag.size()-1);
		patQuery = patQuery.substr(0, patQuery.size()-1);
		//cout << ruleTag << "\t" << patQuery << "\ttaoli" << endl;
		
		vector<string> ruleTagVec;
		vector<string> patQueryVec;
		string ruleTagTmp, patQueryTmp;

		boost::algorithm::split(ruleTagVec, ruleTag, boost::is_any_of("|"));
		boost::algorithm::split(patQueryVec, patQuery, boost::is_any_of("|"));
		for(int i = 0; i < ruleTagVec.size(); i ++)
		{
			if(i == 0)
			{
				ruleTagTmp = ruleTagTmp + ruleTagVec[i];
				patQueryTmp = patQueryTmp + patQueryVec[i];
			}
			else
			{
				if(ruleTagVec[i] == ruleTagVec[i-1] && (ruleTagVec[i].find("_show_") != string::npos || ruleTagVec[i].find("_patt_") != string::npos))
				{
					patQueryTmp = patQueryTmp + patQueryVec[i];
				}
				else
				{
					ruleTagTmp = ruleTagTmp + "|" + ruleTagVec[i];
					patQueryTmp = patQueryTmp + "|" + patQueryVec[i];
				}
			}
		}
		//cout << ruleTagTmp << "\t" << patQueryTmp << "\ttaoli" << endl;

		comRuleMapString[ruleTagTmp] = patQueryTmp;
	}
	else if(flag == 0)
	{
		map<string , vector<TagNode> >::iterator vitMT;
		for(vitMT = vetPosType[index].posTag.begin(); vitMT != vetPosType[index].posTag.end(); vitMT++)
		{
			string strTmp = vitMT->first;
			if(strTmp.find(tag) != 0 && strTmp.find("number") != 0)
			{
				continue;
			}
			string ruleTagTmp = ruleTag + strTmp + "|";
			vector<TagNode> vecTN;
			vecTN = vitMT->second;
			vector<TagNode>::iterator vitVec;
			for(vitVec = vecTN.begin(); vitVec != vecTN.end(); vitVec++)
			{
				string patQueryTmp = patQuery + vitVec->text + "|";
				if(vitVec->end/2 <= index)
				{
					continue;
				}
				BasePosComRule(tag, vitVec->end/2,  comRuleMapString, vetPosType, ruleTagTmp, patQueryTmp, 0, oriQuery);
			}
		}
	}
	else if(flag == 1)
	{
		map<string , vector<TagNode> >::iterator vitMT;
		bool isFind = false;
		for(vitMT = vetPosType[index].posTag.begin(); vitMT != vetPosType[index].posTag.end(); vitMT++)
		{
			string strTmp = vitMT->first;
			if(strTmp.find(tag) != 0)
			{
				continue;
			}
			else
			{
				string ruleTagTmp;
				string patQueryTmp;
				if(disregard)
				{
					ruleTagTmp = ruleTag + "*|" + strTmp + "|";
					patQueryTmp = patQuery + "|";
				}
				else
				{
					ruleTagTmp = ruleTag + strTmp + "|";
					patQueryTmp = patQuery;
				}
				isFind = true;
				vector<TagNode> vecTN;
				vecTN = vitMT->second;
				vector<TagNode>::iterator vitVec;
				for(vitVec = vecTN.begin(); vitVec != vecTN.end(); vitVec++)
				{
					string patQueryTmpTmp = patQueryTmp + vitVec->text + "|";
					if(vitVec->end/2 <= index)
					{
						continue;
					}
					BasePosComRule(tag, vitVec->end/2,  comRuleMapString, vetPosType, ruleTagTmp, patQueryTmpTmp, 1, oriQuery, false);
				}
			}
		}
		if(!isFind)
		{
			string patQueryTmp = patQuery + oriQuery.substr(index*2, 2);
			string ruleTagTmp = ruleTag;
			BasePosComRule(tag, index + 1,  comRuleMapString, vetPosType, ruleTagTmp, patQueryTmp, 1, oriQuery, true);
		}
	}
}


void QueryMatch::docRulesSelect(string &ruleSplit, string &querySplit, string &keyTag, map<string, string> &selectResult)
{
	//cout << ruleSplit << "\t" << querySplit << "\t" << keyTag << endl;
	string keyDoc = keyTag + "_key";
	map<string, vector<string> > ::iterator mVectorRule = keyMapRule.find(keyDoc);
	if(mVectorRule == keyMapRule.end())
		return;
	vector<string> ruleVec;
	ruleVec = mVectorRule->second;
	for(vector<string>::iterator sVit = ruleVec.begin(); sVit != ruleVec.end(); sVit++)
	{
		string ruleTrue = *sVit;

		vector<string> ruleTagVec;
		vector<string> patQueryVec;
		vector<string> ruleTrueVec;
		string ruleTagTmp, patQueryTmp;

		boost::algorithm::split(ruleTagVec, ruleSplit, boost::is_any_of("|"));
		boost::algorithm::split(ruleTrueVec, ruleTrue, boost::is_any_of("|"));
		boost::algorithm::split(patQueryVec, querySplit, boost::is_any_of("|"));
		int i,j;
		bool isAllAllow = false;
		for(i = 0, j = 0; i < ruleTrueVec.size(); i ++)
		{
			if(i == 0)
			{
				if(ruleTrueVec[i] == ruleTagVec[j])
				{
					ruleTagTmp = ruleTagTmp + ruleTrueVec[i];
					patQueryTmp = patQueryTmp + patQueryVec[j];
					j++;
				}
				else if(ruleTrueVec[i].find("_show_") != string::npos)
				{
					ruleTagTmp = ruleTagTmp + ruleTrueVec[i];
					patQueryTmp = patQueryTmp + "";
				}
				else
				{
					break;
				}
			}
			else
			{
				if(j < ruleTagVec.size() && ruleTrueVec[i] == ruleTagVec[j])
				{
					ruleTagTmp = ruleTagTmp + "|" + ruleTrueVec[i];
					patQueryTmp = patQueryTmp + "|" + patQueryVec[j];
					j++;
				}
				else if(j == ruleTagVec.size() && (ruleTrueVec[i].find("_show_") != string::npos || ruleTrueVec[i] == "*"))
				{
					ruleTagTmp = ruleTagTmp + "|" + ruleTrueVec[i];
					patQueryTmp = patQueryTmp + "|";
				}
				else if(ruleTrueVec[i].find("_show_") != string::npos)
				{
					ruleTagTmp = ruleTagTmp + "|" + ruleTrueVec[i];
					patQueryTmp = patQueryTmp + "|";
				}
				else
				{
					break;
				}
			}
		}
		if(i == ruleTrueVec.size() && j == ruleTagVec.size())
		{
			selectResult[ruleTagTmp] = patQueryTmp;
		}
	}
}

