#include "string_Util.h"
#include <errno.h>
#include <cctype>
#include <sstream>
using namespace std;


static int s_iconv(const my_sso_string &src, my_sso_string &result,my_sso_string in,my_sso_string out)
{
	int max_len = src.length()*3+1;
	iconv_t cd = iconv_open(out.c_str(),in.c_str());
	if (cd<0){
		return -1;	
	}

	char *buf = new char[max_len];
	if (buf == NULL)
		return -1;

	char *inbuff = const_cast<char *>(src.c_str());

	size_t inbytesleft = src.length();
	char *outbuff = buf;
	size_t outbytesleft = max_len;
	size_t ret = iconv(cd, &inbuff, &inbytesleft, &outbuff, &outbytesleft);

	if(ret == size_t(-1))
	{
		fprintf(stderr,"iconv failed: %s\n", strerror(errno));
	}else {
		size_t content_len = max_len - outbytesleft;
		result.assign(buf,content_len);
	}
	iconv_close(cd);
	delete []buf;
	return ret;
}


int gbk2utf8(char * in, int * inlen,char * out, int * outlen)
 {
          char * outbuf = out;
          char * inbuf = in;
          size_t outlenbuf = *outlen;
          size_t inlenbuf = *inlen;
          //iconv_t m_gbk2utf16_ = iconv_open("utf-8//IGNORE", "gbk//IGNORE");
          iconv_t m_gbk2utf8_ = iconv_open("utf-8//IGNORE", "GB18030//IGNORE");
          size_t ret = iconv(m_gbk2utf8_, &inbuf, &inlenbuf, &outbuf, &outlenbuf);
          iconv_close(m_gbk2utf8_);

         if(ret == size_t(-1) && errno == EILSEQ)
          {
                  *outlen = outbuf - out;
                  *inlen = inbuf - in;
                  return -2;
          } else {
                  *outlen = outbuf - out;
                  *inlen = inbuf - in;
                  if(ret == size_t(-1) && errno == E2BIG)
                          return -1;
                  else
                          return *outlen;
          }
}

int _utf16_to_utf8(const my_sso_string &src, my_sso_string &result)
{
	return s_iconv(src,result,"utf-16","UTF8");
}
int _utf8_to_utf16(const my_sso_string &src, my_sso_string &result)
{
	return s_iconv(src,result,"UTF8","utf-16");
}
int _utf16_to_gbk(const my_sso_string &src, my_sso_string &result)
{
	return s_iconv(src,result,"utf-16","GB18030");
}
int _gbk_to_utf16(const my_sso_string &src, my_sso_string &result)
{
	return s_iconv(src,result,"GB18030","utf-16");
}

int _gbk_to_utf16le(const my_sso_string &src, my_sso_string &result)
{
	return s_iconv(src,result,"GB18030","utf-16le");
}


int _utf8_to_gbk(const my_sso_string &src, my_sso_string &result)
{
	return s_iconv(src,result,"UTF8","GB18030");
}
int _gbk_to_utf8(const my_sso_string &src, my_sso_string &result)
{
	return s_iconv(src,result,"GB18030","UTF8");
}


my_sso_string trim_left(my_sso_string &s,const my_sso_string &drop=" ")
{
	return s.erase(0,s.find_first_not_of(drop));
}

my_sso_string trim(my_sso_string &s,const my_sso_string &drop=" ")
{
	s.erase(s.find_last_not_of(drop)+1);
	return s.erase(0,s.find_first_not_of(drop));
}
/*my_sso_string trim_line1(my_sso_string &s)
{
	const my_sso_string drop1="\n";
	s.erase(s.find_last_not_of(drop1)+1);
	return s.erase(0,s.find_first_not_of(drop1));
}
my_sso_string trim_line2(my_sso_string &s)
{
	const my_sso_string drop2="\r";
	s.erase(s.find_last_not_of(drop2)+1);
	return s.erase(0,s.find_first_not_of(drop2));
}
my_sso_string trim_line(my_sso_string &s)
{
	trim_line1(s);
	trim_line2(s);
}

*/
my_sso_string  trimall(my_sso_string &src)
{
	int len=src.length();
	if(len==0)
		return "";
	int i=0;
	while(i<len&&isspace(src[i])){
		char temp[2];
		temp[0]=src[i];
		temp[1]='\0';
		trim(src,temp);
		++i;
	}
	len=src.length()-1;
	while(len>=0&&isspace(src[len])){
		char temp[2];
		temp[0]=src[len];
		temp[1]='\0';
		trim(src,temp);		
		--len;
	}
	return src;
}

//处理gbk的截长问题
my_sso_string cut_strLen(my_sso_string &src,int len){
	unsigned char tmp;
	int realLen=0;
	
	if (src.length()<=len)
	{
		return src;
	}else{
		for (int i=0;i<src.length();++i)
		{
			tmp=src[i];
			if (tmp>=128)
			{
				if ((i+1)>(len-1))
				{
					break;
				}
				realLen=i+1;
				++i;
			}else{
				if(i>(len-1))
					break;
				realLen=i;
			}
		}
		src=src.substr(0,realLen+2);
		src[realLen+1]='\0';
	}
	return src;
}

//全角转半角
my_sso_string ToDBS(my_sso_string str) {
	my_sso_string result = "";
	unsigned char tmp;
	unsigned char tmp1;
	for (unsigned int i = 0; i < str.length(); i++) {
		tmp = str[i];
		tmp1 = str[i + 1];
		if (tmp == 163) {
			result += (unsigned char) str[i + 1] - 128;
			i++;
			continue;
		} else if (tmp > 163) {
			result += str.substr(i, 2);
			i++;
			continue;
		} else if (tmp >=128) {
			if(tmp==161&&tmp1==161)
				result += " ";
			i++;
			continue;
		} else {
			result += str.substr(i, 1);
		}
	}
	return result;
}

void toUpper(std::basic_string<char>& s)
{
	for (std::basic_string<char>::iterator p = s.begin();p!=s.end();++p){
		*p =toupper(*p);
	}
}

void toLower(std::basic_string<char>& s)
{
	for (std::basic_string<char>::iterator p = s.begin();p!=s.end();++p){
		*p =tolower(*p);
	}
}

char * cltrim ( char * sLine )
{
	if(sLine==NULL)
		return sLine;
	char* pos=sLine;
	while ( *pos && isspace(*pos) )
		pos++;
	strcpy(sLine,pos);
	return sLine;
}
char * crtrim ( char * sLine )
{
	if (sLine==NULL)
		return sLine;
	char * p = sLine + strlen(sLine) - 1;
	while ( p>=sLine && isspace(*p) )
		p--;
	p[1] = '\0';
	return sLine;
}
char * ctrim ( char * sLine )
{
	if (sLine==NULL)
	{
		return sLine;
	}
	return cltrim ( crtrim ( sLine ) );
}

int separateIP(const char *value, char *ip, int *port)
{
	char * pos = strchr((char*)value,':');
        if(!pos)
        {
                return -1;
        }
        //*port = htons(atoi(pos+1));
        *port = atoi(pos+1);
        int len = pos - value;
        memcpy(ip,value,len);
        ip[len] = '\0';
        return 0;	
}



int strtoks(const char *src,const char *sep,std::vector<my_sso_string> &ret){
	ret.clear();
	
	//fprintf(stderr,"[strtoks]%s:%s\n",src,sep);
	const char *pos = NULL;
	int len;
	char des[1024];
	des[1023]='\0';	
	while(pos=strstr(src,sep)){
		len=pos-src;
		strncpy(des,src,len);
		des[len]='\0';
		fprintf(stderr,"key=%s\n",des);
		ret.push_back(des);
		src=pos+strlen(sep);;
	}
	strncpy(des,src,1024);
	fprintf(stderr,"key=%s\n",des);
    ret.push_back(des);
	return 0;
}


/*
int getkeyofUrl( char* url,char* ukey,int &nextPos){
	const char* curpos;
	if (url==NULL||ukey==NULL)
	{
		fprintf(stderr, "getkeyofUrl:url ,ukey is NULL!\n");
		return -1;
	}


	if (nextPos==0)
	{
		//cut blank char
		while ((*url)==' '||(*url)=='\t')
		{
			++nextPos;
		}
		if (strncmp(url,STRHTTP,STRHTTP_LEN)==0)
		{
			nextPos+=STRHTTP_LEN;
		}
	}
	curpos=url+nextPos;
	if((*curpos)=='\0'||(*curpos)==' '||(*curpos)=='\t')
		return -1;
	const char* keyBeg=curpos;

	while ((*curpos)!='\0'&&(*curpos)!='.'&&(*curpos)!='\\'
		&&(*curpos)!='/'&&(*curpos)!='\t'&&(*curpos)!=' ')
	{
		++curpos;
	}
	strncpy(ukey,keyBeg,curpos-keyBeg);
	nextPos=curpos-url+1;
	if((*curpos)=='\0'||(*curpos)==' '||(*curpos)=='\t')
		nextPos=curpos-url;
	ukey[curpos-keyBeg]='\0';
	return 0;
}*/ 
  
/*
int getHostofUrl( char* srcUrl,char* dhost,int hostLen){
	if (srcUrl==NULL||dhost==NULL)
	{
		fprintf(stderr, "getHostofUrl:desString or srcUrl is NULL!\n");
		return -1;
	}
	ctrim(srcUrl);
	if(strlen(srcUrl)==0)
		return -1;
	 char *pos=srcUrl;
	 char *urlBeg;
	 char *urlEnd;

	char* pfind=cfind(pos,STRHTTP);
	if (pfind==NULL)
	{
		pfind=pos;
	}else
	{
		pos=pfind+STRHTTP_LEN;
	}
	urlBeg=pfind;
	while((*pos)&&(*pos)!='/'){
		++pos;
	}
	urlEnd=pos-1;
	if((urlEnd-urlBeg+1)<=0)
		return -1;
	strncpy(dhost,urlBeg,urlEnd-urlBeg+1);
	if((urlEnd-urlBeg+1)>=hostLen)
		return -1;
	dhost[urlEnd-urlBeg+1]='\0';
	return 0;
}
*/

/**
* 比较好的hash算法
* http://murmurhash.googlepages.com/
*/
unsigned int _sgHash(const void *key, int len)
{
	const unsigned int m = 0x5bd1e995;
    const int r = 24;
    const int seed = 97;
    unsigned int h = seed ^ len;
    // Mix 4 bytes at a time into the hash
    const unsigned char *data = (const unsigned char *)key;
    while(len >= 4)
    {
    	unsigned int k = *(unsigned int *)data;
    	k *= m; 
    	k ^= k >> r; 
    	k *= m; 
    	h *= m; 
    	h ^= k;
    	data += 4;
    	len -= 4;
    }
    // Handle the last few bytes of the input array
    switch(len)
    {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
        h *= m;
    };
    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

my_sso_string  str_find(my_sso_string &src,int ibeg,my_sso_string begtag,my_sso_string endtag,int &ifind)
{
	int pos1=src.find(begtag,ibeg);
	if(pos1==my_sso_string::npos)
	{	
		ifind=-1;
		return "";
	}
	pos1+=begtag.size();
	int pos2=src.find(endtag,pos1);
	if(pos2==my_sso_string::npos)
	{
		ifind=-1;
		return "";
	}
	ifind=pos1;
	return src.substr(pos1,pos2-pos1);
}


uint64_t hex2uint64(const my_sso_string hexStr)
{
		uint64_t ret;
        char *offset;
        if (hexStr.length() > 2){
            if (hexStr[0] == '0' && hexStr[1] == 'x'){
                return strtoull(hexStr.c_str(),&offset,0);
            }
        }
        ret=strtoull(hexStr.c_str(),&offset,16);
		fprintf(stderr,"srcStr:%s,desInt:%llx\n",hexStr.c_str(),ret);
		return ret;
}
/***注意：有的linux下一个中文占用三个字节,有的占用2个字节；
 * 			windows占两个字节***/  
void chinese_or_english(char *str)  
{  
  char chinese[4] = {0};  
  for (int i = 0; i < strlen(str); i++) {  
    //if (str[i] >= 0 && str[i] <= 127) {      //ascII  
    if ((str[i] & 0x80) == 0) {   //chinese:the top is 1  
      cout<<"alpha:"<<str[i]<<endl;  
    }  
    //else if (str[i] < 0){  
    else {  
      chinese[0] = str[i];  
      chinese[1] = str[i + 1];  
      //chinese[2] = str[i + 2];  
      i++;    //skip one more  
      //i++;  
      printf("chinese:%s\n", chinese);  
    }  
  }  
}  
int chinese_or_english_vec(char *str, std::vector<string>& vecout)  
{  
	char chinese[4] = {0};
	int j = 0;
	string tmp;
	ostringstream oss;
	if(strlen(str) == 0)
	{
		return 0;
	}
	for (int i = 0; i < strlen(str); i++) {  
		if ((str[i] & 0x80) == 0) {   //chinese:the top is 1  
			oss.str("");
			oss << str[i];
			vecout[j++] = oss.str().c_str();
    	}  
		else {  
			chinese[0] = str[i];  
			chinese[1] = str[i + 1];  
			i++;    //skip one more  
			printf("chinese:%s\n", chinese);
			oss.str("");
			oss << chinese;
			vecout[j++] = oss.str().c_str();
		}
	}
	return 0;
}

int32_t chinese_or_english_length(char *str)  
{  
	int32_t length = 0;
  char chinese[4] = {0};  
  for (int i = 0; i < strlen(str); i++) {  
    if ((str[i] & 0x80) == 0) {   //chinese:the top is 1  
		length++;
    }  
    else {  
      chinese[0] = str[i];  
      chinese[1] = str[i + 1];  
      i++;    //skip one more  
      //i++;  
	 length += 2;
    }  
  }
	return length;  
}  

