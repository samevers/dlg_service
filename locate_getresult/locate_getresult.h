/*************************************************
 *
 * Author: lichao
 * Create time: 2014  2ÔÂ 26 18Ê±28·Ö50Ãë
 * E-Mail: lichaotx020@sogou-inc.com
 * version 1.0
 *
*************************************************/

#ifndef QUERY_REWRITE_H_
#define QUERY_REWRITE_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;


struct ResultData{
	long double longitude;
	long double latitude;
	std::string lng;
	std::string lat;
	std::string title;
	std::string url;
	std::string source;
	std::string datatime;
};



class LocationResultGet
{

public:
	LocationResultGet();
	~LocationResultGet();
	bool GetLocationResult(long double longitude, long double latitude, std::vector<ResultData> & locateResult, long double distance);
	void loaddiclocate();

private:
	std::vector<ResultData> locateInfoALL;
	//void loaddiclocate();



}; 


#endif
