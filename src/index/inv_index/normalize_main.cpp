#include "normalize.h"
using namespace std;


int main(int arg, char** argv)
{
	if(arg !=  2)
	{
		cerr << "[INFO] Please input a filename!" << endl;
		return -1;
	}
	string inputfile = argv[1];
	ifstream infile;
	infile.open(inputfile.c_str(), ios::in);
	if(!infile)
	{
		cerr << "[ERROR] Fail to open filename!" << endl;
		return -1;
	}
	string line;
	char dest[4096];
	Normal norm;
	norm.Init();
	int32_t num = 0;
	SEGMENT_1* seg = SEGMENT_1::getInstance();
	seg->Init();

	while(getline(infile, line))
	{
		spaceGary::StringTrim(line);
		vector<string> seg_parts;
		spaceGary::StringSplit(line, seg_parts, "##_##");
		string key = seg_parts[0];
		string ans = seg_parts[1];

		/*
		if(key.length() < 1 || key.length() > 200 || ans.length() < 1 || ans.length() > 200)
		{
			cerr << "[note] length of key or ans is illegual!" << endl;
			cerr << "line = " << line << endl;
			cerr << "key = " << key << endl;
			cerr << "ans = " << ans << endl;
			continue;
		}
		*/
		string key_norm;
		string seg_norm;
		num ++;
		if(key.length() > 100)
		{
			cerr << "[INFO] key is too long, line number "<< num<< endl;
			continue;
		}
		if(norm.Normalize_(key, seg_norm, key_norm, seg) == -1)
		{
			cerr << "[ERROR]: Fail to get Normalize_() outcome." << endl;
			continue;
		}
		//cout << key_norm;
		//cout << "##_##";
		// ans
		/*
		 * seg_parts.clear();
		memset(dest,0x00,4096);
		EncodingConvertor::getInstance()->dbc2gchar(
			ans.c_str(),
			(gchar_t*)dest,
			4096/2,
			true);
		Segment_(graph,string(dest),seg_parts);
		for(int i = 0; i < seg_parts.size(); i++)
		{
			cout << seg_parts[i];
		}
		*/
		//cout << ans;
		//cout << endl;
		//
		cout << seg_norm << "##_##"<< ans << endl;
	}
	return 0;
}
