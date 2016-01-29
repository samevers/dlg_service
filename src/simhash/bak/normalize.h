#ifndef __NORMALIZE_H__
#define __NORMALIZE_H__

#include <errno.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
using namespace std;

// ����ͣ�ôʵ�
int load_stopwd();

// ��ʼ��Ԥ����
int normalize_init();

// �ͷż�����Դ
void normalize_release();

// ȥ��һ���ı��Ļ��кͻس�
char * ul_ccode_chomp_r_n(const char * strIn, char * strOut);

// �ж��Ƿ�Ϊ �����ַ�
// samhu.
// OK.
int isSpecialCharacter(string str);

// �ж��Ƿ�Ϊ����
// samhu.
// OK.
int is_digit(const char* str);

// ȥ�������ַ�,���³��������ַ���
// samhu.
// OK.
void deleteSpecialCharacter(string& line);

// ��ȡ�ַ����������ַ����������֣�ascii��ĸ������
// samhu.
// OK.
//string ul_ccode_valid_word(const char * strIn, int szStr);
string ul_ccode_valid_word(const char * strIn, int szStr);

// ��ȡhtml ��ҳ�е����ݺ�url
int html_content2(string& line, int size);

// ��ȡ��url ����ı���urlΪ��ҳtoken�е�url��
string doUrl(string& str, vector<string> url_vec);

// ȥ��ͣ�ôʡ�����
void replace_stopwd(string& str);

int ul_ccode_is_cn_en_japanese(const char* str);

string doUrl(string line, string url_img,string url_img_tail);

// ����Ǳ���滻�ɿո�
string replace_punc(string& line);
string replace_punc2(const char* strIn);

// ʶ�� url(demo)
// ��http:,https:��www.�Լ�ftp: Ϊ��־������url���ҵ����ֹ�������"����<������
// samhu.
void url_punc(string& line);

/* ����ת����
*	param: string&, ���룬�������á�
*	return��0���ɹ���-1��ʧ�ܡ�
*/
int TRAM2SIM(string& line_tmp,int size,char* transfer);

// ���ı�����Ԥ����
// return destbuff 
// samhu.
// OK.
int  text_normalize(const char * oritext, char * destbuff, int bufsize);

// ks
int html_entity2char(const char *src,char *dest,int codetype);


#endif
