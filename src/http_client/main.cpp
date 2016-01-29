#include <stdio.h>
#include <stdlib.h>
#include "HttpClient.hpp"
#include "HttpRequest.hpp"
#include <service_log.hpp>
#include <strings.h>
#include <string>
//#include "gtest/gtest.h"
//#include "gflags/gflags.h"
//#include "glog/logging.h"
#include <iostream>
#include "../jsoncpp/json/json.h"

//using namespace ns_chaoli_utility;

using namespace std;

string FLAGS_query = "dahuashueihu";
string FLAGS_input = "query.input";

int test_file()
{
    HttpClient client(50, 50, 300);
    client.connect_server("127.0.0.1", 20155);
    //client.connect_server("10.138.76.30", 65668);
	
	string s;
	int line_num = 0;
	vector<string> parts;
	timeval start_time,end_time;

	while(getline(cin,s))
	{
		//cout<<s<<endl;
		gettimeofday(&start_time,NULL);
		line_num++;
		if(line_num % 10000 == 0)
			cerr<<line_num<<endl;
	//	if(!CStringUtilityChaoli::Trim(s))
	//		continue;
	//	if(!CStringUtilityChaoli::Split(s,parts,"\t"))
	//		continue;

		//_INFO("string process cost %d",WASTE_TIME_US(start_time));

		Json::Value root;
		Json::Value array_obj;
		Json::Value item;

		root["longitude"] = 116.403133;
		root["latitude"]= 39.947066;
		//root["query"] = UrlEncode("hello server");
		//root["request_id"]=UrlEncode("httpclient");

		string send_str = root.toStyledString();

		cout << "send_str:" << send_str;

    	HttpRequest req(HTTP10, POST, "/mspider");
		req.fillContent(
				send_str.size(),
				send_str.c_str());
    	char buf[40960];
    	int len = -1;
    	req.dump_to_buff(buf, 40960, len);

		//cerr<<"send buffer len is : "<<len<<endl;
		//cerr<<"send buffer is:\n"<<buf<<endl;

		string query = root["query"].toStyledString();
		//cerr<<"send query is:\n"<<UrlDecode(query)<<endl;

		//_INFO("encode http request cost %d",WASTE_TIME_US(start_time));

		std::string header;
		char body_buf[40960];
		uint32_t body_len = 0;
    	client.send(buf, len);
		//_INFO("send http request cost %d",WASTE_TIME_US(start_time));
		int32_t ret = client.recv(
				header,
				body_buf,
				40960,
				body_len
				);
	//	_INFO("recevie result cost %d",WASTE_TIME_US(start_time));

		//cout << "header:" <<
		//	header << endl;
		//cout << "body len:" <<
		//	body_len << endl;
		//cout << "body buf:" <<
		//	body_buf << endl;

		string content_buf = body_buf;
		cout << content_buf << endl;

		int32_t index_pos = content_buf.find("{");

		if(index_pos != -1)
		{
			content_buf = content_buf.substr(index_pos);
		}
		else
		{
			cerr << "find error!" << endl;
			return 0;
		}

		//_INFO("[query=%s,cost=%d]",parts[0].c_str(),WASTE_TIME_US(start_time));

		Json::Reader reader;
		Json::Value value;
		if(reader.parse(content_buf, value))
		{
			cout << "value size:" << 
			value["test"].size() << endl;
			string temp_str = value["test"].asString();
			cout << "result is:" << temp_str << endl;
		}
		else
		{
			cerr << "parse error!" << endl;
		}

		//_INFO("parse result cost %d",WASTE_TIME_US(start_time));

	//	sleep(1);

	}
	client.disconnect();

	return 0;
}


int main(int argc, char* argv[]) 
{
//	google::AllowCommandLineReparsing();
//	google::ParseCommandLineFlags(&argc, &argv, true);
//	::testing::InitGoogleTest(&argc, argv);
//	google::InitGoogleLogging(argv[0]);
//	FLAGS_log_dir = "../../log";
	/*
    sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(62668);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
	*/
	if(argc < 2 )
	{
		test_file();
	}
	else
	{
	//	FLAGS_query = argv[1];

	//    HttpClient client(50, 500, 500);
	//    client.connect_server("127.0.0.1", 62666);
	//    //client.connect_server("10.16.129.100", 62666);
	//
	//	timeval start_time;
	//	gettimeofday(&start_time,NULL);
	//
	//	Json::Value root;
	//	Json::Value array_obj;
	//	Json::Value item;
	//
	//	root["query"] = UrlEncode(FLAGS_query);
	//	root["request_id"]=UrlEncode("httpclient");
	//
	//	string send_str = root.toStyledString();
	//
	//	//cout << "send_str:" << send_str;
	//
	//    HttpRequest req(HTTP10, POST, "/mspider");
	//	req.fillContent(
	//			send_str.size(),
	//			send_str.c_str());
	//    char buf[40960];
	//    int len = -1;
	//    req.dump_to_buff(buf, 40960, len);
	//	//_INFO("encode http request cost %d",WASTE_TIME_US(start_time));
	//
	//	std::string header;
	//	char body_buf[40960];
	//	uint32_t body_len = 0;
	//    client.send(buf, len);
	//
	//	//_INFO("send http request cost %d",WASTE_TIME_US(start_time));
	//
	//	int32_t ret = client.recv(
	//			header,
	//			body_buf,
	//			40960,
	//			body_len
	//			);
	//
	//	//_INFO("recevie result cost %d",WASTE_TIME_US(start_time));
	//
	//	//cout << "header:" <<
	//	//	header << endl;
	//	//cout << "body len:" <<
	//	//	body_len << endl;
	//	//cout << "body buf:" <<
	//	//	body_buf << endl;
	//
	//	string content_buf = body_buf;
	//
	//	int32_t index_pos = content_buf.find("{");
	//
	//	if(index_pos != -1)
	//	{
	//		content_buf = content_buf.substr(index_pos);
	//	}
	//	else
	//	{
	//		cerr << "find error!" << endl;
	//		return 0;
	//	}
	//	
	//	//_INFO("send and receive cost %d",WASTE_TIME_US(start_time));
	//	_INFO("[query=%s,cost=%d]",FLAGS_query.c_str(),WASTE_TIME_US(start_time));
	//
	//	//Json::Reader reader;
	//	//Json::Value value;
	//	//if(reader.parse(content_buf, value))
	//	//{
	//	//	cout << "value size:" << 
	//	//		value["psmt_outputs"].size() << endl;
	//	//	for(int32_t i = 0; i < 
	//	//			value["psmt_outputs"].size(); i++)
	//	//	{
	//	//		string temp_str = UrlDecode(value["psmt_outputs"][i]["dest"].asString());
	//	//		cout << "result is:" << temp_str << endl;
	//	//	}
	//	//}
	//	//else
	//	//{
	//	//	cerr << "parse error!" << endl;
	//	//}

	//	client.disconnect();
	//
		//_INFO("parse result cost %d",WASTE_TIME_US(start_time));
	}


	//fprintf(stdout, "%s\n", buf);
 //   _INFO("req str=\n%s", buf);
    return 0;
}
