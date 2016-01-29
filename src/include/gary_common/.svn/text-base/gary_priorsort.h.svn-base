#ifndef __PRIOR_SORT_H_
#define __PRIOR_SORT_H_

/////////////////////////////////////////////////
// 描述：用于top n排序, 其实质是堆排序
// 输入:用户需要的top n结果数
// 输出:实际的top m结果数(m<=n)
// 说明:结果集不一定有序,但一定是用户插入的k(k> n)条结果中的top m条结果
// 作者：oxfordyuan
/////////////////////////////////////////////////

#include "twsetypedef.h"
#include "gary_sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

//左孩子节点
#define LEFT_PRIOR(i)        ((i) << 1)        
//右孩子节点
#define RIGHT_PRIOR(i)    (((i) << 1) + 1)

namespace spaceGary
{
	template<class T>
	class CPriorSort
	{
	public:
		CPriorSort(TUINT32 udwMaxElemNum);
		~CPriorSort();

		//每次检索前初始化
		void Reset(TUINT32 udwMaxSortElemNum = 0);

		//添加一个元素
		void Add(T tValue);

		/*
		返回结果集
		@param dwResNum(in, out) 结果个数
		@ret 最终结果集, NULL表示错误
		*/
		T *FillResult(TUINT32 &dwResNum);

	protected:
		//构造堆
		void Heapify(TINT32 dwKeyPos = 0);

	protected:	
		//最多支持的元素个数
		TUINT32 m_udwMaxElemNum;
		//当前排序的元素的最大个数
		TUINT32 m_udwMaxSortElemNum;
		//结果集缓冲区
		T *m_tpValues;	
		//用于递归
		TUINT32 *m_udwpIdxBuff;
		//当前元素个数
		TUINT32 m_udwCurrElemNum;

		//是否sort
		bool is_sort;
	};

	template<class T>
	CPriorSort<T>::CPriorSort(TUINT32 udwMaxElemNum)
	{
		assert(udwMaxElemNum >= 1);

		m_udwMaxElemNum = udwMaxElemNum;
		m_udwMaxSortElemNum = 0;

		m_tpValues = new T[m_udwMaxElemNum];

		TUINT32 udwStackSize = (TUINT32)log2(m_udwMaxElemNum)+20;
		m_udwpIdxBuff = new TUINT32[udwStackSize];
	
		m_udwCurrElemNum = 0;

		is_sort = false;
	}

	template<class T>
	CPriorSort<T>::~CPriorSort()
	{
		if (NULL != m_tpValues)
		{
			delete[] m_tpValues;
			m_tpValues = NULL;
		}

		if (NULL != m_udwpIdxBuff)
		{
			delete[] m_udwpIdxBuff;
			m_udwpIdxBuff = NULL;
		}
	}

	template<class T>
	void CPriorSort<T>::Reset(TUINT32 udwMaxSortElemNum)
	{
		m_udwCurrElemNum = 0;

		if (udwMaxSortElemNum > 0)
		{
			m_udwMaxSortElemNum = udwMaxSortElemNum;
		}
		else {
			m_udwMaxSortElemNum = m_udwMaxElemNum;
		}

		is_sort = false;
	}

	template<class T>
	void CPriorSort<T>::Add(T tValue)
	{
		// 当前存储的元素个数小于上限,则直接插入
		if(m_udwCurrElemNum < m_udwMaxSortElemNum)
		{
			for(uint32_t i = 0; i < m_udwCurrElemNum; i++) 
			{
				if(m_tpValues[i] == tValue) 
				{
					return;
				}
			}
			
			m_tpValues[m_udwCurrElemNum] = tValue;
			m_udwCurrElemNum++;

			//插入元素后,当前元素个数如果等于上限,则排序
			if (m_udwCurrElemNum == m_udwMaxSortElemNum)	
				QuickSortAsc(m_tpValues, 0, m_udwMaxSortElemNum-1);				
		}
		else if (m_udwCurrElemNum == m_udwMaxSortElemNum)
		{
			// 如果要插入的元素比当前存储的元素集合中的最小值还要小,则不插入
			if(m_tpValues[0] >= tValue)
				return;

			// 重新构造堆
			for(uint32_t i = 0; i < m_udwCurrElemNum; i++) 
			{
				if (m_tpValues[i] == tValue) 
				{
					Heapify(i);
					return;
				}
			}
			m_tpValues[0] = tValue;	
			Heapify(0);
		}
		else
		{
			printf("优先队列逻辑异常\n");
		}

		return;

	}

    template<class T>
    T* CPriorSort<T>::FillResult(TUINT32 &dwResNum)	
    {
		if (!is_sort) {
			QuickSortAsc(m_tpValues, 0, m_udwCurrElemNum-1);
			is_sort = true;
		}

        dwResNum = m_udwCurrElemNum;
        return m_tpValues;
    }

	template<class T>
	void CPriorSort<T>::Heapify(TINT32 dwKeyPos)
	{
		TINT32 dwStackPos = dwKeyPos;

		TUINT32 udwIdx = dwStackPos;
		TUINT32 udwLeftIdx, udwRightIdx;
		TUINT32 udwMinIdx; //左右孩子和本身节点进行比较后最小的那个值所在的节点
		T tValue;	

		m_udwpIdxBuff[dwStackPos] = udwIdx;

		while (dwStackPos >= dwKeyPos)
		{
			udwIdx = m_udwpIdxBuff[dwStackPos--];
			udwMinIdx = udwIdx;
			udwLeftIdx = LEFT_PRIOR(udwIdx);
			udwRightIdx = RIGHT_PRIOR(udwIdx);

			if (udwLeftIdx < m_udwMaxSortElemNum && m_tpValues[udwLeftIdx] < m_tpValues[udwIdx])
				udwMinIdx = udwLeftIdx;
			if (udwRightIdx < m_udwMaxSortElemNum && m_tpValues[udwRightIdx] < m_tpValues[udwMinIdx])
				udwMinIdx = udwRightIdx;

			if (udwMinIdx != udwIdx)
			{
				tValue = m_tpValues[udwIdx];
				m_tpValues[udwIdx] = m_tpValues[udwMinIdx];
				m_tpValues[udwMinIdx] = tValue;

				m_udwpIdxBuff[++dwStackPos] = udwMinIdx;
			}
		}
	}
}

#endif
