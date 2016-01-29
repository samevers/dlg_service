/*************************************************
 *
 * Author: lichao
 * Create time: 2014  2ÔÂ 26 16Ê±33·Ö02Ãë
 * E-Mail: lichaotx020@sogou-inc.com
 * version 1.0
 *
*************************************************/


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
#include <math.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <boost/algorithm/string.hpp>

#include "locate_getresult.h"


using namespace std;
using namespace boost;


static string PRE_DATA_DIR = "../../data/location";
static string FILE_LOCATE_NAME="news.info";
//const long double EARTH_RADIUS = 6378.137;
const long double EARTH_RADIUS = 6370996.81;

//std::vector<ResultData> LocationResultGet::locateInfoALL;

LocationResultGet::LocationResultGet()
{
}

LocationResultGet::~LocationResultGet()
{
}

void LocationResultGet::LocationResultGet::loaddiclocate()
{
	ostringstream oss;
	oss.clear();
	oss.str("");
	oss << PRE_DATA_DIR << "/" << FILE_LOCATE_NAME;
	string regFile;
	regFile = oss.str().c_str();
	oss.clear();
	oss.str("");
	fstream fin(regFile.c_str());
	cerr << regFile.c_str()<< "\tfilename" << endl;
	if (!fin)
	{
		cerr<<"MSG_INFO: no such the file["<<regFile.c_str()<<"]!!"<<endl;
		return;
	}
	cerr<<"Begin to load file:["<<regFile.c_str()<<"]..."<<endl;
	string line;
	while(getline(fin,line))
	{
		trim(line);
		string tmp_split = "\t";
		std::vector<string> eles;
		string tmp_queryString = line + tmp_split;
		std::string::size_type tmp_pos;
		for (int j = 0; j < tmp_queryString.length(); j++)
		{
			tmp_pos=tmp_queryString.find(tmp_split,j);
			if (tmp_pos != -1)
			{
				eles.push_back(tmp_queryString.substr(j,tmp_pos-j));
				j = tmp_pos+ tmp_split.length()-1;
			}
		}
		if(eles.size() != 6)
		{
			cerr << "data is error: " << line;
			continue;
		}
		ResultData tmpResultData;
		try{
			stringstream oss;
			oss.str("");
			oss << eles[0].c_str();
			oss >> tmpResultData.longitude;
			oss.clear();
			oss.str("");
			oss << eles[1].c_str();
			oss >> tmpResultData.latitude;
			oss.clear();
			oss.str("");
			cout << eles[0].c_str() << "\ttaoli\t" << eles[1].c_str() << endl;
			printf("%Lf\t::\t%Lf\n",tmpResultData.longitude, tmpResultData.latitude );

			tmpResultData.lng = eles[0];
			tmpResultData.lat = eles[1];
			tmpResultData.title = eles[2];
			tmpResultData.source = eles[3];
			tmpResultData.datatime = eles[4];
			tmpResultData.url = eles[5];
			locateInfoALL.push_back(tmpResultData);
		}
		catch(boost::exception & err)
		{
			cerr << "data is error: " << line;
			continue;
		}
	}
}

long double rad(long double d)
{
	return d * 3.1415926 / 180.0;
}

long double GetDistance(long double lat1, long double lng1, long double lat2, long double lng2)
{
	long double radLat1 = rad(lat1);
	long double radLat2 = rad(lat2);
	long double a = radLat1 - radLat2;
	long double b = rad(lng1) - rad(lng2);
 
	long double s = 2 * asin(sqrt(pow(sin(a/2),2) + cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
	s = s * EARTH_RADIUS;
	s = round(s * 10000) / 10000;
	return s;
}

bool LocationResultGet::GetLocationResult(long double longitude, long double latitude, std::vector<ResultData> & locateResult, long double distance)
{
	//printf("%f\t%f\t%f\t%d\n", longitude, latitude, distance, locateInfoALL.size());
	vector<long double> vecDistance;
	for(vector<ResultData>::iterator it = locateInfoALL.begin(); it != locateInfoALL.end(); it++)
	{
		//printf("%Lf\t%Lf\t%Lf\t%d\n", it->longitude, it->latitude, distance, locateInfoALL.size());
		//cerr << it->longitude << "\t" << it->latitude << "\t" << it->title << "\t" << it->url << "\t" << it->source << "\t"  << it->datatime << endl;
		long double tmpDis = GetDistance(latitude, longitude, it->latitude, it->longitude);
		if(tmpDis >= distance)
		{
			continue;
		}
		size_t vit = 0;
		for(;vit < vecDistance.size();vit++)
		{
			if(vecDistance[vit] > tmpDis)
			{
				break;
			}
		}
		if(vit == vecDistance.size())
		{
			vecDistance.push_back(tmpDis);
			locateResult.push_back(*it);
		}
		else{
			 vecDistance.insert(vecDistance.begin() + vit, tmpDis);
			 locateResult.insert(locateResult.begin() + vit, *it);
		}
		//cerr << tmpDis << "\tdistance" << endl;
		/*if(tmpDis  < distance)
		{
			locateResult.push_back(*it);
			//cerr << it->longitude << "\t" << it->latitude << "\t" << it->title << "\t" << it->url << "\t" << it->source << "\t"  << it->datatime << "\tlitao" << endl;
		}*/
	}
	return true;
}

