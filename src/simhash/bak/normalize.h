#ifndef __NORMALIZE_H__
#define __NORMALIZE_H__

#include <errno.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
using namespace std;

// 加载停用词典
int load_stopwd();

// 初始化预加载
int normalize_init();

// 释放加载资源
void normalize_release();

// 去掉一行文本的换行和回车
char * ul_ccode_chomp_r_n(const char * strIn, char * strOut);

// 判断是否为 特殊字符
// samhu.
// OK.
int isSpecialCharacter(string str);

// 判断是否为数字
// samhu.
// OK.
int is_digit(const char* str);

// 去掉特殊字符,留下常规有用字符。
// samhu.
// OK.
void deleteSpecialCharacter(string& line);

// 获取字符串中有用字符，包括汉字，ascii字母和数字
// samhu.
// OK.
//string ul_ccode_valid_word(const char * strIn, int szStr);
string ul_ccode_valid_word(const char * strIn, int szStr);

// 抽取html 网页中的内容和url
int html_content2(string& line, int size);

// 抽取除url 外的文本（url为网页token中的url）
string doUrl(string& str, vector<string> url_vec);

// 去掉停用词、符号
void replace_stopwd(string& str);

int ul_ccode_is_cn_en_japanese(const char* str);

string doUrl(string line, string url_img,string url_img_tail);

// 将半角标点替换成空格
string replace_punc(string& line);
string replace_punc2(const char* strIn);

// 识别 url(demo)
// 以http:,https:和www.以及ftp: 为标志来查找url，找到后截止到最近的"或者<结束。
// samhu.
void url_punc(string& line);

/* 繁体转简体
*	param: string&, 输入，设置引用。
*	return：0，成功；-1，失败。
*/
int TRAM2SIM(string& line_tmp,int size,char* transfer);

// 对文本进行预处理
// return destbuff 
// samhu.
// OK.
int  text_normalize(const char * oritext, char * destbuff, int bufsize);

// ks
int html_entity2char(const char *src,char *dest,int codetype);


#endif
