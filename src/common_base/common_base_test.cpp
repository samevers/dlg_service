/*************************************************
 *
 * Author: 
 * Create time: 2015 6ÔÂ 11 15Ê±58·Ö34Ãë
 * E-Mail: @sogou-inc.com
 * version 1.0
 *
*************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

#include "gtest/gtest.h"
#include "database.h"

using namespace std;

TEST(DataBase, All)
{
	CMysql mysql("127.0.0.1", "root", "123456", "db_dg_sys", "set names gbk");
	mysql.connect();
	mysql.open();
	CMysqlExecute executor(&mysql);
	executor.execute("set names gbk");

	//string sql("select uid, phone_num, add_time from tb_user_info where content like '%\"type\": \"5\"%' limit 5");
	string sql("select uid, detail_info from tb_user_info");
	CMysqlQuery query(&mysql);
	int ret = query.execute(sql.c_str());
	if(ret == 0) 
	{
		int count = query.rows();
		for(int i = 0; i < count; ++i) 
		{
			CMysqlRow *row = query.fetch();
			std::cout << "uid=" << row->fieldValue("uid") << endl;
			std::cout << "detail_info=" << 
				row->fieldValue("detail_info") << endl;
		}
	}
}

int main(int argc, char ** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}











