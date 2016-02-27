/*************************************************
 *
 * Author: 
 * Create time: 2015 9ÔÂ 21 09Ê±59·Ö42Ãë
 * E-Mail: @sogou-inc.com
 * version 1.0
 *
*************************************************/


#include "doc_index.h"

using namespace std;

string input_file = "base_data.txt";
string output_file = "../../data/base_data.index";
const string stopword_file = "../../../data/stop/stopword.txt";
const char* dir_base = "../../../";
int main(int arg,char** argv)
{
	if(arg == 3)
	{
		input_file = argv[1];
		output_file = argv[2];
	}

	CDlgIndex dlg_index;
	if( dlg_index.Init(dir_base) != 0)
	{
		cerr<<"init dlg index error !"<<endl;
		//return -1;
	}
	if( dlg_index.LoadStopword(stopword_file.c_str()) != 0)
	{
		cerr<<"load stopword file error !"<<endl;
		//return -1;
	}

	if(dlg_index.MakeIndex(input_file.c_str(),output_file.c_str()) != 0)
	{
		cerr<<"make index error !"<<endl;
		return -1;
	}

	cerr<<"make index ok!"<<endl;

	return 0;

}
