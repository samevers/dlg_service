#include "http_server.hpp"
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/tcp.h>
#include <assert.h>
#include <poll.h>
#include <iostream>
using namespace std;

//#include "gtest/gtest.h"
//#include "gflags/gflags.h"
//#include "glog/logging.h"

//#include <service_log.hpp>
//
using namespace httpserver;

Http_Server::Http_Server()
{
	_epoll_fd = INVALID_FD;
	_pipe_read = INVALID_FD;
	_pipe_write = INVALID_FD;
	_socket_server_listen = INVALID_FD;
	_stop_task = 0;
	_epoll_ready_event_num = 0;
	_now_connections = 0;

	pthread_mutex_init(&_epoll_mutex, NULL);
	pthread_mutex_init(&_map_mutex,NULL);
	pthread_mutex_init(&_connections_mutex,NULL);
}

Http_Server::~Http_Server()
{
	::close(_epoll_fd);
	::close(_pipe_read);
	::close(_pipe_write);
	::close(_socket_server_listen);
	pthread_mutex_destroy(&_epoll_mutex);
	pthread_mutex_destroy(&_map_mutex);
	pthread_mutex_destroy(&_connections_mutex);

	for (std::map<int, SocketHandle*>::const_iterator it = m_fd_set.begin(); it != m_fd_set.end(); ++it)
	{
		::close(it->second->get_fd());
		delete it->second;
	}
}

int Http_Server::open(size_t thread_num, size_t stack_size, int port)
{
	if ((_epoll_fd = epoll_create(MAX_FD_NUM)) == -1)
	{
		fprintf(stderr,"error: epoll create fail!!\n");
		return -1;
	}
	if (create_pipe())
	{
		fprintf(stderr,"error: create pipe fail!! \n");
		return -1;
	}
	if (create_listen(_socket_server_listen, port))
	{
		fprintf(stderr,"error: create listen fail !! \n");
		return -1;
	}

	add_input_fd(_pipe_read);
	add_input_fd(_socket_server_listen);

	return task_base::open(thread_num, stack_size);		// task
}

int Http_Server::stop()
{
	_stop_task = 1;
	write(_pipe_write, &_stop_task, 1);
	join();
	return 0;
}

// 具体的系统服务逻辑代码
int Http_Server::svc()
{
	int fd,new_fd;

	while (!_stop_task)
	{
		cerr << "[INFO] task ...." << endl;
		pthread_mutex_lock(&_epoll_mutex);
		if (_stop_task) 
		{
			pthread_mutex_unlock(&_epoll_mutex);
			break;
		}

		if (_epoll_ready_event_num <= 0)
			_epoll_ready_event_num = epoll_wait(_epoll_fd, _epoll_ready_event, MAX_EPOLL_EVENT_NUM, -1);
		if (_epoll_ready_event_num-- < 0)
		{
			pthread_mutex_unlock(&_epoll_mutex);
			if (errno == EINTR)
				continue;	
			else
				break;
		}

		fd = _epoll_ready_event[_epoll_ready_event_num].data.fd;
		if (fd == _socket_server_listen)
		{
			while ((new_fd = accept(fd, NULL, NULL)) >= 0)
			{
				pthread_mutex_lock(&_connections_mutex);				
				if(_now_connections>=MAX_FD_NUM || new_fd >= 1024)
				{
					cerr << "new connection error,because the max connection" << MAX_FD_NUM << "is achieved" << endl;
					close(new_fd);
					pthread_mutex_unlock(&_connections_mutex);
				}
				else
				{
					_now_connections++;							
					pthread_mutex_unlock(&_connections_mutex);
					set_socket(new_fd, O_NONBLOCK);
					create_handle(new_fd,LISTEN_PORT_1);
					add_input_fd(new_fd);
					cerr << "new connection on socket:" << new_fd;
				}
			}
			pthread_mutex_unlock(&_epoll_mutex);
			continue;
		}
		if (fd == _pipe_read)
		{
			pthread_mutex_unlock(&_epoll_mutex);
			continue;
		}




		del_input_fd(fd);
		pthread_mutex_unlock(&_epoll_mutex);

		recv_request(fd);											// 收发服务请求
	}

	return 0;
}

int Http_Server::create_pipe()
{
	int options;
	int pipe_fd[2];

	if (pipe(pipe_fd))
		return -1;

	_pipe_read = pipe_fd[0];
	_pipe_write = pipe_fd[1];

	for (int i=0; i<2; i++)
	{
		if ((options = fcntl(pipe_fd[i], F_GETFL)) == -1)
			return -1;
		if (fcntl(pipe_fd[i], F_SETFL, options | O_NONBLOCK) == -1)
			return -1;
	}

	return 0;
}

int Http_Server::create_listen(int &socket_fd, unsigned short port)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		return -1;
	if (set_socket(socket_fd, O_NONBLOCK))
		return -1;
	if (bind(socket_fd, (const sockaddr*)&addr, sizeof addr))
		return -1;
	if (listen(socket_fd, MAX_FD_NUM))
		return -1;

	return 0;
}

int Http_Server::set_socket(int fd, int flag)
{
	int options;
	options = SOCKET_SND_BUF_SIZE;
	setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &options, sizeof(int));
	options = SOCKET_RCV_BUF_SIZE;
	setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &options, sizeof(int));
	options = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(int));
	options = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, options | flag);
	int on = 1;
	int ret = -1;
	ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on));
	return ret;
}

int Http_Server::add_input_fd(int fd)
{
	epoll_event event;
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = fd;
	epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event);

	return 0;
}

int Http_Server::del_input_fd(int fd)
{
	epoll_event event;
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = fd;
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, &event);

	return 0;
}

int Http_Server::create_handle(int fd, int flag)
{
	SocketHandle * newhdle = new SocketHandle;
	newhdle->set_flag(flag);
	newhdle->set_fd(fd);
	insert_handle(fd,newhdle);

	return 0;
}

int Http_Server::insert_handle(int fd,SocketHandle * hdle)
{
	pthread_mutex_lock(&_map_mutex);
	std::pair< std::map<int, SocketHandle*>::iterator , bool> ret = 
		m_fd_set.insert(std::map<unsigned int, SocketHandle*>::value_type(fd, hdle));
	pthread_mutex_unlock(&_map_mutex);
	if(!ret.second)
	{
		cerr << "<btlswkxt> insert map error!!" << endl;
		return -1;
	}

	return 0;
}

SocketHandle * Http_Server::get_handle(int fd)
{	
	pthread_mutex_lock(&_map_mutex);
	std::map<int, SocketHandle *>::iterator it = m_fd_set.find(fd);
	if (it != m_fd_set.end()) 
	{
		it->second->acquire();
		pthread_mutex_unlock(&_map_mutex);
		return it->second;
	}
	pthread_mutex_unlock(&_map_mutex);
	cerr << "<btlswkxt> do not get handle !!" << endl;
	return NULL;
}

int Http_Server::release_handle(SocketHandle* hdle, bool close_handle)
{
    int fd = hdle->get_fd();
	pthread_mutex_lock(&_map_mutex);
    // 需要关闭连接，用于连接错误或者短连接的情况
    if (close_handle) {
        if(hdle->release() == 0)
        {
            del_input_fd(fd);
            ::close(fd);
            delete hdle;
            m_fd_set.erase(fd);
            pthread_mutex_lock(&_connections_mutex);
            _now_connections--;
            pthread_mutex_unlock(&_connections_mutex);
			cerr << "close connection:" << fd <<endl;
        }	
        else 
		{
			cerr << "close connection error:" << fd << "ref:" << hdle->release() << endl;
        }
    }
    else 
	{
        hdle->release();
    }
	pthread_mutex_unlock(&_map_mutex);

	return 0;
}

// 收发服务请求
int Http_Server::recv_request(int fd)
{
	SocketHandle * hdle = get_handle(fd);
	assert(hdle);
	Worker * worker = new Worker;
   	assert(worker);
	timeval timeout = { 0, RECEIVE_TIMEOUT * 1000 };
    worker->handle = hdle;

    int ret = -1;
	cerr << "[INFO] begin to get http header" <<endl; 
    ret = read_http_header_timeout(hdle, worker->header, &timeout);
    if (ret < 0) 
	{
		cerr << "get http header error fd:" << endl;
        release_handle(hdle, true);
        delete worker;
        return -1;
    }
	cerr << "[INFO] read_http_header_timeout is OK!" << endl;
    ret = GetInfo(worker);		// 解析 http 服务的一些参数
	cerr << "url=" << worker->request_url << ", content_len=" << worker->content_len;
    if (ret < 0) 
	{
		cerr << "GetInfo error!";
        release_handle(hdle, true);
        delete worker;
        return -1;
    }
    worker->genContentBuf();	// 设置content 的 buf 空间
	memset(worker->content_buf, '\0', 
			worker->content_len);

    if (readn_timeout(hdle,worker->content_buf, worker->content_len, &timeout) == worker->content_len) // 读取http 接收到的content 内容
	{
		worker->content_buf[worker->content_len] = '\0';
        m_handler->handle_worker(worker);		// 将worker 传给后台具体的处理模块，最终给出结果。
        add_input_fd(fd);
    }
    else 
	{
		cerr << "get http content error!";
        //_ERROR("get http content error");
        release_handle(hdle, true);
        delete worker;
        return -1;
    }
	cerr << "[INFO] content = " << worker->content_buf << endl;
	return 0;
}

int Http_Server::retrieve_worker(Worker * worker)
{
    int ret;
    char response_buf[40960];
    int len;
    worker->response->dump_to_buff(response_buf, 40960, len);	// response_buf 是最终的后台返回结果
    send_result(worker->handle, response_buf, len);			// 将最终结果写入 response_buf 中
    release_handle(worker->handle, !worker->keep_alive);
	delete worker;
	return ret;
}

int Http_Server::send_result(SocketHandle *hdle,const void * buf, int buf_len)
{
	int ret = 0;
	timeval timeout = { 0, REPLY_TIMEOUT * 1000 };

	std::string httpheader;
	hdle->lock();
	if(writen_timeout(hdle->get_fd(), buf, buf_len, &timeout) != (int)buf_len)
		ret = -3;	
	hdle->unlock();	

	return ret;
}

// 注意，这里只做最基本的http头解析，解析出content-length，connection，url，method
// 其他留到HttpHandler自行解析
int Http_Server::GetInfo(Worker* worker) {
    std::string header = worker->header;
	cerr << "[INFO] " << header << endl;
    int ret = 0;
    char* uri = (char*)malloc(header.length());
    unsigned int http_version[2];
    int idx = -1;
    const char* buf;
    if (header.find("POST") != std::string::npos) {
        buf = header.find("POST") + header.c_str(); 
        if (sscanf(buf, "POST /%s HTTP/%u.%u",uri, &(http_version[0]),&(http_version[1])) != 3) 
		{
			cerr << "get post header error!" << endl;
            //_ERROR("get post header error");
            ret = -1;
        }
    }
    else if (header.find("GET") != std::string::npos) {
        buf = header.find("GET") + header.c_str(); 
        if (sscanf(buf, "GET /%s HTTP/%u.%u",uri, &(http_version[0]),&(http_version[1])) != 3) 
		{
			cerr << "get post head error!" << endl;
            //_ERROR("get post header error");
            ret = -1;
        }
    }
    else if (header.find("PUT") != std::string::npos) {
        buf = header.find("PUT") + header.c_str(); 
        if (sscanf(buf, "PUT /%s HTTP/%u.%u",uri, &(http_version[0]),&(http_version[1])) != 3) 
		{
			cerr << "get post head error!" << endl;
            //_ERROR("get post header error");
            ret = -1;
        }
    }
    else if (header.find("DELETE") != std::string::npos) {
        buf = header.find("DELETE") + header.c_str(); 
        if (sscanf(buf, "DELETE /%s HTTP/%u.%u",uri, &(http_version[0]),&(http_version[1])) != 3) 
		{
			cerr << "get post head error!" << endl;
            //_ERROR("get post header error");
            ret = -1;
        }
    }
    else 
	{
		cerr << "unsupported header method!" << endl;
        //_ERROR("unsupported header method");
        ret = -1;
    }
    if (ret == -1) {
        free(uri);
        return ret;
    }
    worker->request_url.assign(uri);
    buf = header.c_str() + header.find("Content-Length:");
    if(sscanf(buf,"Content-Length: %d",&worker->content_len) != 1) 
	{
		cerr << "fail to get content len!" << endl;
        //_ERROR("fail to get content len");
        worker->content_len = 0;
    }
    if (header.find("Connection: close") != std::string::npos) {
        worker->keep_alive = false;
    }
    else if (header.find("Connection: keep-alive") != std::string::npos) {
        worker->keep_alive = true;
    }
    free(uri);
    return ret;
}

int Http_Server::read_http_header_timeout(SocketHandle * hdle, std::string &http, timeval *timeout)
{
	char buf[READ_BUF_SIZE + 1];
	int n,pos;	
	std::string & _buffer = hdle->get_read_buf();
	int fd = hdle->get_fd();
	
	cerr << "[INFO] _buffer in read_http_header_timeout: " << _buffer << endl;
	
	while(1)
	{
		if ((pos = _buffer.find("\r\n\r\n")) >= 0)
		{
			http = _buffer.substr(0, pos);
			_buffer = _buffer.substr(pos + 4);
			return 0;
		}
		if((n = read_data(fd,buf,READ_BUF_SIZE,timeout)) <= 0)
			return -1;
		buf[n] = '\0';
		_buffer += buf;
	}
	cerr << "[INFO] _buffer in read_http_header_timeout: " << _buffer << endl;
	return -1;
}

int Http_Server::readn_timeout(SocketHandle * hdle, char* content, int buf_len, timeval *timeout)
{
	char buf[READ_BUF_SIZE+1];
	int n,len;
	std::string & _buffer = hdle->get_read_buf();				// 读取content 内容
	int left = buf_len - _buffer.length();
	int fd = hdle->get_fd();

	cerr << "[INFO] _buffer in readn_timeout: " << buf << endl;
	if(left <= 0)
	{
        memcpy(content, _buffer.substr(0,buf_len).c_str(), buf_len);	// 将 content内容赋值到 content 变量
        _buffer = _buffer.substr(buf_len);
		return buf_len;
	}
    memcpy(content, _buffer.c_str(), _buffer.length());

    _buffer.clear();

	while (left > 0)
	{
		len = left>READ_BUF_SIZE?READ_BUF_SIZE:left;
		if ((n = read_data(fd, buf, len,timeout)) <= 0)
			return buf_len - left;

        memcpy(content + (buf_len - left), buf, n);
		left -= n;
	}


	return buf_len;
}

int Http_Server::read_data(int fd, void * buf, int buf_len, timeval * timeout)
{
    pollfd read_fd;
    read_fd.fd = fd;
    read_fd.events = POLLIN;
    int poll_ret = poll(&read_fd, 1, timeout->tv_sec * 1000 + timeout->tv_usec / 1000);
    if (poll <= 0 || !(read_fd.revents & POLLIN)) {
        return -1;
    }
	return read(fd, buf, buf_len);
}

int Http_Server::generate_http_header(std::string& res,int len)
{
	char buf[HTTP_HEADER_MAX_LENGTH];

	//generate header
	int ret = snprintf(buf, HTTP_HEADER_MAX_LENGTH, "HTTP/1.1 %d %s\r\n",200,"OK");
	res = buf;

	// 生成响应报文时间戳
	time_t t;
	struct tm tmp_time;
	time(&t);
	char timestr[64];
	strftime(timestr, sizeof(timestr), "%a, %e %b %Y %H:%M:%S %Z", gmtime_r(&t, &tmp_time));
	ret = snprintf(buf,HTTP_HEADER_MAX_LENGTH,"Date:%s\r\n",timestr);
	res += buf;

	//add content length
	ret = snprintf(buf,HTTP_HEADER_MAX_LENGTH,"Content-Length:%d\r\n",len);
	res += buf;

	//add content type
	ret = snprintf(buf,HTTP_HEADER_MAX_LENGTH,"Content-Type: %s; charset=%s\r\n","text/xml","utf-16");

	snprintf(buf,HTTP_HEADER_MAX_LENGTH,"\r\n");
	res += buf;

	return res.length();
}

int Http_Server::writen_timeout(int fd, const void *buf, int buf_len, timeval *timeout)
{
	int left = buf_len;
	int n;
    pollfd write_fd;
    write_fd.fd = fd;
    write_fd.events = POLLOUT;
    int poll_ret;

	while (left > 0)
	{
        poll_ret = poll(&write_fd, 1, timeout->tv_sec * 1000 + timeout->tv_usec / 1000);
        if (poll_ret <= 0 || !(write_fd.revents & POLLOUT)) {
            return -1;
        }
		if ((n = write(fd, buf, left)) == 0)
			return buf_len - left;

		buf = (char *)buf + n;
		left -= n;
	}

	return buf_len;
}

int Http_Server::register_handler(HttpHandler* handler) {
    if (handler == NULL) {
        return -1;
    }
    m_handler = handler;
    m_handler->setServer(this);
    return 0;
}
