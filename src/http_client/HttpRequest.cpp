#include "HttpRequest.hpp"
#include <string.h>
#include <stdlib.h>
//#include "gtest/gtest.h"
//#include "gflags/gflags.h"
//#include "glog/logging.h"
#include <iostream>

using namespace std;
//#include <service_log.hpp>

HttpRequest::HttpRequest(http_version _version, http_method _method, const char* _url) {
    version = _version;
    method = _method;
    url = std::string(_url);
    content_buff = NULL;
    for (int i = 0; i < http_header_field_count; i++) {
        field_vals[i] = NULL;
    }
}

HttpRequest::~HttpRequest() {
    if (content_buff != NULL) {
        free(content_buff);
    }
    for (int i = 0; i < http_header_field_count; i++) {
        if (field_vals[i] != NULL) {
            free(field_vals[i]);
        }
    }
}

int HttpRequest::setField(http_header_field field, const char* field_value) {
    if (field == Content_Length) {
		//LOG(ERROR) << "can't set content length";
        //_ERROR("can not set Content-Length");
        return -1;
    }
    if (field_vals[field] != NULL) {
        free(field_vals[field]);
    }
    int val_len = strlen(field_value);
    field_vals[field] = (char*)malloc(val_len + 1);
    if (field_vals[field] == NULL) {
        return -1;
    }
    memcpy(field_vals[field], field_value, val_len);
    field_vals[field][val_len] = '\0';
    return 0;
}

int HttpRequest::dump_to_buff(char* dest, int max_dest_len, int& len) {
    int head_print_len = 0;
    int body_len = 0;
    int write_count = 0;
    int to_write_count = -1;
    // µÚÒ»ÐÐ method + space + url + space + version + \r\n
    to_write_count = strlen(http_method_name[method]) + 1 + url.length() + 1 + strlen(http_version_str[version]) + strlen(line_sep);
    write_count = snprintf(dest, max_dest_len - head_print_len, "%s %s %s%s", 
            http_method_name[method],
            url.c_str(),
            http_version_str[version],
            line_sep);
    if (write_count != to_write_count) {
        return -1;
    }
    head_print_len += write_count;
    for (int i = 0; i < http_header_field_count; i++) {
        if (field_vals[i] != NULL) {
            to_write_count = strlen(http_header_field_names[i]) + strlen(field_vals[i]) + strlen(line_sep);
            write_count = snprintf(dest + head_print_len, max_dest_len - head_print_len, "%s%s%s",
                    http_header_field_names[i],
                    field_vals[i],
                    line_sep);
            if (to_write_count != write_count) {
                return -1;
            }
            head_print_len += write_count;
        }
    }
    write_count = snprintf(dest + head_print_len, max_dest_len - head_print_len, "%s", line_sep);
    if (write_count != strlen(line_sep)) {
        return -1;
    }
    head_print_len += write_count;
    len = head_print_len;
    if (field_vals[Content_Length] != NULL) {
        body_len += (int)atoi(field_vals[Content_Length]);
        if (head_print_len + body_len > max_dest_len) {
            return -1;
        }
        memcpy(dest + head_print_len, content_buff, body_len);
        len = len + body_len;
        dest[len] = '\0';
    }
    return 0;
}

int HttpRequest::fillContent(unsigned int content_len, const char* content) {
    if (field_vals[Content_Length] != NULL) {
        free(field_vals[Content_Length]);
    }
    if (content_buff != NULL) {
        free(content_buff);
    }
    field_vals[Content_Length] = (char*)malloc(100);
    snprintf(field_vals[Content_Length], 100, "%u", content_len);
    content_buff = (char*)malloc(content_len + 10);
    memcpy(content_buff, content, content_len);
    return 0;
}
