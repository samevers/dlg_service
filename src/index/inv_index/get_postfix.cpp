/*************************************************
 *
 * Author: 
 * Create time: 2015 9月 21 18时23分27秒
 * E-Mail: @sogou-inc.com
 * version 1.0
 *
*************************************************/


#include "../../include/service_log.hpp"
#include "../../include/gary_common/gary_string.h"
#include "../../include/gary_common/gary_common.h"
#include "WordSegmentor4/Segmentor/Graph.h"
#include "Platform/bchar.h"
#include "Platform/encoding.h" 
#include "Platform/encoding/support.h"
#include <iostream>
#include <string>
#include <vector>

#include "dlg_index.h"

using namespace std;
using namespace spaceGary;

int main(int argc,char** argv)
{
	EncodingConvertor::initializeInstance(); 
    analysis::DicTree::instance()->loadDic();

    analysis::TGraph graph(65536);
    graph.open(analysis::DicTree::instance());
	
	char line[4096];
	int line_num = 0;
	while(fgets(line,4096,stdin))
	{
		line_num ++;
		if(line_num % 100000 == 0)
		{
			_INFO("read line [%d]",line_num);
		}
		string line_str = line;
		StringTrim(line_str);
		vector<string> parts;
		StringSplit(line_str,parts,"\t");
		if(parts.size() != 9)
		{
			_INFO("line [%d][%s] format error!",
					line_num,line_str.c_str());
			continue;
		}

		//
		//半角转全角
		char dest[4096];
		memset(dest,0x00,4096);
		EncodingConvertor::getInstance()->dbc2gchar(
				parts[0].c_str(),
				(gchar_t*)dest,
				4096/2,
				true);
		
		vector<string> seg_parts;
		CPoiIndex::GraphSegment(
				graph,string(dest),seg_parts);

		string dlg_name = "";

		if(seg_parts.size() < 2)
			continue;

		memset(dest,0x00,4096);
		EncodingConvertor::getInstance()->sbc2dbc(
				parts[parts.size()-1].c_str(),
				dest,
				4096);
		parts[parts.size()-1] = dest;

		memset(dest,0x00,4096);
		EncodingConvertor::getInstance()->sbc2dbc(
				parts[parts.size()-2].c_str(),
				dest,
				4096);
		parts[parts.size()-2] = dest;

		//输出结果
		cout<<seg_parts[seg_parts.size()-1]<<endl;
		if(seg_parts.size() > 2)
		{
			cout<<seg_parts[seg_parts.size()-2]<<"#_#"<<
				seg_parts[seg_parts.size()-1]<<endl;
		}

		}

	return 0;
}
