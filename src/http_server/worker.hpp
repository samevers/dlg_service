#ifndef HTTP_WORKER_HPP
#define HTTP_WORKER_HPP
#include <string>
#include "socket_handle.hpp"
#include "linked_list.hpp"
#include "HttpResponse.hpp"


class Worker {
    public:
        std::string header;
        char* content_buf;
        int content_len;
		httpserver::HttpResponse* response;
        std::string request_url;
        bool keep_alive;

        Worker() {
            content_buf = NULL;
            content_len = 0;
            response = NULL;
            // 默认是短连接！
            keep_alive = false;
        }

        ~Worker() {
            if (content_buf != NULL) {
                free(content_buf);
            }
            if (response != NULL) {
                delete response;
            }
        }

        int genContentBuf() {
            if (content_len == 0) {
                return 0;
            }
            content_buf = (char*)malloc(content_len + 10);
            return 0;
        }

        SocketHandle * handle;
        linked_list_node_t task_list_node;

};

#endif
