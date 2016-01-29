#ifndef __ENCODING_UTILITY_H
#define __ENCODING_UTILITY_H

#include <cstddef>

typedef unsigned short gchar_t;

//中文空格
static const gchar_t CHINESE_SPACE_CHAR = 0xA1A1;
//常用的另一个中文空格
static const gchar_t CHINESE_SPACE_CHAR2 = 0xA0A3;
//ASCII的空格转换后的结果
static const gchar_t SPACE_CONVERSION_CHAR = 0xA0A1;
//ASCII的tab转换后的结果
static const gchar_t TAB_CONVERSION_CHAR = 0x0920;

class EncodingConvertor{
private:
    static EncodingConvertor *dinstance;

    static const int MAP_ARR_SIZE = 256;

    unsigned short** pFanToJianMap;

    EncodingConvertor();
    ~EncodingConvertor();

    bool InitFan2JianMap();
    bool DestroyFan2JianMap();

    void EncodeChar(const char *p, char &hc, char &lc);
    void DecodeChar(const char *p, char *q); 

public:
    static EncodingConvertor *Instance(){
        if(dinstance) return dinstance;
        dinstance = new EncodingConvertor();
        return dinstance;
    }

    static void ReleaseInstance(){
        delete dinstance;
        dinstance = NULL;
    }

    //半角转全角
    // return : 全角字符的个数,不包含最后的0， 失败返回-1
    int dbc2sbc(const char *p, char *q, int qlen, bool trans_fan = true, bool trans_alpha = true);

    //全角转半角
    // return : 字符串的字节数，失败返回-1
    int sbc2dbc(const char *p, char *q, int qlen, bool trans_fan = true, bool trans_alpha = true);

    //url编码
    bool urlEncode(const char *src, char *dst, int dlen);

    //url解码
    bool urlDecode(const char *src, char *dst, int dlen);

};

inline
bool isGBKChnChar(gchar_t wc) {
    unsigned char * upc = (unsigned char *)&wc;
    unsigned char b1 = *upc;
    unsigned char b2 = *(upc+1);
    
    //GBK中汉字编码在三个区域
    //GBK-2区(GB2312汉字)：B0A1-F7FE
    //GBK-3区：8140-A0FE
    //GBK-4区：AA40-FEA0
    if (((b1 >= 0xB0) && (b1 <= 0xF7) && (b2 >= 0xA1) && (b2 <= 0xFE)) || 
        ((b1 >= 0x81) && (b1 <= 0xA0) && (b2 >= 0x40) && (b2 <= 0xFE)) || 
        ((b1 >= 0xAA) && (b1 <= 0xFE) && (b2 >= 0x40) && (b2 <= 0xA0)))
    {
        return true;
    }

    return false;
}

#endif
