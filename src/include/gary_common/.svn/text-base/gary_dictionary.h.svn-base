/*******************************************************

* 文件名： gary_dictionary.h
* 功  能： 针对dictionary的封装
* 版  本： version 1.0 2010-4-14
* 修  改：

********************************************************/

#ifndef __GARY_DICTIONARY_H
#define __GARY_DICTIONARY_H

#include "twsetypedef.h"
#include <stdlib.h>
#include <memory.h>

namespace spaceGary
{


#define ENGWORD_SIGN_PART 1   //标示是词的一部分
#define ENGWORD_SIGN_WORD 2   //标示是词的结束，即该路径上是一个完整的词
#define ENGWORD_SIGN_ROOT 0   //标记特殊的root节点


//节点结构
struct SCharNode {

	TINT32 dwIndex;     //节点索引， 0为初始值， 正数为叶子节点， 负数为其他节点
	SCharNode * next[256];  //256叉树

	SCharNode() 
	{
		dwIndex = 0;
		memset(next, 0, 256*sizeof(SCharNode *));
	}

	void Reset()
	{
		dwIndex = 0;
		memset(next, 0, 256*sizeof(SCharNode *));
	}
};


/* 词典类 */
template <TUINT32 T_udwDicSize>
class CDictionary
{
public:

	CDictionary();
	~CDictionary();

	//查找一个词, 返回该词的索引号， 正数正确
	TINT32 LookUp(const TCHAR *pWord, TUINT32 udwWordLen);
	
	//查找一个词， 返回节点
	const SCharNode* LookUp(const TCHAR *pWord, TUINT32 udwWordLen, const SCharNode *pBeginNode);

	//增加一个词， 返回该词的索引号
	TINT32 AddWord(const TCHAR *pWord, TUINT32 udwWordLen);
	
	//重置词典状态
	void Reset();

private:
	
	//分配节点
	SCharNode* AllocateNode();

private:

	SCharNode * m_pRoot;      //树的根节点
	SCharNode * m_pNodeBuf;   //内存池
	TINT32 m_dwCurNodeIndex; //标示内存池的使用情况
};

template <TUINT32 T_udwDicSize>
CDictionary<T_udwDicSize>::CDictionary()
{
	m_pNodeBuf = new SCharNode[T_udwDicSize];
	m_pRoot = new SCharNode;
	m_dwCurNodeIndex = 1; //空出0
}

template <TUINT32 T_udwDicSize>
CDictionary<T_udwDicSize>::~CDictionary()
{
	if (NULL != m_pNodeBuf)
	{
		delete[] m_pNodeBuf;
		m_pNodeBuf = NULL;
	}

	if (NULL != m_pRoot)
	{
		delete m_pRoot;
		m_pRoot = NULL;
	}
}

/******************** 词典重置 ***********************/
template <TUINT32 T_udwDicSize>
inline
void CDictionary<T_udwDicSize>::Reset()
{
	m_dwCurNodeIndex = 1;
	m_pRoot->Reset();

}

/* 查找一个词, 返回该词的索引号， 正数正确 */
template <TUINT32 T_udwDicSize>
inline
TINT32 CDictionary<T_udwDicSize>::LookUp(const TCHAR *pWord, TUINT32 udwWordLen)
{
	if (NULL == pWord || udwWordLen == 0)
	{
		return 0;
	}

	SCharNode *pCharNode = m_pRoot;
	for (TUINT32 i = 0; i < udwWordLen; i++)
	{
		TUINT8 ubyIdx = *(const TUINT8 *)(pWord + i);
		if (pCharNode->next[ubyIdx] == NULL)
		{
			return 0;
		}
		pCharNode = pCharNode->next[ubyIdx];
	}

	return pCharNode->dwIndex;
	
}

/* 查找一个词, 返回节点 */
template <TUINT32 T_udwDicSize>
inline
const SCharNode* CDictionary<T_udwDicSize>::LookUp(const TCHAR *pWord, TUINT32 udwWordLen, const SCharNode *pBeginNode)
{
	if (NULL == pWord || udwWordLen == 0)
	{
		return 0;
	}

	if (pBeginNode == NULL)
	{
		pBeginNode = m_pRoot;
	}

	const SCharNode *pCharNode = pBeginNode;

	TINT32 dwNeedWordMatchLen = 0;

	TCHAR cTheChar = '\0';

	for (TUINT32 i = 0; i < udwWordLen; i++)
	{
		cTheChar = *(pWord + i);

		if (0 == dwNeedWordMatchLen)
		{
			if (cTheChar < 0)
			{
				dwNeedWordMatchLen = 2;
			}
			else
			{
				dwNeedWordMatchLen = 1;

				//进行大小写转化
				if (cTheChar >= 'A' && cTheChar <= 'Z')
				{
					cTheChar -= 'A' - 'a';
				}
			}
		}

		if ((pCharNode = pCharNode->next[(TUINT8)cTheChar]) == NULL)
		{
			return NULL;
		}

		dwNeedWordMatchLen--;
	}

	return pCharNode;
	
}


//增加一个词， 返回该词的索引号，如果为正数则为正常返回
template <TUINT32 T_udwDicSize>
inline
TINT32 CDictionary<T_udwDicSize>::AddWord(const TCHAR *pWord, TUINT32 udwWordLen)
{
	if (pWord == NULL || udwWordLen == 0)
	{
		return 0;
	}

	SCharNode *pCurNode = m_pRoot;
	for (TUINT32 i = 0; i < udwWordLen; i++)
	{
		const TUINT8 ubytCharVal = *(const TUINT8 *)(pWord + i);

		//需要分配节点
		if (pCurNode->next[ubytCharVal] == NULL)
		{
			SCharNode *pNewNode = AllocateNode();
			if (NULL == pNewNode)
			{
				return 0;
			}
			
			pCurNode->next[ubytCharVal] = pNewNode;

			pCurNode = pCurNode->next[ubytCharVal];
		}
		else
		{
			pCurNode = pCurNode->next[ubytCharVal];
		}
	}

	//修改状态
	if (pCurNode->dwIndex < 0)
	{
		pCurNode->dwIndex = - pCurNode->dwIndex;
	}

	return pCurNode->dwIndex;
}

//分配节点
template <TUINT32 T_udwDicSize>
inline
SCharNode* CDictionary<T_udwDicSize>::AllocateNode()
{
	if ((TUINT32)m_dwCurNodeIndex >= T_udwDicSize)
	{
		return NULL;
	}

	SCharNode *pNode = m_pNodeBuf + m_dwCurNodeIndex;
	pNode->Reset();
	pNode->dwIndex = -m_dwCurNodeIndex; //默认都是中间状态节点
	m_dwCurNodeIndex++;

	return pNode;
}

};

#endif

