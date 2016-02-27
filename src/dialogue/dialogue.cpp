#include "dialogue.h"
#include "EditDist.h"
using namespace std;

//const static string DA_IndexFile_DLG = "index_dir/qqchat.data.da.Index";
//const static string DOC_IndexFile_DLG = "index_dir/qqchat.data.doc.Index";
//const static string DA_IndexFile_DLG = "index_dir/da.Index";
//const static string DOC_IndexFile_DLG = "index_dir/doc.Index";
//const string DA_IndexFile_DLG = "index_dir/singer.txt.da.Index";
//const string  DOC_IndexFile_DLG = "index_dir/sogou.query.post.doc.Index";
const static string DA_IndexFile_DLG = "index_dir/dlg.da.index";
const static string DOC_IndexFile_DLG = "index_dir/dlg.doc.index";

const static string FILE_PUNC = "punc/punc.txt";
const static int threshold = 50;			// data[i].length()/query.length()，太大的话，表示查找失败；
static queue<string> context;				// 保存上下文
static map<string, int> hash_context;		// 保存上下文

//
DIALOGUE::DIALOGUE()
{
	EncodingConvertor::initializeInstance();
	analysis::DicTree::instance()->loadDic();
}
DIALOGUE::~DIALOGUE()
{
	;
}
int DIALOGUE::loadDic(const char* dir_base)
{
	// load punc dict
	ifstream infile;
	string puncFilePath = string(dir_base) + "/data/" + FILE_PUNC;
	cerr << "[INFO] path of punc.txt: "<<puncFilePath << endl;
	infile.open(puncFilePath.c_str(), ios::in);
	if(!infile)
	{
		cerr << "[ERROR] : Fail to open punc.txt file!" << endl;
		return -1;
	}
	string line;
	while(getline(infile, line))
	{
		spaceGary::StringTrim(line);
		if(line != "" && line != " ")
		{
			Hash_punc[line] ++;
		}
	}
	infile.close();
	//
	
	return 0;
}
int DIALOGUE::Init(const char* dir_base) // data directory
{
	// Init dlg da and docs index
	//doc_search.Init(dir_base);
	string data_path = string(dir_base) + "data/";
	string index_file = data_path + "/" + DOC_IndexFile_DLG;
	doc_search.Init(index_file.c_str());
	/*{
		cerr << "[ERROR] Fail to load index file!" << endl;
		return -1;
	}*/

	// da_index
	data_path = string(dir_base) + "data/";
	index_file = data_path + "/" + DA_IndexFile_DLG;
	cerr << "[dialogue.cpp] [Init] da_index file : " << index_file << endl;
	if(da_index->Init(index_file.c_str()) == -1)
	{
		return -1;
	}

	// Load dicts	
	if(loadDic(dir_base) == -1)
	{
		return -1;
	}
	seg = SEGMENT_1::getInstance();
	seg->Init();

	return 0;
}

int DIALOGUE::Release() 
{
	seg->Release();
	doc_search.Release();
	da_index->Release();
}
int DIALOGUE::Normalize_(const char* query)
{
	;
}
int DIALOGUE::show_results(size_t query_length, map<int, string>& da_vec)
{
	map<int, string>::iterator iter;
	vector<string> values;
	string singer, song, album;
	map<string, string> anwser_hash;
	map<string, string>::iterator iter_;
	vector<string> tmp;
	vector<string> anwser_vec;
	string anwser;
	int32_t nearest_dis = 10000;
	int32_t dis;
	for(iter = da_vec.begin(); iter != da_vec.end(); iter++)
	{
		anwser_vec.clear();
		anwser_hash.clear();
		cerr << "out: " << iter->first << "\t" << iter->second << endl;
		if(iter->first > 5)
		{
			cerr << "[NOTE] Cannot find the shortest and most similar QUESTION!" << endl;
			break;
		}
		anwser = iter->second;
		spaceGary::StringSplit(anwser,anwser_vec, "###");
		if(anwser.length() < 1)
		{
			cerr << "[NOTE] length of anwser is too short!" << endl;
			return 0;
		}
		for(int i = 0; i < anwser_vec.size(); i++)
		{
			if((dis = anwser_vec[i].length() < query_length ? 
						query_length - anwser_vec[i].length(): anwser_vec[i].length() - query_length) < nearest_dis)
			{
				anwser = anwser_vec[i];
				nearest_dis = dis;
			}
		}
		da_index->GetIndexResults(anwser, anwser_hash, tmp, seg);
		cerr << "----------------------------------------------------------------------------------------------------------------------------" << endl;
		for(iter_ = anwser_hash.begin(); iter_ != anwser_hash.end(); iter_++)
		{
			cerr << "| ANWSER:" << iter_->second << endl;
		}
		cerr << "----------------------------------------------------------------------------------------------------------------------------" << endl;
		break;
		//spaceGary::StringSplit(iter->second, values, "|||");
		//for(int i = 0; i < values.size(); i++)
		//{
		//}
	}
	return 0;
}


int DIALOGUE::IndexOutcome(const char* query)
{
	char dest[4096];
	memset(dest,0x00,4096);
	EncodingConvertor::getInstance()->dbc2gchar(
			query,
			(gchar_t*)dest,
			4096/2,
			true);

	DlgResult doc_result;
	vector<string> data;
	vector<string> anwser;
	string query_ = query;
	cerr << "============================ BEGINE ==================================" << endl;
	cerr << "【ME】: " << query << "" << endl << endl;
	/*if(context.size() < 3)
	{
		context.push(query_);
	}else
	{
		string del = context.back();
		context.pop();
		hash_context.erase(del);
		
		context.push(query_);

	}
	hash_context[query_]++;
	if(hash_context[query_] >= 3)
	{
		cerr << "【Angry】: 干嘛重复啊，喂??? "<< endl;
		hash_context[query_]--;
		//context.pop();
		cerr << "============================ END ==================================" << endl;
		return 0;
	}

	map<string,int>::iterator iter;
	for(iter = hash_context.begin(); iter != hash_context.end(); iter++)
	{
		cerr << "content of hash_context: " << iter->first << "\t" << iter->second << endl;
	}
	cerr << "size of context: " << context.size()<< endl;
	for(int i = 0 ; i < context.size(); i++)
	{
		cerr << "content of context: " << context.size()<< endl;
	}
	*/
	//if(doc_search.GetDlgResult(query_,doc_result) == 0)
	doc_search.GetIndexResults(query_, doc_result, seg);
	{
		//string key = doc_result.query;
		data = doc_result.data;
	}
	cerr << "\tJUST LET ME THINK A MOMENT ... " << endl << "\t... ..." << endl << "\t... ..."<< endl << endl;
	if(data.size() > 0)
	{
		for(int i = 0; i < data.size(); i++)
		{
			if(data[i].length()/strlen(query) > threshold)
			{
				cerr << "【HU ZI】:  [ O_o ???   What are you 弄啥嘞 ? ]" << endl;
				//anwser.clear();
			}
			else	
				cout << "\t\t\t\t\t\tU R ASKING ...\t" << data[i] << " ?"<< endl;
		}
	} else
	{
		cerr << "【HU ZI】:  [ 你能好好说话不？？        What are you 弄啥嘞 ? ]" << endl;
	}

	EditDist ed;
	vector<int> sortED;
	map<int, string> cadidate_sort;
	sortED.clear();
	cadidate_sort.clear();
	vector<string> tmp;
	tmp = data;
	ed.EditSort(query,tmp,cadidate_sort, sortED, seg);
	// show results
	show_results(query_.length(), cadidate_sort);
	cadidate_sort.clear();
	sortED.clear();
	cerr << "----------------------------------------------------------------------------------------------------" << endl;


	/*
	if(anwser.size() > 0)
	{
		for(int i = 0; i < anwser.size(); i++)
		{
			spaceGary::StringTrim(anwser[i]);
			cerr << "【HU ZI】: [ " << anwser[i] << " ]" << endl;
		}
	}
	*/
	cerr << "============================ END ==================================" << endl;

	return 0;
}

int DIALOGUE::MakeResponse(const char* query, const char* anwser)
{
	;
}
