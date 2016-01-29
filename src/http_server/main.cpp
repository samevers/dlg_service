#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <malloc.h>
#include <signal.h>
#include <unistd.h>
//#include <service_log.hpp>
#include "configuration.hpp"
#include "http_server.hpp"
#include "HttpHandler.hpp"
#include "Platform/log.h"

#include<iostream>
#include<string>
using namespace std;

//#include "gtest/gtest.h"
//#include "gflags/gflags.h"
//#include "glog/logging.h"

using namespace httpserver;

#define DEFAULT_CONFIG_FILE "http.cfg"
#define DEFAULT_CONFIG_KEYNAME "HTTP"
void sigterm_handler(int signo) {}
void sigint_handler(int signo) {}

void usage(const char *bin_name)
{
    printf("Usage:\n");
    printf("        %s [options]\n\n", bin_name);
    printf("Options:\n");
    printf("        -h:             Show help messages.\n");
    //printf("        -t:             Turn on trace mode.\n");
    printf("        -f file:        The configuration file.(default is \"%s\")\n", DEFAULT_CONFIG_FILE);
    printf("        -k key:         The configuration's root key.(default is \"%s\")\n", DEFAULT_CONFIG_KEYNAME);
}

int main(int argc, char* argv[]) {
    mallopt(M_MMAP_THRESHOLD, 256*1024);
    setenv("LC_CTYPE", "zh_CN.gbk", 1);
    const char *config_filename = DEFAULT_CONFIG_FILE;
    const char *config_keyname = DEFAULT_CONFIG_KEYNAME;
    char opt_char;

    while ((opt_char = getopt(argc, argv, "f:k:th")) != -1) {
        switch (opt_char) {
            case 'f':
                config_filename = optarg;
                break;
            case 'k':
                config_keyname = optarg;
                break;
            case 'h':
                usage(argv[0]);
                return EXIT_SUCCESS;
            default:
                usage(argv[0]);
				break;
                return EXIT_FAILURE;
        }
    }

    close(STDIN_FILENO);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, &sigterm_handler);
    signal(SIGINT, &sigint_handler);

	//google::AllowCommandLineReparsing();
	//google::ParseCommandLineFlags(&argc, &argv, true);
	//::testing::InitGoogleTest(&argc, argv);
	//google::InitGoogleLogging(argv[0]);
	//FLAGS_log_dir = "../../log";

	// init
	//ns_query_rewrite::QueryRewrite::Init("data/base/query_rewrite");
	//ns_query_rewrite::QueryRewrite::LoadIndex("data/base/query_rewrite");

	std::cerr << "gcc version:" << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << std::endl;
	//cerr << "http server starting" << endl;
    {
        int ret;

        Configuration config;
        if (config.open(config_filename, config_keyname)) 				// 打开配置文件
		{
			std::cerr << "open configure file fail, file name:" << config_filename << ", key:" << config_keyname;
            exit(-1);
        }
		
		// http server 部分
        Http_Server* http;
        HttpHandler* handler;
        handler = new HttpHandler();
        ret = handler->open(config);									// 加载我的程序一系列词典
        if (ret < 0) 
		{
            exit(-1);
        }

        http = new Http_Server();
        ret = http->open(config.receiver_num, config.thread_stack_size, config.listen_port);	// 根据配置文件创建 http_server
        if (ret < 0) 
		{
            exit(-1);
        }
        http->register_handler(handler);								// 将 handler 注册到 http_server
		//	LOG(INFO) << "http server start ok";
        http->activate();												// 激活 服务
																		// 后面的任务就交给 http_server.cpp 中的svc 来完成。
        pause();
        http->stop();
        delete http;
        delete handler;
    }

	//cerr << "http server stop ok";
    return 0;
}
