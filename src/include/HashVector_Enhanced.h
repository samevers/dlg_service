//实现了一个基于vector的哈希
//优点：相当于map 少了每个节点中的指针，减少了空间的使用量； 先hash， 每个桶中vector在内存中连续， 提供了查询时间
//使用范围： 符合3段式的场景 1. 读入数据  2. 排序  3. 对排序的数据进行查询， 而不是修改
//author： garysong  2009-06-22 
//vertion：2.0， 需要结构体中存在 char *word; 
//change： 1.2 garysong  2009-09-10 增加根据word进行find的功能， 支持HASH_BASIC 定制
//change:  2.0 garysong  2009-10-27 查询串变长， 考虑到了cache命中问题和查询速度

#ifndef __HASHVECTOR_ENHANCED_H
#define __HASHVECTOR_ENHANCED_H

#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include <iostream>


//using namespace std;

/*
NODETYPE的要求
1. 增加对HASH_BASIC泛型化， 也支持key的泛型话，
2. 要求必须定义 unsigned int word_offset; 标示一个词的偏移
*/

template <typename NODETYPE, unsigned int HASH_BASIC=11111>
class CHashVector_Enhanced {

public:

CHashVector_Enhanced() {
	m_wordsBuf = NULL;
	m_wordsBufSize = 0;
}

~CHashVector_Enhanced() {
	if (NULL != m_wordsBuf) {
		delete[] m_wordsBuf;
		m_wordsBuf = NULL;
	}
}

/*获得一个词的hash*/
unsigned int GetHashValue(const char *str) {
	unsigned index=0;
	while(*str) {
		index^=((index<<5) + (index>>2) + *str++);
	}

	return index%HASH_BASIC;
}

/*比较函数*/
bool Compare(NODETYPE &t1, NODETYPE &t2) {
	return strcmp(m_wordsBuf+t1.word_offset, m_wordsBuf+t2.word_offset) < 0;
}

/*添加元素， 原word会变为标志其在vect中的偏移量*/
bool AddItem(NODETYPE & item, const char *word) {
	//保存item中word指针
	item.word_offset = m_charVect.size();
	const char *p = word;
	while (*p) {
		m_charVect.push_back(*p++);
	}
	m_charVect.push_back('\0');
	unsigned int hashValue = GetHashValue(word);
	data[hashValue].push_back(item);
	return true;
}


/*元素排序， 并附加实现内部转换的功能*/
/*bool Sort() {
	for (int i = 0; i < HASH_BASIC; i++) {
		vector<NODETYPE> &nodeVect = data[i];
	    sort(nodeVect.begin(), nodeVect.end(), (func)Compare);
	}
	return true;
}*/

bool Wap(NODETYPE &node1, NODETYPE &node2) {
	NODETYPE tmp;
	tmp = node1;
	node1 = node2;
	node2 = tmp;
	
	return true;
}

int Partition(std::vector<NODETYPE> &vect, int left, int right) {
	int key = left;
	const char *pchKeyValue = m_wordsBuf+vect[key].word_offset;
	
	//处理特殊情况， 只有两个元素， 第一个元素大于第二个元素
	if (right == left+1) {
		if(strcmp(pchKeyValue, m_wordsBuf+vect[right].word_offset) > 0) {
			Wap(vect[key], vect[right]);
		}

		return right;
	}
	
	
	left++;
    while (left < right) {
		while (left < right && strcmp(m_wordsBuf+vect[left].word_offset, pchKeyValue) < 0) 
			left++;
		while (left <= right && right > key && strcmp(m_wordsBuf+vect[right].word_offset, pchKeyValue) >= 0) 
			right--;
		if (left < right) {
			Wap(vect[left], vect[right]);
			left++;
			right--;
		}
	}
    
	//存在相等的特殊情况， 需要处理
	if (right == left && strcmp(m_wordsBuf+vect[right].word_offset, pchKeyValue) >= 0) {
		right--;
	}
    
	
	if (right == key) {
		return right;
	}
	
	Wap(vect[key], vect[right]);
	
	return right;
}

void QSort(std::vector<NODETYPE> &vect, int left, int right) {
	if (left < right) {
		int key = Partition(vect, left, right);
		QSort(vect, left, key-1);
		QSort(vect, key+1, right);
	}
}
bool Sort() {
	if (NULL != m_wordsBuf) {
		delete[] m_wordsBuf;
	}
	m_wordsBuf = new char[m_charVect.size()];
	memcpy(m_wordsBuf, &m_charVect[0], m_charVect.size());
	for (unsigned int i = 0; i < HASH_BASIC; i++) {
		
		std::vector<NODETYPE> &nodeVect = data[i];
	    QSort(nodeVect, 0, nodeVect.size()-1);
	}

   // delete[] m_wordsBuf;
	//m_wordsBuf = NULL;
	return true;
}


/*查找元素*/
NODETYPE *Find(const char *word) {
	unsigned int hashValue = GetHashValue(word);
	std::vector<NODETYPE> &vect = data[hashValue];
	int left = 0;
	int right = vect.size()-1;
    int mid = 0;
	int compareRet;


    
	//for (int i = 0; i <= right; i++) {
	//	fprintf(stdout, "%s\n", m_wordsBuf+vect[i].word_offset);
	//}
	
	while (right >= left) {
		mid = (left+right)/2;
		compareRet = strcmp(word, m_wordsBuf+vect[mid].word_offset);
		if (compareRet == 0) {
			return &(vect[mid]);
		}
		else if (compareRet > 0) {
			left = mid+1;
		}
		else {
			right = mid-1;
		}
	}

	return NULL;
}


/*写入文件*/
bool WriteFile(FILE *fp) {
	if (!fp) {
		return false;
	}

	//写入wordsBuf
	m_wordsBufSize = m_charVect.size();
	fwrite(&m_wordsBufSize, sizeof(m_wordsBufSize), 1, fp);

	//为提高cache，进行调整
	if (NULL != m_wordsBuf) {
		delete[] m_wordsBuf;
	}
	m_wordsBuf = new char[m_wordsBufSize];
	if (NULL == m_wordsBuf) {
		fprintf(stderr, "alloc m_wordsBuf is error!\n");
		return false;
	}
	char *pWord = m_wordsBuf;
	for (unsigned int i = 0; i < HASH_BASIC; i++) {
		std::vector<NODETYPE> &vect = data[i];
		for (typename std::vector<NODETYPE>::iterator itr = vect.begin(); itr != vect.end(); ++itr) {
			unsigned int word_offset = itr->word_offset;
			itr->word_offset = (unsigned int)(pWord-m_wordsBuf);
			while (m_charVect[word_offset]) {
				*pWord++ = m_charVect[word_offset++];
			}
			*pWord++ = '\0';
		}
	}
    //int writeRet = fwrite(&m_charVect[0], sizeof(char), m_wordsBufSize, fp);
	uint64_t writeRet = fwrite(m_wordsBuf, sizeof(char), m_wordsBufSize, fp);
	if (uint64_t(writeRet) != m_wordsBufSize) {
		fprintf(stderr, "error in fwrite! writeRet = %ld, m_wordsBufSize = %ld\n", writeRet, m_wordsBufSize);
		return false;
	}
	fprintf(stdout, "wordsBufSize = %d\n",  m_wordsBufSize);
	
	//写入HashVector大结构
	unsigned int hashSize = HASH_BASIC;
	fprintf(stdout, "hash_size = %u\n",  hashSize);
	fwrite(&hashSize, sizeof(unsigned int), 1, fp);
	for (unsigned int i = 0; i < HASH_BASIC; i++) {
		fwrite(&i, sizeof(unsigned int), 1, fp);
		std::vector<NODETYPE> &vect = data[i];
		unsigned int vectSize = vect.size();
		fwrite(&vectSize, sizeof(unsigned int), 1, fp);
		if (vectSize > 0) {
			fwrite(&vect[0], sizeof(NODETYPE), vectSize, fp);
		}
	}

	delete[]  m_wordsBuf;
	m_wordsBuf = NULL;
	return true;
}

/*从文件中读出*/
bool ReadInFile(FILE *fp) {
	if (!fp) {
		return false;
	}
	fread(&m_wordsBufSize, sizeof(m_wordsBufSize), 1, fp);
	if (NULL != m_wordsBuf) {
		delete[] m_wordsBuf;
		m_wordsBuf = NULL;
	}
	m_wordsBuf = new char[m_wordsBufSize];
	if (NULL == m_wordsBuf) {
		fprintf(stderr, "alloc m_wordsBuf error!\n");
		return false;
	}
	uint64_t readRet = fread(m_wordsBuf, sizeof(char), m_wordsBufSize, fp);
	if (uint64_t(readRet) != m_wordsBufSize) {
		fprintf(stderr, "error! read m_wordsBuf error! readRet = %ld    m_wordsBufSize = %ld\n", readRet, m_wordsBufSize);
		return false;
	}

    unsigned int hashSize;
	if (fread(&hashSize, sizeof(unsigned int), 1, fp) != 1) {
		fprintf(stderr, "error! error from load hashSize!\n");
		return false;
	}
	
	//如果两个hash basic不一致， 则失败
	if (hashSize != HASH_BASIC) {
		fprintf(stderr, "error! the hash size [%d] != HASH_BASIC [%d]!\n", hashSize, HASH_BASIC);
		return false;
	}

	for (unsigned int i = 0; i < hashSize; i++) {
		unsigned int index;
		fread(&index, sizeof(unsigned int), 1, fp);
		std::vector<NODETYPE> &vect = data[index];
		vect.clear();
		unsigned int vectSize;
		fread(&vectSize, sizeof(unsigned int), 1, fp);
		if (vectSize > 0) {
			vect.resize(vectSize);
			vect.assign(vectSize, NODETYPE());
			fread(&vect[0], sizeof(NODETYPE), vectSize, fp);
		}
	}

	return true;
}

/*清空*/
bool Clear() {
	for (unsigned int i = 0; i < HASH_BASIC; i++) {
		data[i].clear();
	}

	m_charVect.clear();
	if (NULL != m_wordsBuf) {
		delete[] m_wordsBuf;
		m_wordsBuf = NULL;
	}

	return true;
}

public:
	std::vector<NODETYPE> data[HASH_BASIC];

private:
	std::vector<char>   m_charVect;
    char *m_wordsBuf;        //用于存储查询词的buf
	uint64_t m_wordsBufSize; 
};

#endif


