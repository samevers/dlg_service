#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#define EXTRA 20

class http_response {
    public:
        short ret_code;
        unsigned int content_len;
        char* content;

    public:
        http_response();
        ~http_response();
        int get_memory(unsigned int len);
        bool check_valid(); 
};

#endif
