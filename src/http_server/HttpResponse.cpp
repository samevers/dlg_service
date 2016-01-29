#include "HttpResponse.hpp"
#include "stdio.h"
#include "stdlib.h"
#include <iostream>
//#include "gtest/gtest.h"
//#include "gflags/gflags.h"
//#include "glog/logging.h"
//#include <service_log.hpp>

namespace httpserver {

HttpResponse::HttpResponse(http_res_version _version, status_code _code) {
    code = _code;
    version = _version;
    content_buff = NULL;
    for (int i = 0; i < http_response_field_count; i++) {
        field_vals[i] = NULL;
    }
};

HttpResponse::~HttpResponse() {
    if (content_buff != NULL) {
        free(content_buff);
    }
    for (int i = 0; i < http_response_field_count; i++) {
        if (field_vals[i] != NULL) {
            free(field_vals[i]);
        }
    }
}

int HttpResponse::setField(http_response_field field, const char* field_value) {
    if (field == Content_Length) {
        //_ERROR("can not set Content_Length");
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

int HttpResponse::fillContent(unsigned int content_len, const char* content) {
    if (field_vals[Content_Length] != NULL) {
        free(field_vals[Content_Length]);
    }
    if (content_buff != NULL) {
        free(content_buff);
    }
    field_vals[Content_Length] = (char*)malloc(100);
    snprintf(field_vals[Content_Length], 100, "%u", content_len);
    content_buff = (char*)malloc(content_len + 10);
    memcpy(content_buff, content, content_len);					// 系统最终的处理结果返回给content_buff，然后返回给用户；
    return 0;
}

int HttpResponse::dump_to_buff(char* dest, int max_dest_len, int& len) {
    int head_print_len = 0;
    int body_len = 0;
    int write_count = 0;
    int to_write_count = -1;
    // 第一行：版本 + 空格 + 状态码串 + \r\n
    to_write_count = strlen(http_res_version_str[version]) + 1 + strlen(status_code_str[code]) + strlen(line_sep);
    write_count = snprintf(dest + head_print_len, max_dest_len - head_print_len, "%s %s%s",
            http_res_version_str[version],
            status_code_str[code],
            line_sep);
    if (write_count != to_write_count) 
	{
		std::cerr << "write cout error!" << std::endl; 
		std::cerr << "write cout:" << write_count << std::endl;
		std::cerr << "to write cout:" << to_write_count << std::endl;
        return -1;
    }
    head_print_len += write_count;

    for (int i = 0; i < http_response_field_count; i++) {
        if (field_vals[i] != NULL) {
            to_write_count = strlen(http_response_field_names[i]) + strlen(field_vals[i]) + strlen(line_sep);
            write_count = snprintf(dest + head_print_len, max_dest_len - head_print_len, "%s%s%s",
                    http_response_field_names[i],
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

		// add by chaoli
		memcpy(dest + head_print_len, content_buff, body_len);
        len = len + body_len;
        dest[len] = '\0';
    }
    return 0;
}

int HttpResponse::setVersion(http_res_version _version) {
    version = _version;
}

int HttpResponse::setStatusCode(status_code _code) {
    code = _code;
}

} // namespace httpserver
