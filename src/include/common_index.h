#ifndef COMMON_INDEX_H_
#define COMMON_INDEX_H_

/**********************************************************
// 通用的Index模块
// 要求DOC_TYPE中必须有 int word_offset, float weight
// 要求INV_TYPE中必须有 int doc_id;
// 要求KEY_TYPE中必须有 int word_offset, int begin, int end
***********************************************************/

#include <string>
#include <vector>
#include <map>

#include "HashVector_Enhanced.h"

//默认的Doc结构
struct DEFAULT_DOC_TYPE {
	int word_offset; //doc在word buf中的偏移
	float weight;   //这个doc的静态全中

	DEFAULT_DOC_TYPE & operator = (const DEFAULT_DOC_TYPE &m) {
		word_offset = m.word_offset;
		weight = m.weight;
		return *this;
	}
};

//默认的倒排结构
struct DEFAULT_INV_TYPE {
	int doc_id;    //这个倒排项所指的doc id

	DEFAULT_INV_TYPE & operator = (const DEFAULT_INV_TYPE &m) {
		doc_id = m.doc_id;
		return *this;
	}
};

//默认的Key结构
struct DEFAULT_KEY_TYPE {
	int word_offset;  //key word在word buf中的偏移 
	int begin; //在倒排表中的开头 左闭
	int end;   //在倒排表中的结尾 右开


	DEFAULT_KEY_TYPE & operator = (const DEFAULT_KEY_TYPE &m) {
		word_offset = word_offset;
		begin = m.begin;
		end = m.end;
		return *this;
	}
};

template <typename DOC_TYPE = DEFAULT_DOC_TYPE, typename INV_TYPE = DEFAULT_INV_TYPE, typename KEY_TYPE = DEFAULT_KEY_TYPE>
class CommonIndex {

public :

	//CommonIndex();
	//~CommonIndex();

	//INV_TYPE *GetInv(const char *key_word, int &num_inv);
	//DOC_TYPE *GetDoc(INV_TYPE *inv_node);
	//const char *GetDocText(DOC_TYPE *doc_node);
	//int Load(FILE *fp_index);
	//int Clear();
	//int Add(const char *key_word, INV_TYPE &inv_node, DOC_TYPE &doc_node, const char *doc_text);
	//int Add(std::vector<std::string> &key_vect, INV_TYPE &inv_node, DOC_TYPE &doc_node, const char *doc_text);
	//int Add(std::vector<std::string> &key_vect, std::vector<INV_TYPE> &inv_vect, DOC_TYPE &doc_node, const char *doc_text);

	//int Save(FILE *fp_index);
	
public:
	typedef std::vector<DOC_TYPE> DOC_VECT;
	typedef std::vector<INV_TYPE> INV_VECT;
	typedef std::map<std::string,  INV_VECT> INDEX_MAP;
	typedef CHashVector_Enhanced< KEY_TYPE> INDEX;
	

private:

	//int ChangeToHVIndex(bool is_need_sort = false);
	//int QSort(INV_VECT &vect, int left, int right);
	//int Partition(INV_VECT &vect, int left, int right);
	//void Wap(INV_TYPE &node1, INV_TYPE &node2);

	//真正使用的二进制数据格式
	char *m_char_buf; 
	int m_num_char; 
	DOC_TYPE *m_doc_buf; 
	int m_num_doc;
	INV_TYPE *m_inv_buf;
	int m_num_inv;
	INDEX m_index;

	//Make index 使用的结构
	std::vector<char> m_char_vect;
	DOC_VECT m_doc_vect;
	INV_VECT m_inv_vect;
	INDEX_MAP m_index_map;

public: 

	CommonIndex() {
	
		m_char_buf = NULL;
		m_num_char = 0;
		m_doc_buf = NULL;
		m_num_doc = 0;
		m_inv_buf = NULL;
		m_num_inv = 0;
	}

	~CommonIndex() {
		
		Clear();
	}

	//根据key， 获得相应的倒排链
	INV_TYPE *GetInv(const char *key_word, int &num_inv){

		KEY_TYPE *key_node = m_index.Find(key_word);
		if (NULL == key_node) {
			return NULL;
		}

		INV_TYPE *inv_list = m_inv_buf + key_node->begin;
		num_inv = key_node->end - key_node->begin;

		return inv_list;
	}

	//根据doc_id， 获取的doc信息
	DOC_TYPE *GetDoc(INV_TYPE *inv_node) {

		return m_doc_buf + inv_node->doc_id;
	}

	//获取doc node的text
	const char *GetDocText(DOC_TYPE *doc_node) {

		return m_char_buf + doc_node->word_offset;
	}

	//增加一个新的索引项
	int Add(const char *key_word, INV_TYPE &inv_node, DOC_TYPE &doc_node, const char *doc_text) {

		doc_node.word_offset = m_char_vect.size();
		const char *p = doc_text;
		while (*p) {
			m_char_vect.push_back(*p++);
		}
		m_char_vect.push_back('\0');

		inv_node.doc_id = m_doc_vect.size();
		m_doc_vect.push_back(doc_node);
		m_index_map[key_word].push_back(inv_node);
		
		return 0;
	}

	//第一个doc， 增加多个key信息， 但倒排项都是一样的内容
	int Add(std::vector<std::string> &key_vect, INV_TYPE &inv_node, DOC_TYPE &doc_node, const char *doc_text) {

		doc_node.word_offset = m_char_vect.size();
		const char *p = doc_text;
		while (*p) {
			m_char_vect.push_back(*p++);
		}
		m_char_vect.push_back('\0');

		inv_node.doc_id = m_doc_vect.size();
		m_doc_vect.push_back(doc_node);
		for (std::vector<std::string>::iterator itr = key_vect.begin(); itr != key_vect.end(); ++itr) {
			m_index_map[itr->c_str()].push_back(inv_node);
		}
		
		return 0;
	}

	//增加一个doc， 但是有多个key和相应的倒排与之想对应
	int Add(std::vector<std::string> &key_vect, std::vector<INV_TYPE> &inv_vect, DOC_TYPE &doc_node, const char *doc_text) {

		doc_node.word_offset = m_char_vect.size();
		const char *p = doc_text;
		while (*p) {
			m_char_vect.push_back(*p++);
		}
		m_char_vect.push_back('\0');

		int doc_id = m_doc_vect.size();
		m_doc_vect.push_back(doc_node);

		typename INV_VECT::iterator itr_inv = inv_vect.begin();
		for (std::vector<std::string>::iterator itr = key_vect.begin(); itr != key_vect.end(); ++itr) {
			INV_TYPE inv_node = *itr_inv;
			inv_node.doc_id = doc_id;
			m_index_map[itr->c_str()].push_back(inv_node);
			++itr_inv;
		}
		
		return 0;
	}

	//必要的时候对倒排进行排序
	int ChangeToHVIndex(bool is_need_sort) {

		for (typename INDEX_MAP::iterator itr = m_index_map.begin(); itr != m_index_map.end(); ++itr) {
			std::string key = itr->first;
			INV_VECT &inv_vect = itr->second;

			if (is_need_sort) {
				QSort(inv_vect, 0, inv_vect.size()-1);
			}

			KEY_TYPE indexNode;
			indexNode.begin = m_inv_vect.size();
			m_inv_vect.insert(m_inv_vect.end(), inv_vect.begin(), inv_vect.end());
			indexNode.end = m_inv_vect.size();

			m_index.AddItem(indexNode, key.c_str());
		    
		}

		//对key进行排序
		m_index.Sort();


		return 0;
	}

	//对倒排进行快速排序
	int QSort(INV_VECT &vect, int left, int right) {
		if (left < right) {
			int key = Partition(vect, left, right);
			QSort(vect, left, key-1);
			QSort(vect, key+1, right);
		}

		return 0;
	}

	//快排过程
	int Partition(INV_VECT &vect, int left, int right) {
		
		//处理只有两个元素的特殊情况
		if (right == left+1) {
			if (m_doc_vect[vect[right].doc_id].weight > m_doc_vect[vect[left].doc_id].weight) {
				Wap(vect[left], vect[right]);
				return right;
			}
		}
		
		int key = left;
		float key_value = m_doc_vect[vect[key].doc_id].weight;
		left++;
		while (left < right) {
			while (left < right && m_doc_vect[vect[left].doc_id].weight >= key_value) {
				left++;
			}
			while (left <= right && right > key && m_doc_vect[vect[right].doc_id].weight < key_value) {
				right--;
			}
		  if (left < right) {
				Wap(vect[left], vect[right]);
				left++;
				right--;
			}
		}

		//处理right == left的情况
		if (right == left) {
			if (m_doc_vect[vect[right].doc_id].weight < key_value) {
				right--;
			}
		}

		if (right == key) {
			return right;
		}

		Wap(vect[key], vect[right]);

		return right;
	}

	//交换
	void Wap(INV_TYPE &node1, INV_TYPE &node2) {

		INV_TYPE node = node1;
		node1 = node2;
		node2 = node;
	}

	//保存索引
	int Save(FILE *fp_index, bool is_need_sort = false) {
		
		if (ChangeToHVIndex(is_need_sort) != 0) {
			return -1;
		}

		m_num_char = m_char_vect.size();
		if (fwrite(&m_num_char, sizeof(m_num_char), 1, fp_index) != 1) {
			return -1;
		}
		if (fwrite(&(m_char_vect[0]), sizeof(m_char_vect[0]), m_num_char, fp_index) != unsigned(m_num_char)) {
			return -1;
		}

		m_num_doc = m_doc_vect.size();
		if (fwrite(&m_num_doc, sizeof(m_num_doc), 1, fp_index) != 1) {
			return -1;
		}
		if (fwrite(&(m_doc_vect[0]), sizeof(m_doc_vect[0]), m_num_doc, fp_index) != unsigned(m_num_doc)) {
			return -1;
		}

		m_num_inv = m_inv_vect.size();
		if (fwrite(&m_num_inv, sizeof(m_num_inv), 1, fp_index) != 1) {
			return -1;
		}
		if (fwrite(&(m_inv_vect[0]), sizeof(m_inv_vect[0]), m_num_inv, fp_index) != unsigned(m_num_inv)) {
			return -1;
		}

		m_index.WriteFile(fp_index);
		
		return 0;

	}

	//读入索引
	int Load(FILE *fp_index) {

		Clear();

		if (fread(&m_num_char, sizeof(m_num_char), 1, fp_index) != 1) {
			fprintf(stderr, "Load m_num_char error!\n");
			return -1;
		}
		m_char_buf = new char[m_num_char];
		if (NULL == m_char_buf) {
			return -1;
		}
		if (fread(m_char_buf, sizeof(char), m_num_char, fp_index) != unsigned(m_num_char)) {
			fprintf(stderr, "Load m_char_buf error!\n");
			return -1;
		}
		
		if (fread(&m_num_doc, sizeof(m_num_doc), 1, fp_index) != 1) {
			fprintf(stderr, "Load m_num_doc error!\n");
			return -1;
		}
		m_doc_buf = new DOC_TYPE[m_num_doc];
		if (NULL == m_doc_buf) {
			return -1;
		}
		if (fread(m_doc_buf, sizeof(DOC_TYPE), m_num_doc, fp_index) != unsigned(m_num_doc)) {
			fprintf(stderr, "Load m_num_doc error!\n");
			return -1;
		}
		
		if (fread(&m_num_inv, sizeof(m_num_inv), 1, fp_index) != 1) {
			fprintf(stderr, "Load m_num_inv error!\n");
			return -1;
		}
		m_inv_buf = new INV_TYPE[m_num_inv];
		if (NULL == m_inv_buf) {
			return -1;
		}
		if (fread(m_inv_buf, sizeof(INV_TYPE), m_num_inv, fp_index) != unsigned(m_num_inv)) {
			fprintf(stderr, "Load m_inv_buf error!\n");
			return -1;
		}
		
		if (m_index.ReadInFile(fp_index) == false) {
			fprintf(stderr, "Load index error!\n");
			return -1;
		}

		return 0;
	}

	//清空索引
	int Clear() {

		if (NULL != m_char_buf) {
			delete[] m_char_buf;
			m_char_buf = NULL;
		}
		m_num_char = 0;

		if (NULL != m_doc_buf) {
			delete[] m_doc_buf;
			m_doc_buf = NULL;
		}
		m_num_doc = 0;

		if (NULL != m_inv_buf) {
			delete[] m_inv_buf;
			m_inv_buf = NULL;
		}
		m_num_inv = 0;

		m_index.Clear();

		m_char_vect.clear();
		m_doc_vect.clear();
		m_inv_vect.clear();
		m_index_map.clear();
		return 0;
	}


};


#endif


