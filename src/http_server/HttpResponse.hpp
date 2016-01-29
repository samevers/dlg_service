#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP
#include <string.h>

namespace httpserver {

static const char* line_sep = "\r\n";
static const int line_sep_len = strlen(line_sep);

static const int http_response_field_count = 33;

typedef enum {
    HTTP10,
    HTTP11,
} http_res_version;

static const char* http_res_version_str[2] = {
    "HTTP/1.0",
    "HTTP/1.1"
};

typedef enum {
    Accept_Ranges,
    Age,
    Allow,
    Cache_Control,
    Connection,
    Content_Encoding,
    Content_Language,
    Content_Length,
    Content_Location,
    Content_MD5,
    Content_Disposition,
    Content_Range,
    Content_Type,
    Date,
    ETag,
    Expires,
    Last_Modified,
    Link,
    Location,
    P3P,
    Pragma,
    Proxy_Authenticate,
    Refresh,
    Retry_After,
    Server,
    Set_Cookie,
    Strict_Transport_Security,
    Trailer,
    ransfer_Encoding,
    Vary,
    Via,
    Warning,
    WWW_Authenticate,
} http_response_field;

static const char* http_response_field_names[http_response_field_count] = {
    "Accept-Ranges: ",
    "Age: ",
    "Allow: ",
    "Cache-Control: ",
    "Connection: ",
    "Content-Encoding: ",
    "Content-Language: ",
    "Content-Length: ",
    "Content-Location: ",
    "Content-MD5: ",
    "Content-Disposition: ",
    "Content-Range: ",
    "Content-Type: ",
    "Date: ",
    "ETag: ",
    "Expires: ",
    "Last-Modified: ",
    "Link: ",
    "Location: ",
    "P3P: ",
    "Pragma: ",
    "Proxy-Authenticate: ",
    "Refresh: ",
    "Retry-After: ",
    "Server: ",
    "Set-Cookie: ",
    "Strict-Transport-Security: ",
    "Trailer: ",
    "Transfer-Encoding: ",
    "Vary: ",
    "Via: ",
    "Warning: ",
    "WWW-Authenticate: "
};

typedef enum {
    SC100,
    SC101,
    SC200,
    SC201,
    SC202,
    SC203,
    SC204,
    SC205,
    SC206,
    SC300,
    SC301,
    SC302,
    SC303,
    SC304,
    SC305,
    SC307,
    SC400,
    SC401,
    SC402,
    SC403,
    SC404,
    SC405,
    SC406,
    SC407,
    SC408,
    SC409,
    SC410,
    SC411,
    SC412,
    SC413,
    SC414,
    SC415,
    SC416,
    SC417,
    SC500,
    SC501,
    SC502,
    SC503,
    SC504,
    SC505,
} status_code;

static const char* status_code_str[40] = {
    "100 Continue",
    "101 Switching Protocols",
    "200 OK",
    "201 Created",
    "202 Accepted",
    "203 Non-Authoritative Information",
    "204 No Content",
    "205 Reset Content",
    "206 Partial Content",
    "300 Multiple Choices",
    "301 Moved Permanently",
    "302 Found",
    "303 See Other",
    "304 Not Modified",
    "305 Use Proxy",
    "307 Temporary Redirect",
    "400 Bad Request",
    "401 Unauthorized",
    "402 Payment Required",
    "403 Forbidden",
    "404 Not Found",
    "405 Method Not Allowed",
    "406 Not Acceptable",
    "407 Proxy Authentication Required",
    "408 Request Time-out",
    "409 Conflict",
    "410 Gone",
    "411 Length Required",
    "412 Precondition Failed",
    "413 Request Entity Too Large",
    "414 Request-URI Too Large",
    "415 Unsupported Media Type",
    "416 Requested range not satisfiable",
    "417 Expectation Failed",
    "500 Internal Server Error",
    "501 Not Implemented",
    "502 Bad Gateway",
    "503 Service Unavailable",
    "504 Gateway Time-out",
    "505 HTTP Version not supported"
};

class HttpResponse {
    private:
        char* content_buff;
        char* field_vals[http_response_field_count];
        status_code code;
        http_res_version version;

    public:
        HttpResponse(http_res_version _version, status_code _code);
        ~HttpResponse();
        int setVersion(http_res_version _version);
        int setStatusCode(status_code _code);
        int setField(http_response_field field, const char* field_value);
        int fillContent(unsigned int content_len, const char* content);
        int dump_to_buff(char* dest, int max_dest_len, int& len);
};

} // namespace httpserver
#endif
