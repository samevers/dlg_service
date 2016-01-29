#include "HttpResponse.hpp"
#include <stdlib.h>
#include <string.h>

http_response::http_response() {
    ret_code = -1;
    content_len = (unsigned int)-1;
    content = NULL;
}

http_response::~http_response() {
    free(content);
}


int http_response::get_memory(unsigned int len) {
    content_len = len;
    content = (char*)malloc(len);
    return 0;
}

bool http_response::check_valid() {
    return (content_len != (unsigned int)-1 && content != NULL);
}

