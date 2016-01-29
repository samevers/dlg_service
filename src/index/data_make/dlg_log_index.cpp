/*************************************************
 *
*************************************************/

#include "dlg_log_index.h"


using namespace std;
using namespace spaceGary;

CommonIndex<DLG_DOC_TYPE,DLG_INV_TYPE,DLG_KEY_TYPE> CDlgLogIndex::m_dlg_index;

CDlgLogIndex::CDlgLogIndex():
    m_graph_segment(65536)
{
    m_graph_segment.open(analysis::DicTree::instance());
}

CDlgLogIndex::~CDlgLogIndex()
{
}


int32_t CDlgLogIndex::Release()
{
	EncodingConvertor::clear();
	EncodingRecognizer::clear();
	return 0;
}

int32_t CDlgLogIndex::Init()
{
	EncodingConvertor::initializeInstance(); 
    analysis::DicTree::instance()->loadDic();

	//初始化编码识别模块
	EncodingRecognizer::initializeInstance();

	return 0;
}

int32_t CDlgLogIndex::MakeIndex(
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

    analysis::TGraph graph(65536);
    graph.open(analysis::DicTree::instance());
	while(fgets(line,4096,fp_in))
	{
		line_num ++;
		if(line_num % 100000 == 0)
		{
			_INFO("read line [%d]",line_num);
		}
		string line_str = line;
		StringTrim(line_str);
		vector<string> parts;
		StringSplit(line_str,parts,"\t");
		if(parts.size() != 4)
		{
			_INFO("line [%d][%s] format error!",
					line_num,line_str.c_str());
			continue;
		}

		//device + user_area +
		//dlg_name + search_num 
		//
		//半角转全角
		char dest[4096];
		memset(dest,0x00,4096);
		if(EncodingConvertor::getInstance()->dbc2gchar(
				parts[2].c_str(),
				(gchar_t*)dest,
				4096/2,
				true) < 0)
		{
			_INFO("line error! [%d][%s]",line_num,line_str.c_str());
			continue;	
		}

	//	tEncodingCharset eresult;
	//	if(EncodingRecognizer::getInstance()->verify(
	//				dest,strlen(dest),EE_GBK,&eresult) < 0 ||
	//			eresult.encoding != EE_GBK)
	//	{
	//		_INFO("line encoding error! [%d][%s]",line_num,line_str.c_str());
	//		continue;
	//	}
		
		vector<string> seg_parts;
		GraphSegment(graph,string(dest),seg_parts);

		string dlg_name = "";//处理过的dlg name

		vector<DLG_INV_TYPE> vec_inv; 
		vector<string> vec_key;
		for(size_t i = 0; i < seg_parts.size(); i++)
		{
			DLG_INV_TYPE inv;
			inv.occ = 0;
			inv.occ_len = seg_parts[i].size();
			inv.occ_filed = 0;

			vec_key.push_back(seg_parts[i]);
			vec_inv.push_back(inv);

			dlg_name += seg_parts[i];
		}

		line_str = dlg_name + "\t" + parts[1] + "\t" + parts[0];
		//for(size_t i = 1; i < parts.size(); ++i)
		//{
		//	line_str += "\t" + parts[i];
		//}

		DLG_DOC_TYPE doc;
		doc.weight = atoi(parts[3].c_str()); //使用检索次数作为权重

		if(vec_key.size() > 0)
			m_dlg_index.Add(vec_key,vec_inv,doc,line_str.c_str());

	}

	FILE* fp_out = fopen(file_out,"w");
	if(fp_out == NULL)
	{
		_ERROR("open file [%s] error!",file_out);
		return -1;
	}
	if(m_dlg_index.Save(fp_out) < 0)
	{
		_ERROR("save index [%s] error!",file_out);
		return -1;
	}

	fclose(fp_in);
	fclose(fp_out);

	return 0;
}

int32_t CDlgLogIndex::LoadIndex(
		const char* file_index)
{
	_INFO("begin to load dlg log index ...");
	FILE* fp_index = fopen(file_index,"r");
	if(fp_index == NULL)
	{
		_ERROR("open file [%s] error!",file_index);
		return -1;
	}

	if(m_dlg_index.Load(fp_index) < 0)
	{
		_ERROR("load index [%s] error!",file_index);
		return -1;
	}

	_INFO("load dlg log index ok!");
	fclose(fp_index);
	return 0;
}


int32_t CDlgLogIndex::GraphSegment(
		analysis::TGraph& graph,
		const string& query,
		std::vector<std::string>& parts)
{
	parts.clear();
    char dest[10240];
    char buf[4096];
    bchar_t zero[1] = {L'\0'};
    if(query.length() < 2 || query.length() > 100) //长度过短或者过长
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
	//	if(m_stopword.find(word) == m_stopword.end())
	//	{
			parts.push_back(string(buf));
		//}
    }
	return 0;
}

//int32_t CDlgLogIndex::GetDlgResult(
//		const string& query,
//		DlgResult& dlg_result)
//{
//	//判断query长度
//	_INFO("query.size() = %d",query.size());
//	if(query.size() <= 6 || query.size() > 100)
//	{
//		_INFO("query [%s] too short or too long!",
//				query.c_str());
//		return -1;
//	}
//
//	//过query黑名单
//	//for(set<string>::iterator it = m_black.begin();
//	//		it != m_black.end();
//	//		++ it)
//	//{
//	//	if(query.find(*it) != string::npos)
//	//	{
//	//		_INFO("query [%s] hit blacklist",query.c_str());
//	//		return -1;
//	//	}
//	//}
//
//	vector<string> seg_parts;
//	vector<string> docs;
//	//倒排结果过滤
//	if(GetUnionResults(query,docs,seg_parts) > 500)
//	{//doc数目太多则不判断，后续需求
//		return -1;
//	}
//	if(seg_parts.size() <= 2 || docs.size() < 1)
//	{//分词term数太小,或者doc数目为0
//		return -1;
//	}
//
//	if(query.size() > 12)
//	{//query长度大于6个字
//		if(GetDlgResultLongQuery(
//					query,
//					docs,
//					dlg_result) == 0)
//			return 0;
//	}
//
//	if(GetDlgResultPostfix(
//				query,
//				docs,
//				seg_parts,
//				dlg_result) == 0)
//	{//通过后缀确定是否是地图需求
//		return 0;
//	}
//
//	return -1;
//}
//
//int32_t CDlgLogIndex::GetDlgResultLongQuery(
//		const std::string& query,
//		const std::vector<std::string>& docs,
//		DlgResult& dlg_result)
//{
//	bool has_result = false;
//	if(docs.size() > 0 && docs.size() < 10)
//	{
//		for(size_t i = 0; i < docs.size(); ++i)
//		{
//			string doc_text = docs[i];
//			vector<string> doc_parts;
//			StringSplit(doc_text,doc_parts,"\t");
//			if(doc_parts.size() != 2)
//			{
//				_ERROR("doc error!,doc=[%s]",doc_text.c_str());
//				continue;
//			}
//
//			size_t pos = 0;
//			pos = doc_parts[0].find(query);
//			if( pos  == 0)
//			{//为前缀，判断后缀词
//			 //后缀词需为地图特征词
//			 	string last = doc_parts[0].substr(pos+query.size());
//				if(m_postfix_word.find(last) != m_postfix_word.end())
//				{
//					dlg_result.query = query;
//					dlg_result.data = doc_parts[1];
//					has_result = true;
//				}
//			 	
//			}
//			else if( pos + query.size() == doc_parts[0].size())
//			{//为后缀，判断前缀词
//			 //前缀词需为地域
//			 	string begin = doc_parts[0].substr(0,pos);
//				if(doc_parts[1].find(begin) != string::npos)
//				{
//					dlg_result.query = query;
//					dlg_result.data = doc_parts[1];
//					has_result = true;
//				}
//			}
//		}
//	}
//
//	if(has_result)
//		return 0;
//	return -1;
//}

//int32_t CDlgLogIndex::GetDlgResultPostfix(
//		const std::string& query,
//		const std::vector<std::string>& docs,
//		const std::vector<std::string>& seg_parts,
//		DlgResult& dlg_result)
//{
//	map<string,int> data_distribution;
//	bool has_result = false;
//
//	//先判断是否命中后缀词
//	//要求分词term数必须大于两个
//	bool flag = false;
//	if(m_postfix_word.find(
//				seg_parts[seg_parts.size()-1]) != 
//			m_postfix_word.end() ||
//		m_postfix_word.find(
//			seg_parts[seg_parts.size()-2] + 
//			seg_parts[seg_parts.size()-1]) != 
//			m_postfix_word.end() )
//	{
//		flag = true;
//	}
//
//	for(size_t i = 0; i < docs.size(); ++i)
//	{
//		string doc_text = docs[i];
//		vector<string> doc_parts;
//		StringSplit(doc_text,doc_parts,"\t");
//		if(doc_parts.size() != 2)
//		{
//			_ERROR("doc error!,doc=[%s]",doc_text.c_str());
//			continue;
//		}
//
//		string data = doc_parts[1];
//
//		if(doc_parts[0] == query && flag)
//		{//需要地域判断
//			map<string,int>::iterator it_data =
//				data_distribution.find(data);
//			if(it_data != data_distribution.end())
//			{
//				it_data->second += 1;
//			}
//			else
//			{
//				data_distribution[data] = 1;
//			}
//		}
//
//	}
//
//	if(data_distribution.size() > 0 && data_distribution.size() < 5)
//	{
//		dlg_result.query = query;
//		string cities = "";
//		for(map<string,int>::iterator it = data_distribution.begin();
//				it != data_distribution.end();
//				++it)
//		{
//			cities += it->first;
//		}
//
//		dlg_result.data = cities;
//
//		has_result = true;
//	}
//
//	if(has_result)
//		return 0;
//	return -1;
//}

//得到求交结果，传进来的必须是全角字符
int32_t CDlgLogIndex::GetUnionResults(
		const string& query,
		vector<DLG_DOC_TYPE*>& docs,
		vector<string>& parts)
{
	//vector<string> parts;
	GraphSegment(m_graph_segment,
			query,
			parts);

	//只有一个term不检索
	if(parts.size() <= 1)
	{
		return 0;
	}
	
	//得到每个term对应的倒排链
	InvNumInfo num_info;
	vector<InvResult> inv_results;
	GetInvResults(parts,num_info,inv_results);

	//求交
	UnionInvResult(inv_results,num_info,docs);
	{//求交有结果
		//for(int i = 0; i < docs.size(); ++i)
		//{
		//	cerr<<m_dlg_index.GetDocText(docs[i])<<endl;
		//}
	}

	return 0;
}

//得到求交结果，传进来的必须是全角字符
int32_t CDlgLogIndex::GetUnionResults(
		const string& query,
		vector<DlgLogResult>& log_docs,
		vector<string>& seg_parts)
{
	vector<DLG_DOC_TYPE*> docs;
	GetUnionResults(query,docs,seg_parts);
	for(size_t i = 0; i < docs.size(); ++i)
	{//求交有结果
		DlgLogResult log_result;
		
		string text = m_dlg_index.GetDocText(docs[i]);
		vector<string> parts;
		spaceGary::StringSplit(text,parts,"\t");
		if(parts.size() != 3)
		{
			_INFO("doc error! %s",text.c_str());
			continue;
		}

		log_result.query = parts[0];
		log_result.area = parts[1];
		log_result.device = parts[2];
		log_result.weight = docs[i]->weight;

		vector<string> area_parts;
		spaceGary::StringSplit(parts[1],area_parts,",");
		if(area_parts.size() == 1)
		{
			log_result.city = area_parts[0];
		}
		else
		{
			log_result.city = area_parts[1];
		}

		if(log_result.city != "unknown" && 
				log_result.city != "null")
		{
			log_docs.push_back(log_result);
		}
	}

	sort(log_docs.begin(),log_docs.end());

	return 0;
}

int32_t CDlgLogIndex::GetInvResults(
		const vector<string>& parts,
		InvNumInfo& num_info,
		vector<InvResult>& inv_results
		)
{
	for(size_t i = 0; i < parts.size(); ++i)
	{
		InvResult result;
		result.inv = 
			m_dlg_index.GetInv(
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
		
		//cerr<<"\n\n\n"<<parts[i]<<"\n\n\n"<<endl;
		//for(int j = 0 ; j < result.inv_num; ++j)
		//{
		//	DLG_DOC_TYPE* doc =
		//		m_dlg_index.GetDoc(result.inv + j);
		//	cerr<<(result.inv+j)->doc_id<<"\t"<<m_dlg_index.GetDocText(doc)<<endl;
		//}
	}
	return 0;
}


int32_t CDlgLogIndex::UnionInvResult(
		const vector<InvResult>& inv_results,
		const InvNumInfo& num_info,
		vector<DLG_DOC_TYPE*>& docs)
{
	if(num_info.min_inv_size == 0)
	{//存在词没有倒排
		return 0;
	}

	vector<int> index_begins(inv_results.size(),0);
	//每个doc都会对应一个INV_TYPE的数组，因为每个
	//term都对应一个INV_TYPE
	vector<vector<DLG_INV_TYPE*> > doc_inv_vector;
	for(int i = 0; 
			i < inv_results[num_info.min_index].inv_num;
			++i)
	{
		DLG_INV_TYPE* inv = 
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
			docs.push_back(m_dlg_index.GetDoc(inv));
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

int32_t CDlgLogIndex::SearchDoc(
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
		if( (doc_index = BinarySearch(
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
		{//未找到
			return -1;
		}

	}
	return 0;
}

int32_t CDlgLogIndex::BinarySearch(
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

