#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <string.h>

typedef enum {
    GET,
    POST,
    PUT,
    DELETE,
} http_method;

static const char* http_method_name[4] = {
    "GET",
    "POST",
    "PUT",
    "DELETE"
};

typedef enum {
    HTTP10,
    HTTP11,
} http_version;

static const char* http_version_str[2] = {
    "HTTP/1.0", 
    "HTTP/1.1"
};

static const char* line_sep = "\r\n";
static const int line_sep_len = strlen(line_sep);

typedef enum {
    Accept,
    Accept_Charset,
    Accept_Encoding,
    Accept_Language,
    Accept_Datetime,
    Authorization,
    Cache_Control,
    Connection,
    Cookie,
    Content_Length,
    Content_MD5,
    Content_Type,
    Date,
    Expect,
    From,
    Host,
    If_Match,
    If_Modified_Since,
    If_None_Match,
    If_Range,
    If_Unmodified_Since,
    Max_Forwards,
    Pragma,
    Proxy_Authorization,
    Range,
    Referer,
    TE,
    Upgrade,
    User_Agent,
    Via,
    Warning,
} http_header_field;

static const int http_header_field_count = 31;

static const char* http_header_field_names[http_header_field_count] = {
    "Accept: ",
    "Accept-Charset: ",
    "Accept-Encoding: ",
    "Accept-Language: ",
    "Accept-Datetime: ",
    "Authorization: ",
    "Cache-Control: ",
    "Connection: ",
    "Cookie: ",
    "Content-Length: ",
    "Content-MD5: ",
    "Content-Type: ",
    "Date: ",
    "Expect: ",
    "From: ",
    "Host: ",
    "If-Match: ",
    "If-Modified-Since: ",
    "If-None-Match: ",
    "If-Range: ",
    "If-Unmodified-Since: ",
    "Max-Forwards: ",
    "Pragma: ",
    "Proxy-Authorization: ",
    "Range: ",
    "Referer: ",
    "TE: ",
    "Upgrade: ",
    "User-Agent: ",
    "Via: ",
    "Warning: "
}; 

class HttpRequest {
    private:
        http_method method;
        http_version version;
        std::string url;
        char* content_buff;
        char* field_vals[http_header_field_count];

    public:
        HttpRequest(http_version _version, http_method _method, const char* _url);
        ~HttpRequest();
        int setField(http_header_field filed, const char* field_value);
        int fillContent(unsigned int content_len, const char* content);
        int dump_to_buff(char* dest, int max_dest_len, int& len);
};

#endif
