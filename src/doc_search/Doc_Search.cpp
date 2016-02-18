/*************************************************
 * @yonghuahu
*************************************************/

#include "Doc_Search.h"

using namespace std;
using namespace spaceGary;

//const string DOC_INDEX_FILE = "index_dir/singer.song.album.doc.index";
//const string QUERY_INDEX_FILE = "index_dir/sogou.query.doc.index";


/*
set<string> cdlgindex::m_stopword;
set<string> cdlgindex::m_postfix_word;
set<string> cdlgindex::m_black;
*/
set<string> m_stopword;
set<string> m_postfix_word;
set<string> m_black;


DOC_SEARCH::DOC_SEARCH()
{
}

DOC_SEARCH::~DOC_SEARCH()
{
	Release();
}

int32_t DOC_SEARCH::LoadDOCs(const char* dir_base)
{
	cerr << "[INFO] LoadIndex() is OK" << endl;

	return 0;
}
int32_t DOC_SEARCH::Init(const char* indexFilePath)
{
	stringstream oss;
	
	//doc_index.Init(dir_base);
	//query_doc_index.Init(dir_base);
	//oss.str("");
	//oss << ILE;
	//cerr << "[INFO] doc_index file path: "<< oss.str().c_str() << endl;
	cerr << "[INFO] doc_index file path: "<< indexFilePath << endl;
	if(doc_index.LoadIndex(indexFilePath) == -1)
	{
		cerr << "[INFO] Fail to loadIndex of doc" << endl;
		return -1;
	}
	cerr << "[INFO] loadIndex of doc ...... OK" << endl;
	return 0;
}

int32_t DOC_SEARCH::Release()
{
	return 0;
}


int32_t DOC_SEARCH::DOC_Search(string& dest_norm, DlgResult& doc_result,SEGMENT_1* seg) 
{
	if(doc_index.GetDlgResult(dest_norm, doc_result, seg) == -1)
		cerr << "[ERROR] Fail to run GetDlgResult() " << endl;
	return 0;
}
int32_t DOC_SEARCH::Query_DOC_Search(string& dest_norm, DlgResult& doc_result)
{
	//if(query_doc_index.GetDlgResult(dest_norm, doc_result) == -1)
	//	cerr << "[ERROR] Fail to run GetDlgResult() " << endl;
	return 0;
}

int32_t DOC_SEARCH::GetIndexResults(
		string& query,
		DlgResult& doc_result,
		SEGMENT_1* seg)
{
	// normalize dest_ content
	vector<string> parts;
	seg->Init();
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
	cerr << "[INFO] dest_norm: " << dest_norm << endl;

	DOC_Search(dest_norm, doc_result, seg);
	//Query_DOC_Search(dest_norm, doc_result);

	int num = 1;
	// show results
	{
	}
	return 0;
}
