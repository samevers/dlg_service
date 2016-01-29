#include "HttpHandler.hpp"
#include "http_server.hpp"
#include "../jsoncpp/json/json.h"
//#include "../chaoli_utility/chaoli_utility.h"

#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

//using namespace ns_chaoli_utility;
static string  DIR_BASE = "../../data";

namespace httpserver 
{


HttpHandler::HttpHandler()
{
	m_location_result = new LocationResultGet();
	cerr << "add locate get" << endl;
	m_location_result->loaddiclocate();		// 加载我的功能词典
/*	
	da_search = new DA_SEARCH();
	doc_search = new DOC_SEARCH();
	da_search.Init(DIR_BASE.c_str());
	doc_search.Init(DIR_BASE.c_str());
*/	

}

HttpHandler::~HttpHandler()
{
	
	if(NULL != m_location_result)
	{
		delete m_location_result;
		m_location_result = NULL;
	}
	
}


unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}


std::string HttpHandler::UrlEncode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)str[i]) || 
            (str[i] == '-') ||
            (str[i] == '_') || 
            (str[i] == '.') || 
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ')
            strTemp += "+";
        else
        {
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] % 16);
        }
    }
    return strTemp;
}


int HttpHandler::handle_worker(Worker* worker) {
    int ret = generate_response(worker);		// 针对 worker 给出的请求内容，进行后台处理；
    m_server->retrieve_worker(worker);			// 将 处理结果 发送给 worker 返回
    return ret;
}

int HttpHandler::setServer(Http_Server* server) {
    m_server = server;
    return 0;
}


/*int32_t HttpHandler::generate_json_result(
		string & json_result)
{
	json_result = "";

	Json::Value root;
	//Json::Value array_psmt_inputs;
	//Json::Value array_psmt_outputs;
	Json::Value array_result_outputs;
	Json::Value item;
	int returnLen = 10;
	if(returnLen > locateResult.size())
	{
		returnLen = locateResult.size();
	}
	cout << returnLen << endl;

	
	for(int i = 0; i < returnLen; i++)
	{
		Json::Value temp_item;
		temp_item["longitude"]=locateResult[i].lng;
		temp_item["latitude"]=locateResult[i].lat;
		temp_item["title"]=UrlEncode(locateResult[i].title);
		temp_item["url"]=UrlEncode(locateResult[i].url);
		temp_item["source"]=UrlEncode(locateResult[i].source);
		//locateResult[i].datatime = "周末";
		temp_item["datetime"]=UrlEncode(locateResult[i].datatime);
		array_result_outputs.append(temp_item);
		cout << locateResult[i].lng << "\t" << locateResult[i].lat << "\t" << locateResult[i].title << endl;
	}
	root["list"] = array_result_outputs;

	json_result = root.toStyledString();
	//cout << json_result << "\tlitao" << endl;
	return 0;
}
*/

int32_t HttpHandler::generate_json_result(
		string & json_result)
{
	json_result = "";

	Json::Value root;
	//Json::Value array_psmt_inputs;
	//Json::Value array_psmt_outputs;
	Json::Value array_result_outputs;
	Json::Value item;
	int returnLen = 10;
	if(returnLen > locateResult.size())
	{
		returnLen = locateResult.size();
	}
	cout << returnLen << endl;

	
	for(int i = 0; i < returnLen; i++)
	{
		Json::Value temp_item;
		temp_item["longitude"]=locateResult[i].lng;
		temp_item["latitude"]=locateResult[i].lat;
		temp_item["title"]=UrlEncode(locateResult[i].title);
		temp_item["url"]=UrlEncode(locateResult[i].url);
		temp_item["source"]=UrlEncode(locateResult[i].source);
		//locateResult[i].datatime = "周末";
		temp_item["datetime"]=UrlEncode(locateResult[i].datatime);
		array_result_outputs.append(temp_item);
		cout << locateResult[i].lng << "\t" << locateResult[i].lat << "\t" << locateResult[i].title << endl;
	}
	root["list"] = array_result_outputs;

	json_result = root.toStyledString();
	//cout << json_result << "\tlitao" << endl;
	return 0;
}

// 返回后台处理逻辑的结果
int HttpHandler::generate_response(Worker* worker) 
{
	if(NULL == worker->response)
	{
		worker->response = new HttpResponse(HTTP10,
				SC200);
	}

	string request = 
		worker->content_buf;

	cerr << "[INFO] in generate_response, request:" << request << endl;
	Json::Reader reader;
	Json::Value value;

	//string query = "";
	long double longitude;
	long double latitude;
	long double distance;

	if(reader.parse(request, value))
	{
		//query = value["query"].asString();
		longitude = value["longitude"].asDouble();
		latitude = value["latitude"].asDouble();
		distance = value["distance"].asDouble();
		//cerr<<"query is:"<<query<<endl;
		printf("%Lf\t%Lf\t%Lf\n", longitude, latitude, distance);
		//cerr << longitude << "\t" << latitude << "\t" <<  distance << "\t" << endl;
	}
	else
	{
		cerr << "parse json error!" << endl;
	}

	longitude = 116.403133;
	latitude = 39.947066;
	distance = 10000000000;
	locateResult.clear();
	if(!m_location_result->GetLocationResult(longitude, latitude, locateResult, distance))
	{
		cerr << "get location error:" << request << endl;
		return -1;
	}

	//map<string, string> da_vec;
	//vector<string> tmp;
	//da_search.GetIndexResults(query, da_vec, tmp);


	string json_result = "";
	generate_json_result(json_result);		// 生成 json 数据结果

	worker->response->fillContent(json_result.size(), 	// 读取 json 结果，返回给用户；
			json_result.c_str());

    return 0;
}



} // namepspace httpserver
