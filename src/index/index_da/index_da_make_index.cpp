/*************************************************
 *
 * Author: lichao
 * Create time: 2014  3ÔÂ 08 12Ê±07·Ö12Ãë
 * E-Mail: lichaotx020@sogou-inc.com
 * version 1.0
 *
*************************************************/


#include "Index_da.h"

using namespace std;
using namespace WHITE_BLACK_DA;


string FLAGS_input = "whitelist.vrqo";
string FLAGS_output = "whitelist.vrqo.index";
string type = "none";
int32_t main(int argc, char ** argv)
{

	if(argc == 3)
	{
		FLAGS_input = argv[1];
		FLAGS_output = argv[2];
	}else if(argc == 4)
	{
		FLAGS_input = argv[1];
		FLAGS_output = argv[2];
		type = argv[3];
	}else
	{
		cerr << "ERROR : lack of parameters ! " << endl;
		return -1;
	}

	WhiteBlackList_ whiteblacklist;
	cout << "input:" << FLAGS_input << endl;
	cout << "output:" << FLAGS_output << endl;
	cout << "type:" << type << endl;
	int32_t ret;
	if(type == "none")
	{
		ret = 
		whiteblacklist.MakeIndex(
				FLAGS_input.c_str(), 
			FLAGS_output.c_str());
	}
	if(type == "extend" || type== "uniq")
	{
		ret = whiteblacklist.MakeIndex( FLAGS_input.c_str(), 
			FLAGS_output.c_str(), type.c_str());
	}
	if(ret != 0)
	{
		cerr << "make index error!" << endl;
	}

	return 0;
}


