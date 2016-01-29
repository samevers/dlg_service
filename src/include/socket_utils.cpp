#include "socket_utils.h"
#include "stdio.h"
#include <sys/types.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <sys/wait.h> 
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include "string_Util.h"
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <linux/if.h>


void SleepMsec ( int iMsec )
{
	if ( iMsec<=0 )
		return;

#if USE_WINDOWS
	Sleep ( iMsec );
#else
	struct timeval tvTimeout;
	tvTimeout.tv_sec = iMsec / 1000; // full seconds
	tvTimeout.tv_usec = ( iMsec % 1000 ) * 1000; // remainder is msec, so *1000 for usec
	select ( 0, NULL, NULL, NULL, &tvTimeout ); // FIXME? could handle EINTR
#endif
}
int gettimeLen_us(timeval &_end,timeval &_beg){
    return ((_end.tv_sec-_beg.tv_sec)*1000000+(_end.tv_usec-_beg.tv_usec));
}

my_sso_string GetLocalIp()
{
  struct ifreq req;
  int sock;
  char *dn_or_ip;
  char current_ip[1024];

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  strncpy(req.ifr_name, "eth0", IFNAMSIZ);

  if ( ioctl(sock, SIOCGIFADDR, &req) < 0 ) {
    fprintf(stderr,"ConnectionServer::Open ioctl error: %s Cann't get current IP\n", strerror (errno));
    sprintf(current_ip,"Get currentip error %d",errno);
  }
  else
  {
    dn_or_ip = (char *)inet_ntoa(*(struct in_addr *) &((struct sockaddr_in *) &req.ifr_addr)->sin_addr);
    strcpy(current_ip,dn_or_ip);
  }
  //fprintf(stderr,"ConnectionServer::Open Current IP:%s\n", current_ip);
  shutdown(sock, 2);
  ::close(sock);

  return current_ip;
}
//创建非阻塞的管道
int create_pipe(int &_pipe_read,int &_pipe_write){
	int options;
	int pipe_fd[2];
	if(pipe(pipe_fd))
		return -1;
	_pipe_read=pipe_fd[0];
	_pipe_write=pipe_fd[1];
	
	for(int i=0;i<2;++i){
		if((options=fcntl(pipe_fd[i],F_GETFL)) == -1 )
			return -1;
		if(fcntl(pipe_fd[i],F_SETFL,options|O_NONBLOCK) == -1)
			return -1;
	}
	return 0;
}


int get_fd_info(int fd, char* ipaddr,int strLen)
{
    struct sockaddr_in ser_name;
	struct sockaddr_in cli_name;

    size_t namelen = sizeof(ser_name);
	struct hostent* result;
	
	if( (fd<=0) || (ipaddr==NULL) ||(strLen<=0))
		return -1;
	 

    if (getpeername(fd, (struct sockaddr *)&ser_name, (socklen_t*)&namelen) != 0) 
	{
         return -1;
    }
	if (getsockname(fd,(struct sockaddr *)&cli_name, (socklen_t*)&namelen) !=0)
	{
		return -1;
	}

	unsigned int ip    = ser_name.sin_addr.s_addr;
	unsigned int cli_ip= cli_name.sin_addr.s_addr;

	snprintf(ipaddr,strLen,"(%d.%d.%d.%d:%hu-->%d.%d.%d.%d:%hu)",ip&0xFF,ip>>8&0xFF,ip>>16&0xFF,ip>>24&0xFF,ntohs(ser_name.sin_port),
													cli_ip&0xFF,cli_ip>>8&0xFF,cli_ip>>16&0xFF,cli_ip>>24&0xFF,ntohs(cli_name.sin_port));	
	return 0;
}

int asyn_connect_timeout(struct sockaddr_in *_addr,timeval *_conn_timeout)
{
	int socket_fd;
	if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) <= 0)
		return -1;
	fprintf(stderr,"[asyn_connect]asyn connecting fd=%d ..\n",socket_fd);
	//设置为非阻塞的
	if (set_socket(socket_fd, O_NONBLOCK))
	{
		close(socket_fd);
		return -1;
	}
/*	
	        #if 1
        int keeplive=1;
        setsockopt(socket_fd,SOL_SOCKET,SO_KEEPALIVE,(void*)&keeplive,sizeof(keeplive));
    
        int keepidle=3;//首次开始探测的空闲时间
        int keepinteval=3;//两次探测的间隔时间
        int keepcount=3;//探测包个数
        setsockopt(socket_fd,SOL_TCP,TCP_KEEPIDLE,(void*)&keepidle,sizeof(keepidle));    
        setsockopt(socket_fd,SOL_TCP,TCP_KEEPINTVL,(void*)&keepinteval,sizeof(keepinteval));
        setsockopt(socket_fd,SOL_TCP,TCP_KEEPCNT,(void*)&keepcount,sizeof(keepcount));    

        #endif
*/
	
	int _ret=connect(socket_fd, (struct sockaddr *)_addr, sizeof(sockaddr_in));
	//异步连接，连接成功
	if (_ret == 0) 
	{
		char ipinfo[64];
    	if(get_fd_info(socket_fd,ipinfo,64))
    	{   
        	fprintf(stderr,"[asyn_connect]fd=%d,get_fd_info ERROR,errno=%d\n",socket_fd,errno);
        	::close(socket_fd);
        	return -1; 
    	}   
		
		fprintf(stderr,"[asyn_connect]fd=%d connected,fdInfo:%s,ret=%d\n",socket_fd,ipinfo,_ret);
		return socket_fd;//如果重连成功，那么直接返回
	}
	//如果连接失败
	if(errno != EINPROGRESS) 
	{
		perror("[asyn_connect]connect server error:");
		unsigned int ip = _addr->sin_addr.s_addr;
		fprintf(stderr,"[asyn_connect]connect server %d.%d.%d.%d:%hu ERROR\n",ip & 0xFF, ip >> 8 & 0xFF,
											ip >> 16 & 0xFF, ip >> 24 & 0xFF,ntohs(_addr->sin_port));
		close(socket_fd);
		return -1;
	}
		
	//如果正在连接中，那么select等待一段时间
	fd_set _wset,_rset;
	FD_ZERO(&_rset);
	FD_SET(socket_fd,&_rset);
	_wset=_rset;
    int retval = select(socket_fd + 1, &_rset, &_wset, NULL, _conn_timeout);
    if (retval == -1) 
	{
		close(socket_fd);
        fprintf(stderr, "[asyn_connect]select fd=%d ERROR\n",socket_fd);
		return -1;
	} 
	else if(retval == 0) 
	{
		close(socket_fd);
		fprintf(stderr, "[asyn_connect]select fd=%d timeout ERROR\n",socket_fd);
        return -1;
    }
	if( (FD_ISSET(socket_fd,&_wset))|| (FD_ISSET(socket_fd,&_rset)) )
	{
		int error,_ret;
		socklen_t opt_len=sizeof(int);
		_ret=getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, (char*)(&error),&opt_len);
		if(_ret < 0) 
		{
			fprintf(stderr, "[asyn_connect]getsockopt ERROR\n");
			return -1;
		}
		if(error) 
		{
			close(socket_fd);
			fprintf(stderr, "[asyn_connect]connect fd=%d ERROR ,%d\n",socket_fd,error);
			return -1;
		}
		char ipinfo[64];
    	if(get_fd_info(socket_fd,ipinfo,64))
    	{   
        	fprintf(stderr,"[asyn_connect]fd=%d,get_fd_info ERROR,errno=%d\n",socket_fd,errno);
        	::close(socket_fd);
        	return -1; 
    	}   
		fprintf(stderr,"[asyn_connect]connect sucess fd=%d,fdInfo:%s\n",socket_fd,ipinfo);
		return socket_fd;
	}
	fprintf(stderr,"[asyn_connect]connect fd=%d ERROR,errno=%d,retval=%d\n",socket_fd,errno,retval);
	close(socket_fd);
	return -1;
}

int set_socket(int fd, int flag)
{
	int options;
	
	//设置socketFd的默认项
	options = SOCKET_SND_BUF_SIZE;
	setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &options, sizeof(int));
	options = SOCKET_RCV_BUF_SIZE;
	setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &options, sizeof(int));
	options = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(int));
	
	//得到fd的选项参数(所有的参数，然再添加)
	options = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, options | flag);
	//return 0;
	int on = 1;
	int ret = -1; 
	ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on));
	return ret;
}


int writen_timeout(int fd, const void *buf, size_t buf_len, timeval *timeout)
{
	int left = buf_len;
	int n;
	fd_set wset;
	while (left > 0)
	{
		FD_ZERO(&wset);
		FD_SET(fd, &wset);
		if ((n=select(fd + 1, NULL, &wset, NULL, timeout)) <= 0){
			fprintf(stderr,"[writen_timeout]fd=%d select,ret=%d errno=%d\n",fd,n,errno);
			return -1;
		}
		if ((n = write(fd, buf, left)) == 0)
		{
			fprintf(stderr,"[writen_timeout]fd=%d ERROR,errno=%d\n",fd,errno);
			return buf_len - left;
		}
		if(n==-1){
			fprintf(stderr,"[writen_timeout]fd=%d,ret=-1,errno=%d\n",fd,errno);
			return -1;
		}
		buf = (char *)buf + n;
		left -= n;
	}
	fprintf(stderr,"[writen_timeout]fd=%d write SUCESS\n",fd);
	return buf_len;
}

int read_once_timeout(int _fd, void *buf, size_t buf_len, timeval *timeout)
{
	if (_fd == INVALID_FD){
		fprintf(stderr,"[read_once_timeout] fd is INVALID\n");
		return  -1;
	}
	fd_set rset;
	int n;
	FD_ZERO(&rset);
	FD_SET(_fd, &rset);
	if ( (n=select(_fd + 1, &rset, NULL, NULL, timeout)) <= 0){
		fprintf(stderr,"[read_once_timeout] fd=%d select ret=%d,errno=%d\n",_fd,n,errno);
		return -1;
	}
	if ((n = read(_fd, buf, buf_len)) <= 0){
		fprintf(stderr,"[read_once_timeout] fd=%d read ERROR,ret=%d,errno=%d\n",_fd,n,errno);
		return -1;
	}
	//my_sso_string _test;
    //_test.assign((char*)buf,n);
    //fprintf(stderr,"[read_once_timeout]fd=%d read SUCESS,%s,count=%d\n",_fd,_test.c_str(),n);

	return n;
}

int readn_timeout(int fd, void *buf, size_t buf_len, timeval *timeout)
{
	int left = buf_len;
	int n;
	fd_set rset;
	while (left > 0)
	{
		FD_ZERO(&rset);
		FD_SET(fd, &rset);
		int ret;
		if ( (ret=select(fd + 1, &rset, NULL, NULL, timeout)) <= 0){
			fprintf(stderr,"[readn_timeout]select ret=%d fd=%d,sec=%d,usec=%d",ret,fd,timeout->tv_sec,timeout->tv_usec);
			return -1;
		}
		if ((n = read(fd, buf, left)) == 0){
			fprintf(stderr,"[readn_timeout]read sucess END\n");
			return buf_len - left;
		}
		if(n==-1)
		{
			fprintf(stderr,"[readn_timeout]errno=%d\n",errno);
			return -1;
		}
		buf = (char *)buf + n;
		left -= n;
	}

	return buf_len;
}

//返回0表示获取成功，retEnd=-1表示到_url的最后
int getHostofUrl(const my_sso_string &_url,int &retBeg,int &retEnd)
{
	if(_url.empty())
		return -1; 
	int pos1=_url.find("http://");
	int pos1_1=_url.find("https://");
	if( (pos1==my_sso_string::npos) &&(pos1_1==my_sso_string::npos)) 
	{   
		retBeg=0;
	}
	else if(pos1_1==my_sso_string::npos)
		retBeg=pos1+7;
	else
		retBeg=pos1+8;
	int pos2=_url.find("/",retBeg);
	int pos3=_url.find(":",retBeg);
	if ((pos3!=my_sso_string::npos)&&((pos2==my_sso_string::npos)||(pos3<pos2)))
	{
		pos2 = pos3;
	}   
	
	if(pos2==my_sso_string::npos) 
		retEnd=-1;
	else
		retEnd=pos2;
	return 0;
}

int get_host_port(const my_sso_string &strAddr,my_sso_string &_host,int &_port)
{
	int _beg,_end;
	if(getHostofUrl(strAddr,_beg,_end))
	{   
		fprintf(stderr,"[ERROR]get hostfrom %s error\n",strAddr.c_str());
		return -2; 
	}
	//得到host信息
	if(_end<=_beg)
		_host=strAddr.substr(_beg);
	else
		_host=strAddr.substr(_beg,_end-_beg);

	//得到port信息
	_port=-1;//初始化port为-1
	if( (_end!=-1)&&(strAddr[_end]==':') )//有host,有port
	{
		_port=atoi(strAddr.c_str()+_end+1);
	}
	if(_port<=0) 
	{
		if( strAddr.find("http://")!=my_sso_string::npos)
			_port=80;
		else if(strAddr.find("https://")!=my_sso_string::npos )
			_port=443;
		else
		{   
			fprintf(stderr,"[ERROR]get port from %s error\n",strAddr.c_str());
			return -1; 
		}   
	}   
	fprintf(stderr,"[INFO]get host=%s,port=%d from %s success!\n",_host.c_str(),_port,strAddr.c_str());
	return 0;
}

int	gethostAddr(const my_sso_string hostInfo,struct sockaddr_in &sockAddr)
{
	my_sso_string _ip;
	int 		_port;
	if(get_host_port(hostInfo,_ip,_port))
		return -1;
	return gethostAddr(_ip,_port,sockAddr);	
}
int	gethostAddr(const my_sso_string host,const int port, struct sockaddr_in &sockAddr)
{
	my_sso_string _ip = hostnameToIp(host);
	char *pos;
	addrinfo *res;
	static addrinfo hints = { 0, PF_INET, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL };

	char port_str[256]={0};
	snprintf(port_str,256,"%d",port);
		
	fprintf(stderr,"[gethostAddr]ip:%s,port:%s\n",_ip.c_str(),port_str);

	if (getaddrinfo(_ip.c_str(),port_str, &hints, &res)){
		fprintf(stderr,"[gethostAddr] error,ip=%s,port=%s,errno=%d\n",_ip.c_str(),port_str,errno);
		return -1;
	}
	sockAddr=*(sockaddr_in *)res->ai_addr;
	freeaddrinfo(res);
	return 0;
}

my_sso_string hostnameToIp(my_sso_string name)
{
    char   *ptr, **pptr;
    struct hostent *hptr;
    char   str[32];
    my_sso_string res;

    //my_sso_string ip="pic.sogou.com";
    if((hptr = gethostbyname(name.c_str())) == NULL)
    {
        //printf(" gethostbyname error for host:%s\n", name.c_str());
        return "";
    }

    switch(hptr->h_addrtype)
    {
    case AF_INET:
    case AF_INET6:
        pptr=hptr->h_addr_list;
        /*for(; *pptr!=NULL; pptr++)
            printf(" address:%s\n",
                   inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
        
        printf(" first address: %s\n",
        inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str)));*/
        inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str));        
        res.assign(str,strlen(str));
        
        break;
    
    default:
        //printf("unknown address type\n");
        break;
    }
    return res;
}

int	create_server_socket(int &socket_fd,int port,int listenNum)
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
	if(MAX_FD_NUM<listenNum)
		listenNum=MAX_FD_NUM;

	if (listen(socket_fd, listenNum))
		return -1; 
	return 0;
}

bool sockadd_in_eq(sockaddr_in &sock_a,sockaddr_in &sock_b){
    if(memcmp(&sock_a,&sock_b,sizeof(sockaddr_in))==0)
        return true;
    return false;
}


