/*************************************************
 * @yonghuahu
*************************************************/

#include "doc_index.h"

using namespace std;
using namespace spaceGary;
using namespace WHITE_BLACK_DA;

//const string DA_INDEX_FILE = "/data/tieba/pair.out.post.norm.daIndex";
const string DA_INDEX_FILE = "/index_dir/da.Index";

/*
CommonIndex<DLG_DOC_TYPE,DLG_INV_TYPE,DLG_KEY_TYPE> CDlgIndex::m_doc_index;
set<string> CDlgIndex::m_stopword;
set<string> CDlgIndex::m_postfix_word;
set<string> CDlgIndex::m_black;
*/
/*
CommonIndex<DLG_DOC_TYPE,DLG_INV_TYPE,DLG_KEY_TYPE> m_doc_index;
set<string> m_stopword;
set<string> m_postfix_word;
set<string> m_black;
*/

CDlgIndex::CDlgIndex():
    m_graph_segment(65536)
{
	EncodingConvertor::initializeInstance(); 
    analysis::DicTree::instance()->loadDic();
    m_graph_segment.open(analysis::DicTree::instance());
}

CDlgIndex::~CDlgIndex()
{
	Release();
}

int32_t CDlgIndex::Init(const char* dir_base)
{
	if(Norm.Init() == -1)
	{
		return -1;
	}
	return 0;
}

int32_t CDlgIndex::Release()
{
	CDlgIndex::Release_();
}

int32_t CDlgIndex::Release_()
{
}

int32_t CDlgIndex::LoadStopword(
		const char* file_stopword)
{
	FILE* fp_stopword = fopen(file_stopword,"r");
	if(NULL == fp_stopword)
	{
		_ERROR("open file [%s] error!",file_stopword);
		return -1;
	}

	char line[1024];
	while(fgets(line,1024,fp_stopword))
	{
		string line_str = line;
		StringTrim(line_str);

		//半角转全角
		char dest[1024];
		memset(dest,0x00,1024);
		EncodingConvertor::getInstance()->dbc2gchar(
				line_str.c_str(),
				(gchar_t*)dest,
				1024/2,
				true);
		
		m_stopword.insert(string(dest));
		
	}
	
	return 0;
}

int32_t CDlgIndex::LoadPostfixWord(
		const char* file_postfix)
{
	FILE* fp_postfix = fopen(file_postfix,"r");
	if(NULL == fp_postfix)
	{
		_ERROR("open file [%s] error!",file_postfix);
		return -1;
	}

	char line[1024];
	while(fgets(line,1024,fp_postfix))
	{
		string line_str = line;
		StringTrim(line_str);

		vector<string> parts;
		StringSplit(line_str,parts,"\t");
		
		m_postfix_word.insert(parts[0]);
	}
	
	return 0;
}

int32_t CDlgIndex::LoadBlackword(
		const char* file_black)
{
	FILE* fp_black = fopen(file_black,"r");
	if(NULL == fp_black)
	{
		_ERROR("open file [%s] error!",file_black);
		return -1;
	}

	char line[1024];
	while(fgets(line,1024,fp_black))
	{
		string line_str = line;
		StringTrim(line_str);

		//半角转全角
		char dest[1024];
		memset(dest,0x00,1024);
		EncodingConvertor::getInstance()->dbc2gchar(
				line_str.c_str(),
				(gchar_t*)dest,
				1024/2,
				true);
		
		m_black.insert(string(dest));
		
	}
	
	return 0;
}


int32_t CDlgIndex::MakeIndex(
		const char* file_in,
		const char* file_out)
{
	FILE* fp_in = fopen(file_in,"r");
	if(NULL == fp_in)
	{
		_ERROR("open file [%s] error!",file_in);
		return -1;
	}
	
	char line[4096];
	int32_t line_num = 0;
	SEGMENT_1 seg;
	seg.Init();
	while(fgets(line,4096,fp_in))
	{
		line_num ++;
		if(line_num % 100000 == 0)
		{
			_INFO("read line [%d]",line_num);
		}
		//_INFO("-------------------line:[%d]", line_num);
		string line_str = line;
		StringTrim(line_str);
		vector<string> parts;
		StringSplit(line_str,parts,"##_##");
		if(parts.size() == 0)
		{
			_INFO("[NOTE] line [%d][%s] format error!",
					line_num,line_str.c_str());
		}
		string destString;
		if(parts.size() > 1)
		{
			destString = parts[0];
		}else
			destString = line_str;
		if(destString.length() < 2)
			continue;
		vector<string> seg_parts;
		//cerr << "destString:" << destString << endl;
		//GraphSegment(graph,string(dest),seg_parts);
		seg.Segment_(destString,seg_parts);

		string doc_name = "";//处理过的doc name

		vector<DLG_INV_TYPE> vec_inv; 						// INV_TYPE 向量
		vector<string> vec_key;								// key 向量
		for(size_t i = 0; i < seg_parts.size(); i++)		// 要建立索引的query 分词结果
		{
			//cerr << "seg_parts["<<i<< "]" << seg_parts[i] << endl;
			DLG_INV_TYPE inv;
			inv.occ = 0;						// 词在doc中的默认位置
			inv.occ_len = seg_parts[i].size();	// 当前词的长度
			inv.occ_filed = 0;					// 词在query中的域，比如括号内、括号外

			vec_key.push_back(seg_parts[i]);				// 词向量
			vec_inv.push_back(inv);							// 倒排节点
															// 以上两者一一对应

			doc_name += seg_parts[i];
		}

		//由于9个字段全用上的话数据量太大索引做不出来，
		//所以这里只使用了doc_name和data字段检索
		line_str = doc_name;

		DLG_DOC_TYPE doc;									// DOC_TYPE 
		doc.weight = 0; //默认doc权重都为0		

		if(vec_key.size() > 0)
			m_doc_index.Add(vec_key,vec_inv,doc,line_str.c_str());// 将key向量、倒排向量、doc、query插入
																  // 【将line_str添加到m_char_vect中】：doc的word_offset = m_char_vect.size(),也就是当前词已有的索引内容的下一个位置处，将该doc内容加入到m_char_vect中;注意最后要加上 \0 作为结尾；
																  // 【将doc添加到m_doc_vect中】：doc的doc_id = m_doc_vect.size(),也就是当前已存在的所有doc内容的下一个位置;
																  // 【构建索引】：对于上述一一对应的key和倒排节点，放入同一个倒排链 INV_TYPE 中，并设置倒排节点的doc_id；最终将key以及对应的倒排链结构体，存入m_index_map中；
																  // 数据类型：map<string, vector<INV_TYPE>> m_index_map

	}

	FILE* fp_out = fopen(file_out,"w");
	if(fp_out == NULL)
	{
		_ERROR("open file [%s] error!",file_out);
		return -1;
	}
	if(m_doc_index.Save(fp_out) < 0)						// 保存索引
	{
		_ERROR("save index [%s] error!",file_out);
		return -1;
	}

	fclose(fp_in);
	fclose(fp_out);

	return 0;
}

int32_t CDlgIndex::LoadIndex(
		const char* file_index)
{
	_INFO("begin to load index ...");
	_INFO("index file: %s", file_index);
	FILE* fp_index = fopen(file_index,"r");					// 打开索引文件
	if(fp_index == NULL)
	{
		_ERROR("open file [%s] error!",file_index);
		return -1;
	}

	if(m_doc_index.Load(fp_index) < 0)						// 读入索引
															// m_num_char
															// m_char_buf:存放key
															// m_num_doc
															// m_doc_buf:存放doc
															// m_num_inv
															// m_inv_buf:存放倒排节点
															// m_index
	{
		_ERROR("load index [%s] error!",file_index);
		return -1;
	}

	_INFO("load doc index ok!");
	fclose(fp_index);
	return 0;
}


int32_t CDlgIndex::GraphSegment(
		analysis::TGraph& graph,
		const string& query,
		std::vector<std::string>& parts)
{
	parts.clear();
    char dest[10240];
    char buf[4096];
    bchar_t zero[1] = {L'\0'};
    if(query.length()<2) //长度过短
		return 0;

    analysis::ResultInfo info[100];
	StringUtils::gbk_utf16le(
			strcpy(buf,query.c_str()), 
			query.length(), 
			dest);
    int r = graph.graphSegment(
			(bchar_t*)dest,
			bcscspn((bchar_t*)dest,zero),
			info,
			100);

    for (int i = 0; i < r; i++) 
	{
		StringUtils::utf16le_gbk(
				dest+info[i].pos*2, 
				info[i].tlen*2, 
				buf);
		string word = buf;
		if(m_stopword.find(word) == m_stopword.end())
		{
			parts.push_back(string(buf));
		}
    }
	return 0;
}

// main
int32_t CDlgIndex::GetDlgResult(
		const string& query,
		DlgResult& doc_result, 
		SEGMENT_1* seg)
{
	//判断query长度
	if(query.size() <= 1 || query.size() > 100)
	{
		_INFO("query [%s] too short or too long!",
				query.c_str());
		return -1;
	}

	//过query黑名单
	for(set<string>::iterator it = m_black.begin();
			it != m_black.end();
			++ it)
	{
		if(query.find(*it) != string::npos)
		{
			_INFO("query [%s] hit blacklist",query.c_str());
			return -1;
		}
	}

	vector<string> seg_parts;
	vector<string> docs;
	//vector<string> anwser;
	string query_norm;
	string seg_norm;
	Norm.Normalize_(query, seg_norm, query_norm,seg);
	cerr << "[INFO] query_norm: " << query_norm << endl;
	cerr << "[INFO]   seg_norm: " << seg_norm << endl;
	//倒排结果过滤
	if(GetUnionResults(seg_norm, docs, 
				//anwser, 
				seg_parts) > 500)
	{
	}
	if(docs.size() < 1)
	{//doc数目为0
		cerr << "[NOTE] There is no docs found." << endl;
		return -1;
	}
	doc_result.data = docs;
	//doc_result.anwser = anwser;
	doc_result.query = seg_norm;
	
/*	if(query.size() > 12)
	{//query长度大于6个字
		if(GetDlgResultLongQuery(
					query,
					docs,
					doc_result) == 0)
		{
			//_DEBUG("get result long!");
			return 0;
		}
	}

	if(GetDlgResultPostfix(
				query,
				docs,
				seg_parts,
				doc_result) == 0)
	{//通过后缀确定是否是地图需求
		//_DEBUG("get result postfix!");
		return 0;
	}
*/
	return 0;
}
int32_t CDlgIndex::GetDlgResultLongQuery(
		const std::string& query,
		const std::vector<std::string>& docs,
		DlgResult& doc_result)
{
	bool has_result = false;
	if(docs.size() > 0 && docs.size() < 10)
	{
		for(size_t i = 0; i < docs.size(); ++i)
		{
			string doc_text = docs[i];
			vector<string> doc_parts;
			StringSplit(doc_text,doc_parts,"\t");
			if(doc_parts.size() != 2)
			{
				_ERROR("doc error!,doc=[%s]",doc_text.c_str());
				continue;
			}

			size_t pos = 0;
			pos = doc_parts[0].find(query);
			if( pos == 0)
			{//为前缀，判断后缀词
			 //后缀词需为地图特征词  //存在误判，先取消此策略,comment by lsk
			 	continue;
		//	 	string last = doc_parts[0].substr(pos+query.size());
		//		if(m_postfix_word.find(last) != m_postfix_word.end())
		//		{
		//			doc_result.query = query;
		//			//doc_result.data = doc_parts[1];
		//			doc_result.data.push_back(doc_parts[1]);
		//			has_result = true;
		//		}
		//	 	
			}
			if( pos + query.size() == doc_parts[0].size())
			{//为后缀，判断前缀词
			 //前缀词需为地域
			 	string begin = doc_parts[0].substr(0,pos);
				if(doc_parts[1].find(begin) != string::npos)
				{
					doc_result.query = query;
					//doc_result.data = doc_parts[1];
					doc_result.data.push_back(doc_parts[1]);
					has_result = true;
				}
			}
		}
	}

	if(has_result)
		return 0;
	return -1;
}

int32_t CDlgIndex::GetDlgResultPostfix(
		const std::string& query,
		const std::vector<std::string>& docs,
		const std::vector<std::string>& seg_parts,
		DlgResult& doc_result)
{
	map<string,int> data_distribution;
	bool has_result = false;


	//先判断是否命中后缀词
	//要求分词term数必须大于两个
	bool flag = false;
	if(m_postfix_word.find(
				seg_parts[seg_parts.size()-1]) != 
			m_postfix_word.end() ||
		m_postfix_word.find(
			seg_parts[seg_parts.size()-2] + 
			seg_parts[seg_parts.size()-1]) != 
			m_postfix_word.end() )
	{
		flag = true;
	}

	for(size_t i = 0; i < docs.size(); ++i)
	{
		string doc_text = docs[i];
		vector<string> doc_parts;
		StringSplit(doc_text,doc_parts,"\t");
		if(doc_parts.size() != 2)
		{
			_ERROR("doc error!,doc=[%s]",doc_text.c_str());
			continue;
		}

		string data = doc_parts[1];

		if(doc_parts[0] == query && flag)
		{//需要地域判断
			map<string,int>::iterator it_data =
				data_distribution.find(data);
			if(it_data != data_distribution.end())
			{
				it_data->second += 1;
			}
			else
			{
				data_distribution[data] = 1;
			}
		}

	}

	//目前严格控制在分布的城市小于5个以内
	if(data_distribution.size() > 0 && 
			data_distribution.size() < 5)
	{
		doc_result.query = query;
		//string cities = "";
		for(map<string,int>::iterator it = data_distribution.begin();
				it != data_distribution.end();
				++it)
		{
			//cities += it->first;
			doc_result.data.push_back(it->first);
		}

		//doc_result.data = cities;
		has_result = true;
	}

	if(has_result)
		return 0;
	return -1;
}

//得到求交结果，传进来的必须是全角字符
int32_t CDlgIndex::GetUnionResults(
		const string& query,
		vector<DLG_DOC_TYPE*>& docs,
		vector<string>& parts,
		string& query_reform)
{
	GraphSegment(m_graph_segment,
			query,
			parts);

	// show segment
	cerr << "[SEGMENT]: "<< endl;
	if(parts.size() < 1)
	{
		cerr <<"[NOTE] segment size lt 1" << endl;
		return 0;
	}

	query_reform = "";
	for(int i = 0; i < parts.size(); i++)
	{
		query_reform += parts[i];
		cerr << "\tword: " << parts[i] << endl;
	}
	//得到每个term对应的倒排链
	InvNumInfo num_info;
	vector<InvResult> inv_results;	// 存放每个词term 的倒排链的指针
	GetInvResults(parts,num_info,inv_results);		// 返回所有词term的倒排链指针到inv_results,里面包含了每个倒排链的起始位置、长度；
	// show Inverse Results
	cerr << "[INV RESULT]:"<< endl;
	if(parts.size() < 1)
	{
		cerr <<"[NOTE] inv result size lt 1" << endl;
		return 0;
	}

	for(int i = 0; i < inv_results.size(); i++)
	{
		cerr << "\t[inv]: "<< inv_results[i].inv <<"\tinv_num:" << inv_results[i].inv_num << endl;
	}
	//求交
	UnionInvResult(inv_results,num_info,docs);		// 根据倒排索引到的docs 返回最终需要的doc
	{//求交有结果
		if(docs.size() > 0)
		{
			//cout << "[docs info]:" << endl;
			for(int i = 0; i < docs.size(); i++)
			{
				//cout << "\t[onInvResultdocs]:" << docs[i]->weight << endl;
			} 
		}
	}

	return 0;
}

int abs_(int32_t a, int32_t b)
{
	return (a-b) > 0 ? (a-b):(b-a);
}

int32_t CDlgIndex::GetUnionResults(
		const string& query,
		vector<string>& docs_text,
		//vector<string>& ans_text,
		vector<string>& seg_parts)
{
	vector<DLG_DOC_TYPE*> docs;
	string query_reform;
	GetUnionResults(query,docs,seg_parts,query_reform);
	string dest_ = "";
	int32_t length = 10000;
	int32_t sim;
	for(size_t i = 0; i < docs.size(); ++i)
	{
		docs_text.push_back(string(m_doc_index.GetDocText(docs[i])));
		string cadidate = string(m_doc_index.GetDocText(docs[i]));
		if(( sim = abs_(query.length(), cadidate.length())) < length)
		{
			length = sim;
			dest_ = cadidate;
		}
	}
	return docs_text.size();
}

int32_t CDlgIndex::GetInvResults(
		const vector<string>& parts,
		InvNumInfo& num_info,
		vector<InvResult>& inv_results
		)
{
	for(size_t i = 0; i < parts.size(); ++i)
	{
		InvResult result;
		result.inv = 
			m_doc_index.GetInv(						// 获取词term 的倒排doc，返回倒排链的指针
					parts[i].c_str(),
					result.inv_num);
		if(result.inv_num > num_info.max_inv_size)
		{
			num_info.max_inv_size = result.inv_num;
			num_info.max_index = inv_results.size();
		}

		if(result.inv_num < num_info.min_inv_size)
		{
			num_info.min_inv_size = result.inv_num;
			num_info.min_index = inv_results.size();
		}

		inv_results.push_back(result);
		
	}
	return 0;
}


int32_t CDlgIndex::UnionInvResult(
		const vector<InvResult>& inv_results,
		const InvNumInfo& num_info,
		vector<DLG_DOC_TYPE*>& docs)
{
	if(num_info.min_inv_size == 0)
	{//存在任何一个词没有倒排，则跳出。目的是要求query 中所有词都存在倒排链。
		// 所有词都必须在数据中出现过。
		//
		//return 0;
	}

	vector<int> index_begins(inv_results.size(),0);
	//每个doc都会对应一个INV_TYPE的数组，因为每个
	//term都对应一个INV_TYPE
	vector<vector<DLG_INV_TYPE*> > doc_inv_vector;	// 存储每个词term的倒排链,构成倒排矩阵；
	for(int i = 0; 
			i < inv_results[num_info.min_index].inv_num;
			++i)
	{
		DLG_INV_TYPE* inv = 						// 对于每个词term的每个倒排节点
			inv_results[num_info.min_index].inv + i;

		vector<DLG_INV_TYPE*> inv_vector(inv_results.size(),NULL);
		if(SearchDoc(
					inv->doc_id,
					inv_results,
					num_info.min_index,
					index_begins,
					inv_vector) >= 0
				)
		{//找到doc
			docs.push_back(m_doc_index.GetDoc(inv));
			inv_vector[num_info.min_index] = inv;
			doc_inv_vector.push_back(inv_vector);
		}
		else
		{//未找到包含该term的doc
			continue;
		}
	}

	return docs.size();

}

int32_t CDlgIndex::SearchDoc(
		const int doc_id,
		const vector<InvResult>& inv_results,
		const int min_index,
		vector<int>& index_begins,
		vector<DLG_INV_TYPE*>& inv_vector)
{
	for(size_t i = 0; i < inv_results.size(); ++i)
	{
		if(i == min_index)
		{//自身不查找
			continue;
		}

		int doc_index = 0;
		if( (doc_index = BinarySearch(		// 二分查找
					doc_id,
					inv_results[i].inv,
					index_begins[i],
					inv_results[i].inv_num) ) >= 0
				)
		{//下次查找从当前索引的下一个位置开始
			index_begins[i] = doc_index + 1;
			inv_vector[i] = inv_results[i].inv + doc_index;
		}
		else
		{// 有一个term 未找到
			//return -1;
		}

	}
	return 0;
}

int32_t CDlgIndex::BinarySearch(
		const int doc_id,
		DLG_INV_TYPE* inv,
		int offset,
		int max_pos)
{
	int left = offset;
	int right = max_pos-1; //因为倒排的区间右边是开区间
	while(left <= right)
	{
		int mid = left + (right-left)/2;
		if( (inv+mid)->doc_id == doc_id)
		{//找到
			return mid;
		}
		else if((inv+mid)->doc_id < doc_id)
		{
			left = mid + 1;
		}
		else
		{
			right = mid - 1;
		}
	}

	return -1;
}

