#include "dialogue.h"

using namespace std;

//const static string DA_IndexFile_DLG = "/tieba/pair.out.post.norm.docIndex";
const static string DA_IndexFile_DLG = "index_dir/doc.Index";
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
	doc_index.Init(dir_base);
	string data_path = string(dir_base) + "data/";
	string index_file = data_path + "/" + DA_IndexFile_DLG;
	if(doc_index.LoadIndex(index_file.c_str()) == -1)
	{
		cerr << "[ERROR] Fail to load index file!" << endl;
		return -1;
	}

	// Load dicts	
	if(loadDic(dir_base) == -1)
	{
		return -1;
	}
	return 0;
}

int DIALOGUE::Release() 
{
}
int DIALOGUE::Normalize_(const char* query)
{
	;
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

	//CDlgIndex doc_index;
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
	if(doc_index.GetDlgResult(query_,doc_result) == 0)
	{
		string key = doc_result.query;
		data = doc_result.data;
		//anwser = doc_result.anwser;
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
				cerr << "\t\t\t\t\t\tU R ASKING ...\t" << data[i] << " ?"<< endl;
		}
	} else
	{
		cerr << "【HU ZI】:  [ 你能好好说话不？？        What are you 弄啥嘞 ? ]" << endl;
	}

	if(anwser.size() > 0)
	{
		for(int i = 0; i < anwser.size(); i++)
		{
			spaceGary::StringTrim(anwser[i]);
			cerr << "【HU ZI】: [ " << anwser[i] << " ]" << endl;
		}
	}
	cerr << "============================ END ==================================" << endl;

	return 0;
}

int DIALOGUE::MakeResponse(const char* query, const char* anwser)
{
	;
}
