#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

//#define READ_BUF_SIZE 512
#define READ_BUF_SIZE 64
#define NOT_ENOUGH_BUFF -3
#define INVALID_HEADER -2
#define READ_TIMEOUT -1
#define ERROR_CHUNK_DATA -4;

typedef enum {
    WORKING,
    FAIL,
} client_state;

class HttpClient {
    private:
        sockaddr_in server_address;
        client_state state;
        int connect_timeout;
        int recv_timeout;
        int send_timeout;
        int _fd;


    protected:
        int read_http_header_timeout(std::string& header, char* body_buf, int& header_len ,int& part_body_len);


    public:
        HttpClient(int _connect_timeout, int _recv_timeout, int _send_timeout);
        ~HttpClient();
        int connect_server(std::string host, int port); 
        virtual int recv(std::string& header, char* body_buf, unsigned int max_buf_len, unsigned int& body_len);
        virtual int send(const char* buf, unsigned int buf_len);
        int disconnect();

    protected:
        int read_data(char* buf, int max_buf_len);
        int readn_timeout(char* buf, int need_to_read);
        int writen_timeout(const char *buf, int need_to_write);

};

#endif

