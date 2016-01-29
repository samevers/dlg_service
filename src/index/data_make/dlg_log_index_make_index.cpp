/*************************************************
 *
*************************************************/


#include "dlg_log_index.h"

DEFINE_string(input,"../../data/PC2015_234.gbk","input log file");
DEFINE_string(output,"../../data/pc_log.index","output index file");


int main(int argc, char ** argv)
{
	google::ParseCommandLineFlags(&argc, &argv, true);
	if(CDlgLogIndex::Init() != 0)
	{
		_ERROR("init error!");
		return -1;
	}

	
	if(CDlgLogIndex::MakeIndex(
			FLAGS_input.c_str(),
			FLAGS_output.c_str()) != 0)
	{
		return -1;
	}

	return 0;
}
