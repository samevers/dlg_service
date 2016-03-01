#include "Segment.h"
#include "general.h"

using namespace std;

const int32_t MAXLEN_ = 20480;

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
	char dest[MAXLEN_];
	// init segment
	SEGMENT_1 * seg;
	seg = SEGMENT_1::getInstance();
	seg->Init();
	while(getline(infile, line))
	{
		spaceGary::StringTrim(line);
		vector<string> seg_parts;
		spaceGary::StringSplit(line, seg_parts, "##_##");
		if(seg_parts.size() == 2)
		{
			string key = seg_parts[0];
			string ans = seg_parts[1];
	
			// key
			//半角转全角
			memset(dest,0x00,MAXLEN_);
			EncodingConvertor::getInstance()->dbc2gchar(
				key.c_str(),
				(gchar_t*)dest,
				MAXLEN_/2,
				true);
			seg->Segment_(string(dest),seg_parts);
			for(int i = 0; i < seg_parts.size(); i++)
			{
				cout << seg_parts[i] << " ";
			}
			cout << "##_##";
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
			cout << ans;
		}
		else if(seg_parts.size() == 1){
			//半角转全角
			memset(dest,0x00,MAXLEN_);
			EncodingConvertor::getInstance()->dbc2gchar(
				seg_parts[0].c_str(),
				(gchar_t*)dest,
				MAXLEN_/2,
				true);
			seg_parts.clear();
			//cerr << "-----seg_parts[0]: " << seg_parts[0] << endl;
			//cerr << "-----dest: " << dest << endl;
			seg->Segment_(string(dest),seg_parts);
			for(int i = 0; i < seg_parts.size(); i++)
			{
				cout << seg_parts[i] << " ";
			}
		}
		cout << endl;
	}
	SEGMENT_1::releaseInstance();
	return 0;
}
