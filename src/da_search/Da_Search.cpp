/*************************************************
 * @yonghuahu
*************************************************/

#include "Da_Search.h"

using namespace std;
using namespace spaceGary;
using namespace WHITE_BLACK_DA;


/*
set<string> cdlgindex::m_stopword;
set<string> cdlgindex::m_postfix_word;
set<string> cdlgindex::m_black;
*/
set<string> m_stopword;
set<string> m_postfix_word;
set<string> m_black;


DA_SEARCH::DA_SEARCH()
{
}

DA_SEARCH::~DA_SEARCH()
{
	Release();
}

int32_t DA_SEARCH::LoadDAs(const char* dir_base)
{
	DaSearch = new Darts::DoubleArray();
	WhiteBlackList_ da;
	cerr << "[INFO] Begin to LoadIndex: " << dir_base << endl;

	string Index_path;
	Index_path = dir_base;
	if(da.LoadIndex(Index_path.c_str(), DaSearch, Search_Buf, Search_Buf_begin_end_len) == -1)
	{
		cerr << "[ERROR] Fail to load Singer_Buf index file!" << endl;
		return -1;
	}
	cerr << "[INFO] LoadIndex() is OK" << endl;

	return 0;
}
int32_t DA_SEARCH::Init(const char* dict_base)
{
	cerr << "[INFO] Begin to loadDAs: " << dict_base << endl;
	if(LoadDAs(dict_base) == -1)
	{
		return -1;
	}

	/*
	if(Norm.Init() == -1)
	{
		return -1;
	}
	*/
	return 0;
}

int32_t DA_SEARCH::Release()
{
	Release_();
	return 0;
}

int32_t DA_SEARCH::Release_()
{
	if(DaSearch)
	{
		delete DaSearch;
		DaSearch = NULL;
	}
	if(Search_Buf)
	{
		delete Search_Buf;
		Search_Buf = NULL;
	}
	return 0;
}


int32_t DA_SEARCH::DA_Search(string& dest_norm, map<string,string>& index_text, string& search_key)                                                         
{
    int ret = -1;
	vector<string> content;
    if(DaSearch == NULL)
    {
        cerr << "[ERROR] DaSearch Darts's Address is NULL" << endl;
        return -1;
    }
    DaSearch->exactMatchSearch(dest_norm.c_str(), ret);
    cerr << "[INFO] ret value of exactMatchSearch = " << ret << endl;
    if(ret > -1)
    {
        if(ret < Search_Buf_begin_end_len)
        {
            string out = &Search_Buf[ret];
			index_text["da"] = out;
		}

	}
	content.clear();
	return 0;
}

int32_t DA_SEARCH::AbstractInfo(string values, string& singer, string& song, string& album)
{
	int start;
	int loc_song;
	int loc_album;
	int loc_singer;
	int loc_lrc;
	loc_song = values.find("song]:");
	loc_album = values.find("[album]:");
	loc_singer = values.find("[singer]:");
	loc_lrc = values.find("[lrc]:");

	if(loc_song != -1 && loc_album > loc_song + 6)
	{
		song = values.substr(loc_song + 6, loc_album - loc_song - 6);
	}
	if(loc_album != -1 && loc_singer > loc_album + 8)
	{
		album = values.substr(loc_album + 8, loc_singer - loc_album - 8);
	}
	if(loc_singer != -1 && loc_lrc > loc_singer + 9)
	{
		singer = values.substr(loc_singer + 9, loc_lrc - loc_singer - 9);
	}

	// filter
	if(singer.length() > 20)
	{
		cerr << "[NOTE] singer's length is too long, probabily wrong content." << endl;
		return -1;
	}
	return 0;
}
int32_t DA_SEARCH::GetIndexResults(
		string& query,
		map<string, string>& da_vec,
		vector<string>& seg_parts,
		SEGMENT_1 *seg)
{
	da_vec.clear();
	// normalize dest_ content
	vector<string> parts;
	seg->Segment_(
			query,
			parts);
	if(parts.size() < 1)
	{
		cerr <<"[note] segment size lt 1" << endl;
		return 0;
	}
	string dest_norm = "";
	for(int i = 0; i < parts.size(); i++)
	{
		dest_norm += parts[i];
	}
	cout << "[INFO] dest_norm: " << dest_norm << endl;
	
	da_vec.clear();
	//dlg.indexoutcome(query.c_str());
	string INDEX_TYPE;
	DA_Search(dest_norm, da_vec, INDEX_TYPE);

	/*map<string, string>::iterator iter;
	for(iter = da_vec.begin(); iter != da_vec.end(); iter++)
	{
		cout << "[DA INDEX] " << iter->second << endl;
	}*/
	
	return 0;
}
