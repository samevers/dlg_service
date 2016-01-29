/*******************************************************

* 文件名： gary_common.h
* 功  能： 针对常用函数的封装
* 版  本： version 1.0 2010-4-18
* 修  改：

********************************************************/

#ifndef __GARY_COMMON_H_
#define __GARY_COMMON_H_
#include "twsetypedef.h"

namespace spaceGary
{

//是否是GBK中文
 /**
        *中文部分:
        *[A1A0 - A1FF]  	全角标点符号   			过滤掉
        *[A2A0 - A2FF]     各种标题标号   			过滤掉
        *[A3A0 - A3FF]   	与基本ASCII字符对应     	转换成相应的ASCII字符,英文分词处理
        *[A4-A5,A0-FF]     日文片假名				中文分词处理
        *[A6-A9,A0-FF]   	各种符号					过滤掉
        *[A8-A9,40-9F]   	各种符号					过滤掉
        *[B0-F7,A0-FF]   	GB2312汉字				中文分词处理
        *[81-A0,40-FF]   	扩展汉字第一部分			中文分词处理或过滤掉
        *[AA-FE,40-AF]  	扩展汉字第二部分			中文分词处理或过滤掉     
*/
inline 
TBOOL IsStrictGBKChar(const TCHAR* szWord)
{
    const TUCHAR c1 = (const TUCHAR)*szWord;
    const TUCHAR c2 = (const TUCHAR)*(szWord+1);

    if (
                ((c1 >= 0xA1 && c1 <= 0xA9) && (c2 >= 0xA0 && c2 <= 0xFF)) ||
                ((c1 >= 0xA8 && c1 <= 0xA9) && (c2 >= 0x40 && c2 <= 0xFF)) ||
                ((c1 >= 0xB0 && c1 <= 0xF7) && (c2 >= 0xA0 && c2 <= 0xFF)) ||
                ((c1 >= 0x81 && c1 <= 0xA0) && (c2 >= 0x40 && c2 <= 0xFF)) ||
                ((c1 >= 0xAA && c1 <= 0xFE) && (c2 >= 0x40 && c2 <= 0xAF)) 
         )
     {
		 return TRUE;
     }

     return FALSE;
}


//是否包好中文GBK
inline
TBOOL IsIncludeChnGBK(const TCHAR* szText, TUINT32 udwTextLen)
{
	const TCHAR* pCur = szText;

	while (pCur < szText+udwTextLen)
	{
		if (*pCur < 0)
		{
			if ((pCur+1) < (szText + udwTextLen))
			{
				if (IsStrictGBKChar(pCur))
				{
					pCur += 2;
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
		}

		pCur++;
	}
	return FALSE;
}

}; //end space

#endif