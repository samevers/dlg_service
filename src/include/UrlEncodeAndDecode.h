#ifndef URL_ENCODE_AND_DECODE_H_
#define URL_ENCODE_AND_DECODE_H_
#include <string>
#include <stdio.h>
#include <stdlib.h>
using std::string;

/* Converts a hex character to its integer value */
inline
char from_hex(char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
inline
char to_hex(char code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
inline
char *url_encode(const char *str) {
  const char *pstr = str;
  char *buf = (char*)malloc(strlen(str) * 3 + 1), *pbuf = buf;
  while (*pstr) {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
      *pbuf++ = *pstr;
    else if (*pstr == ' ') 
      *pbuf++ = '+';
    else 
      *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
inline
char *url_decode(const char *str) {
  const char *pstr = str;
  char *buf = (char*)malloc(strlen(str) + 1), *pbuf = buf;
  while (*pstr) {
    if (*pstr == '%') {
      if (pstr[1] && pstr[2]) {
        *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
        pstr += 2;
      }
    } else if (*pstr == '+') { 
      *pbuf++ = ' ';
    } else {
      *pbuf++ = *pstr;
    }
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

/* cpp interface */
inline
bool is_url_encode_str(const string & str)
{
	int naive_count = 0;
	const char * p = str.c_str();
	while(*p)
	{
		if (*p == '%')
		{
			if (p[1]&&p[2])
			{
				/*if(!isdigit(p[1]) && (p[1] < 'a' || p[1] > 'f'))*/
				if(!isdigit(p[1]) && !(p[1] >= 'a' &&  p[1] <= 'f') && !(p[1] >= 'A' && p[1] <= 'F'))
					return false;
				/*if(!isdigit(p[2]) && (p[2] < 'a' || p[2] > 'f'))*/
				if(!isdigit(p[2]) && !(p[2] >= 'a' && p[2] <= 'f') && !(p[2] >= 'A' && p[2] <= 'F'))
					return false;
				p += 3;
			}else{
				return false;
			}
		} else if (*p == '+' || *p == ' ' || *p == '~')
		{
			p ++;
			naive_count ++;
		}else {
			return false;
		}
	}
	
	if (naive_count == (int)str.size())
	{
		return false;
	}
	return true;
}

inline
string url_decoded_str(const string & str)
{
	const char * data = str.c_str();
	char * value = url_decode(data); 
	string ret = value;
	if (value != NULL)
	{
		free(value);
		value = NULL;
	}
	return ret;
}

inline
string url_encoded_str(const string & str)
{
	const char * data = str.c_str();
	char * value = url_encode(data); 
	string ret = value;
	if (value != NULL)
	{
		free(value);
		value = NULL;
	}
	return ret;
}
#endif
