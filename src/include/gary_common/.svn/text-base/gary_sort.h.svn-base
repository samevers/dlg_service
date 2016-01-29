#ifndef __GARY_SORT_H_
#define __GARY_SORT_H_

#include "twsetypedef.h"

namespace spaceGary
{

template <class TDAT>
inline
TINT32 QuickSortAsc(TDAT *pBuf,TINT32 dwLeft ,TINT32 dwRight)
{
	if(dwLeft >= dwRight) return 0;
	register TINT32 i,j,k;
	TDAT V,S;
	for(i=1,j=dwRight-1;j>>=1;i++);
	TINT32 dwStackSize = i<<6;
	TINT32 *pStack = new TINT32[dwStackSize];
	//int pStack[dwStackSize];
	dwStackSize -= 4;

	pStack[0] = dwRight;
	pStack[1] = dwLeft;
	i = 2;
	for(;i;)
	{
		dwLeft = pStack[--i];
		dwRight = pStack[--i];
		if(dwRight - dwLeft < 16)
		{
			for(j=dwRight;j>dwLeft;j--)
			{
				if(pBuf[j] < pBuf[j-1])
				{
					V = pBuf[j];
					pBuf[j] = pBuf[j-1];
					pBuf[j-1] = V;
				}
			}
			for(j=dwLeft+2;j<=dwRight;j++)
			{
				V = pBuf[j];
				k = j;
				while(V<pBuf[k-1])
				{
					pBuf[k] = pBuf[k-1];
					k--;
					pBuf[k] = V;
				}
			}
			continue;
		}
		j = dwRight - 1;
		V = pBuf[(dwLeft + dwRight) >> 1];
		pBuf[(dwLeft + dwRight) >> 1] = pBuf[j];
		pBuf[j] = V;
		
		if(pBuf[j] > pBuf[dwRight])
		{
			V = pBuf[j];
			pBuf[j] = pBuf[dwRight];
			pBuf[dwRight] = V;
		}
		if(pBuf[dwLeft] > pBuf[dwRight])
		{
			V = pBuf[dwLeft];
			pBuf[dwLeft] = pBuf[dwRight];
			pBuf[dwRight] = V;
		}
		if(pBuf[dwLeft] > pBuf[j])
		{
			V = pBuf[j];
			pBuf[j] = pBuf[dwLeft];
			pBuf[dwLeft] = V;
		}
		V = pBuf[j];
		k = dwLeft;
		for(;;)
		{
			while(pBuf[++k] < V);
			while(pBuf[--j] > V)
				if(j == k)
					break;
			if(k>=j)break;
			S = pBuf[k];
			pBuf[k] = pBuf[j];
			pBuf[j] = S;
		}
		V = pBuf[dwRight - 1];		
		pBuf[dwRight - 1] = pBuf[k];
		pBuf[k] = V;
		if(i >= dwStackSize)
		{
			delete[] pStack;
			return -1;
		}
		if(dwRight > k+1)
		{
			pStack[i++] = dwRight;
			pStack[i++] = k+1;
		}
		if(k-1 > dwLeft)
		{
			pStack[i++] = k-1;
			pStack[i++] = dwLeft;
		}
	}
	delete[] pStack;
	return 0;

}

};

#endif


