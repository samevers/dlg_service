//#include "Index_da.h"
#include "general.h"
#include "../../segment/Segment.h"

using namespace std;

const static int MAXSTRLEN=10240;
const static int MAXQUERYLEN=MAXSTRLEN/2;

SEGMENT_1* seg;	

int32_t DeleteSpace(string & input,
		string & output, char del)
{
	for(size_t i = 0; i < input.size(); i++)
	{
		if(input[i] < 0)
		{
			if(i + 1 >= input.size())
			{
				return -1;
			}
			else
			{
				output.push_back(input[i]);
				output.push_back(input[i+1]);
				i+= 1;
			}
		}
		else
		{
			if(input[i] == del)
			{
			}
			else
			{
				output.push_back(input[i]);
			}
		}
	}
	return 0;
}
// split
int split_sam(vector<string>& vec, string line, string blank)
{
	if(line.length() < 1)
		return 0;
	int start = 0;
	int loc;
	string sub;

	while((loc = line.find(blank.c_str())) != -1)
	{
		sub = line.substr(0, loc);
		vec.push_back(sub);
		start = loc + blank.length();
		line = line.substr(start, line.length() - start);
	}
	vec.push_back(line);
	return 1;
}

//Êä³öindexÎÄ¼þ
int32_t PreProcess_dlg_cor(string filename) 
{
	ifstream infile;
	infile.open(filename.c_str(), ios::in);
	if(!infile)
	{
		cerr << "[ERROR] Fail to open inputfile: " << filename << endl;
		return -1;
	}
	cerr << "[INFO] open file : " << filename << endl;
	string line;
	string query;
	string extend;
	vector<string> vec;
	vector<string> parts;
	map<string, string> hash_uniq;
	while(getline(infile, line))
	{
		vec.clear();
		spaceGary::StringTrim(line);
		spaceGary::StringSplit(line, vec, "##_##");
		parts.clear();
		seg->Segment_(
			vec[0],
			parts);
		if(parts.size() < 1)
		{
			cerr <<"[note] segment size lt 1" << endl;
			cerr <<"[note] query = " << vec[0] << endl;
			if(hash_uniq.find(vec[0]) != hash_uniq.end())
			{
				hash_uniq[vec[0]] += "##-##" + vec[1];
			}else
			{
				hash_uniq[vec[0]] = vec[1];
			}
			continue;
		}
		string dest_norm = "";
		for(int i = 0; i < parts.size(); i++)
		{
			dest_norm += parts[i];
		}
		query = dest_norm;
 		if(hash_uniq.find(query) != hash_uniq.end())
		{
			hash_uniq[query] += "##-##" + vec[1];
		}else
		{
			hash_uniq[query] = vec[1];
		}

	}

	map<string, string>::iterator iter;
	for(iter = hash_uniq.begin(); iter != hash_uniq.end(); iter ++)
	{
		cout << iter->first << "##_##" << iter->second << endl;
	}
	return 0;

}

int main(int arg, char* argv[])
{
	seg = SEGMENT_1::getInstance();
	seg->Init();
	string filename = argv[1];
	PreProcess_dlg_cor(filename);
	return 0;
}
