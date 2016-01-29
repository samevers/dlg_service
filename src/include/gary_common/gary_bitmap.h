#ifndef __GARY_BITMAP_H_
#define __GARY_BITMAP_H_

#include "twsetypedef.h"

namespace spaceGary
{

//设置64位上的某一位
inline
void SetBitMap(TUINT64 &bitMap, TUINT32 udwPos)
{
	 *((TUINT8 *)&bitMap + (udwPos >> 3)) |= 1 << (udwPos & 7);
}

//获得某一位的状态
inline
TBOOL GetBitMap(TUINT64 &bitMap, TUINT32 udwPos)
{
	return (*((TUINT8 *)&bitMap + (udwPos >> 3)) & (1 << (udwPos & 7))) != 0;
}


//获得64位上的map个数
inline
TUINT32 GetBitMapCount(TUINT64 &bitMap)
{
TINT8 BYTE_COUNTS[] = {	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
						1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
						1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
						2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
						1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
						2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
						2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
						3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
						1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
						2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
						2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
						3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
						2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
						3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
						3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
						4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8	};

	TUINT32 udwCount = 0;
	TUINT8 *pTmp = (TUINT8 *)&bitMap;
	for (TUINT32 i = 0; i < 8; i++)
	{
		udwCount += BYTE_COUNTS[*(pTmp+i) & 0xFF];
	}

	return udwCount;
}


};

#endif


