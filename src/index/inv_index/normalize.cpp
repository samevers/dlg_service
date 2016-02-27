/*************************************************
 * @yonghuahu
*************************************************/

#include "normalize.h"

using namespace std;

const char* SYN_FILE = "/search/yonghuahu/dm_/project/dlg_service/data/syn/synterm.txt";
analysis::TGraph graph(65536);

Normal::Normal()
{
}
Normal::~Normal()
{
}

int32_t Normal::Init()
{
	// init segment dict
	//analysis::TGraph m_graph_segment;
	EncodingConvertor::initializeInstance(); 
    analysis::DicTree::instance()->loadDic();
    graph.open(analysis::DicTree::instance());

	if( loadSyn()== -1)
	{
		cerr << "[ERROR] Fail to load syn dict." << endl;
		return -1;
	}

	return 0;
}

int32_t Normal::Segment_(
		analysis::TGraph& graph,
		const string& query,
		std::vector<std::string>& parts)
{
	parts.clear();
    char dest[10240];
    char buf[4096];
    bchar_t zero[1] = {L'\0'};
    if(query.length()<2) //长度过短
		return 0;

    analysis::ResultInfo info[100];
	StringUtils::gbk_utf16le(
			strcpy(buf,query.c_str()), 
			query.length(), 
			dest);
    int r = graph.graphSegment(
			(bchar_t*)dest,
			bcscspn((bchar_t*)dest,zero),
			info,
			100);

    for (int i = 0; i < r; i++) 
	{
		StringUtils::utf16le_gbk(
				dest+info[i].pos*2, 
				info[i].tlen*2, 
				buf);
		string word = buf;
		parts.push_back(string(buf));
    }
	return 0;
}

int Normal::loadSyn()
{
	ifstream infile;
	infile.open(SYN_FILE, ios::in);
	if(!infile)
	{
		cerr << "[ERROR] Fail to load syn dict. 2" << endl;
		return -1;
	}
	string line;
	vector<string> vec;
	while(getline(infile, line))
	{
		spaceGary::StringSplit(line, vec, "\t");
		if(vec.size() < 4)
			continue;
		string word = vec[0];
		string syn = vec[3];
		hash_syn[word].push_back(syn);
		vec.clear();
	}
	return 0;

}
int Normal::Normalize_(const string& line, string& segment, string& norm_, SEGMENT_1* seg)
{
	char dest[4096];
	string line_tmp(line.c_str());
	spaceGary::StringTrim(line_tmp);
	string key = line_tmp;
	
	//半角转全角
	memset(dest,0x00,4096);
	EncodingConvertor::getInstance()->dbc2gchar(
		key.c_str(),
		(gchar_t*)dest,
		4096/2,
		true);
	vector<string> seg_parts;
	seg->Segment_(string(dest),seg_parts);
	stringstream oss;
	oss.str("");
	//cerr << "[INFO] normalizing to be:" << endl;
	string reform;
	segment = "";
	for(int i = 0; i < seg_parts.size(); i++)
	{
		//cerr << "\tori:"<< seg_parts[i] << endl;
		reform = seg_parts[i];
		segment += reform;
		if(hash_syn.find(seg_parts[i]) != hash_syn.end())
		{
			if(hash_syn[seg_parts[i]].size() > 0)
			{
				oss << (hash_syn[seg_parts[i]])[0];
				reform += (hash_syn[seg_parts[i]])[0];
				//for(int j = 0; j < hash_syn[seg_parts[i]].size(); j++)
				{
					//oss << (hash_syn[seg_parts[i]])[j] << "||";
					//reform += (hash_syn[seg_parts[i]])[j] + "||";
				}
			}
			else
				oss << seg_parts[i];
		}
		else
			oss << seg_parts[i];
		//cerr << "\treform:"<< reform << endl;
	}
	
	norm_ = oss.str();

	return 0;
}

int Normal::Normalize_seg(const string& line, string& norm_)
{
	char dest[4096];
	string line_tmp(line.c_str());
	spaceGary::StringTrim(line_tmp);
	string key = line_tmp;
	
	//半角转全角
	memset(dest,0x00,4096);
	EncodingConvertor::getInstance()->dbc2gchar(
		key.c_str(),
		(gchar_t*)dest,
		4096/2,
		true);
	vector<string> seg_parts;
	Segment_(graph,string(dest),seg_parts);
	stringstream oss;
	oss.str("");
	cerr << "[INFO] normalizing to be:" << endl;
	string reform;
	for(int i = 0; i < seg_parts.size(); i++)
	{
		reform = seg_parts[i];
		oss << seg_parts[i];
	}
	
	norm_ = oss.str();

	return 0;
}


