#include "HttpClient.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <poll.h>
//#include <service_log.hpp>
#include <errno.h>
#include <unistd.h>
#include <string.h>

//#include "gtest/gtest.h"
//#include "gflags/gflags.h"
//#include "glog/logging.h"
#include <iostream>

using namespace std;

#define SOCKET_SND_BUF_SIZE         (1024*1024)
#define SOCKET_RCV_BUF_SIZE         (1024*1024)

HttpClient::HttpClient(int _connect_timeout, int _recv_timeout, int _send_timeout) {
    connect_timeout = _connect_timeout;
    recv_timeout = _recv_timeout;
    send_timeout = _send_timeout;
    _fd = -1;
    state = FAIL;
}

HttpClient::~HttpClient() {
    if (_fd != -1) {
        close(_fd);
    }
}

int HttpClient::connect_server(std::string host, int port) {
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(host.c_str());
    if( (_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
		//LOG(ERROR) << "HttpClient::connect() create socket error";
        //_ERROR("HttpClient::connect() create socket error");
        return -1;
    }
    int options;
    options = fcntl(_fd, F_GETFL);
    fcntl(_fd, F_SETFL, options | O_NONBLOCK);
    bool succ = true;
    timeval begin;
    gettimeofday(&begin, NULL);
    if (connect(_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        pollfd connect_fd;
        connect_fd.fd = _fd;
        connect_fd.events = POLLOUT;
        int poll_ret = poll(&connect_fd, 1, connect_timeout);
        if (poll_ret <= 0 || !(connect_fd.revents & POLLOUT)) {
            succ = false;
        }
    }
    timeval end;
    gettimeofday(&end, NULL);
    long cost = end.tv_sec * 1000000 + end.tv_usec - begin.tv_sec * 1000000 - begin.tv_usec;
	//LOG(INFO) << "connect cost=" << cost;
    //_INFO("connect cost=%ld", cost);
    if (succ) {
      //  _INFO("HttpClient::connect() connect %s:%d succ", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
		//LOG(INFO) << "HttpClient::connect() connect " <<
			//	inet_ntoa(server_address.sin_addr) << ":" <<
			//	ntohs(server_address.sin_port);


        state = WORKING;
        options = SOCKET_SND_BUF_SIZE;
        setsockopt(_fd, SOL_SOCKET, SO_SNDBUF, &options, sizeof(int));
        options = SOCKET_RCV_BUF_SIZE;
        setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &options, sizeof(int));
        options = 1;
        setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(int));
        return _fd;
    }
    else 
	{
		//LOG(ERROR) << "HttpClient::connect() connect " <<
				//inet_ntoa(server_address.sin_addr) << ":" <<
			//	ntohs(server_address.sin_port) << " fail";

        //_ERROR("HttpClient::connect() connect %s:%d fail", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
        return -1;
    }
}

int HttpClient::recv(std::string& header, char* body_buf, unsigned int max_buf_len, unsigned int& body_len) {
    int sep_index;
    char part_body[READ_BUF_SIZE];
    int header_len = 0;
    int part_body_len = READ_BUF_SIZE;
    body_len = -1;

    int header_ret = read_http_header_timeout(header, part_body, header_len, part_body_len);

//	cerr<<"header len : "<<header_len<<endl;
//	cerr<<"part body len: "<<part_body_len<<endl;
//	cerr<<header<<endl;

    if (header_ret < 0) 
	{
		//LOG(ERROR) << "read http header error!";
        //_ERROR("read http header error");
		cerr<<"read http header error"<<endl;
        return INVALID_HEADER;
	}


    int idx = header.find("Content-Length:");
    if (idx != std::string::npos) 
	{
        sep_index = header.find("\r\n", idx);
        //body_len = atoi(header.substr(idx + strlen("Content-Length:"), sep_index - idx - strlen("Content-Length:")).c_str());
		sscanf(header.c_str()+idx,"Content-Length:%d",&body_len);
		//cerr<<"Content Length: "<<body_len<<endl;
        if (body_len > max_buf_len) 
		{
			//LOG(ERROR) << "max buf len error!";
			cerr<<"not enough buffer"<<endl;
            return NOT_ENOUGH_BUFF;
        }
        memcpy(body_buf, part_body, part_body_len);
        int need_to_read = body_len - part_body_len;
		//cerr<<"content need to read: "<<need_to_read<<endl;
		if(need_to_read <= 0)
		{	
			cerr<<"content length equl or less than 0"<<endl;
			//return INVALID_HEADER;
			return -1;
		}
        int read_ret = readn_timeout(body_buf + part_body_len, need_to_read);
		//cerr<<"read content length"<<read_ret<<endl;
        if (read_ret != need_to_read)
		{
			cerr<<"read timeout"<<endl;
			//LOG(ERROR) << "body len:" << body_len;
			//LOG(ERROR) << "part body len:" << part_body_len;
			//LOG(ERROR) << "read_ret:" << read_ret;
			//LOG(ERROR) << "need_to_read:" << need_to_read;
			//LOG(ERROR) << "read timeout!";
            return READ_TIMEOUT;
        }
    }
    else if (header.find("Transfer-Encoding: chunked") != std::string::npos) {
        body_buf = 0;
        int cur_len = part_body_len;
        int read_ret;
        while ((read_ret = readn_timeout(body_buf + cur_len, 1024)) == 1024) {
            if (cur_len + read_ret > max_buf_len) {
                return NOT_ENOUGH_BUFF;
            }
            cur_len = cur_len + 1024;
        }
        if (read_ret > 0) {
            cur_len += read_ret;
        } 
        char *cursor = body_buf;
        int chunk_len;
        while (1) {
            if (sscanf(cursor,"%x\r\n", &chunk_len) != 1) {
                body_len = 0;
                return ERROR_CHUNK_DATA;
            }
            if (chunk_len == 0) {
                break;
            }
            cursor = strstr(cursor,"\r\n");
            cursor += strlen("\r\n");
            memmove(body_buf + body_len, cursor, chunk_len);
            cursor = cursor + chunk_len + strlen("\r\n");
            body_len += chunk_len;
            body_buf[body_len] = '\0';
        }
        body_buf[body_len] = '\0';
    }
    else 
	{
		//LOG(ERROR) << "invalid error!";
        return INVALID_HEADER;
    }
    return 0;
}

int HttpClient::send(const char* buf, unsigned int buf_len) {
    return writen_timeout(buf, buf_len);
}

int HttpClient::disconnect() {
    close(_fd);
    _fd = -1;
    return 0;
}

int HttpClient::read_http_header_timeout(std::string& header, char* body_buf, int& header_len ,int& part_body_len) {
    char read_buf[READ_BUF_SIZE + 1];
    int n, pos;
    char *end;
    while (1) {
        n = readn_timeout(read_buf, READ_BUF_SIZE);
        if (n <= 0) {
            return -1;
        }
        read_buf[n] = '\0';
        end = strstr(read_buf, "\r\n\r\n");
        if (end != NULL) {
            pos = end - read_buf;
            char tmp_buf[READ_BUF_SIZE + 1];
            int left_header_len = pos + strlen("\r\n\r\n");
            memcpy(tmp_buf, read_buf, left_header_len);
            tmp_buf[left_header_len] = '\0';
            header = header + tmp_buf;
            part_body_len = n - left_header_len;
            memcpy(body_buf, read_buf + left_header_len, part_body_len);
            header_len = header.length();
            return 0;
        }
        else {
            header = header + read_buf;
        }
    }
    return 0;
}

int HttpClient::read_data(char* buf, int max_buf_len) {
    pollfd read_fd;
    read_fd.fd = _fd;
    read_fd.events = POLLIN;
    int poll_ret = poll(&read_fd, 1, recv_timeout);
    if (poll_ret <= 0 || !(read_fd.revents & POLLIN)) {
        return -1;
    }
    return read(_fd, (void*)buf, max_buf_len);
}

int HttpClient::readn_timeout(char* buf, int need_to_read) {
    char r_buf[READ_BUF_SIZE + 1];
    int n, len;
    int left = need_to_read;
    if (left <= 0) {
        return -1;
    }
    while (left > 0)
	{
        len = left > READ_BUF_SIZE ? READ_BUF_SIZE : left;

		//cerr<<"len is: "<<endl;

        n = read_data(r_buf, len);
        if (n <= 0) {
            return need_to_read - left;
        }
        memcpy(buf + (need_to_read - left), r_buf, n);
        left = left - n;
    }
	//cerr<<"need to read return "<<need_to_read<<endl;
    return need_to_read;
}

int HttpClient::writen_timeout(const char *buf, int need_to_write) {
    int left = need_to_write;
    int n;
    pollfd write_fd;
    write_fd.fd = _fd;
    write_fd.events = POLLOUT;
    int poll_ret;

	//printf("------- send buf is:%s",buf);

    while (left > 0) {
        poll_ret = poll(&write_fd, 1, send_timeout);
        if (poll_ret <= 0 || !(write_fd.revents & POLLOUT)) {
            return -1;
        }
        n = write(_fd, buf, left);
        if (n == 0) {
            return need_to_write - left;
        }
        buf = buf + n;
        left = left - n;
    }
    return need_to_write;
}
