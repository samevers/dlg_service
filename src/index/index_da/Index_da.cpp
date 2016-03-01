#include "Index_da.h"
//typedef unsigned int uint32_t;

using namespace std;

namespace WHITE_BLACK_DA{
const static int MAXSTRLEN=10240;
const static int MAXQUERYLEN=MAXSTRLEN/2;


int32_t DeleteSpace(string & input,
		string & output, char del)
{
	for(size_t i = 0; i < input.size(); i++)
	{
		if(input[i] < 0)
		{
			if(i + 1 >= input.size())
			{
				return -1;
			}
			else
			{
				output.push_back(input[i]);
				output.push_back(input[i+1]);
				i+= 1;
			}
		}
		else
		{
			if(input[i] == del)
			{
			}
			else
			{
				output.push_back(input[i]);
			}
		}
	}
	return 0;
}

WhiteBlackList_::WhiteBlackList_() 
{
	EncodingConvertor::initializeInstance();
	ec = EncodingConvertor::getInstance();
	m_char_buf = NULL;
	m_len_char = 0;
	m_len_struct = 0;
	seg = SEGMENT_1::getInstance();
	seg->Init();
}

WhiteBlackList_::~WhiteBlackList_() 
{

}
// split
int split_sam(vector<string>& vec, string line, string blank)
{
	if(line.length() < 1)
		return 0;
	int start = 0;
	int loc;
	string sub;

	while((loc = line.find(blank.c_str())) != -1)
	{
		//sub = line.substr(start, loc);
		sub = line.substr(0, loc);
		//cerr << "[debug_sam] sub = " << sub << endl;
		vec.push_back(sub);
		start = loc + blank.length();
		line = line.substr(start, line.length() - start);
	}
	vec.push_back(line);
//	cerr << "[debug_sam] sub = " << line << endl;
//	cerr << "[debug_sam] vec.size = " << vec.size() << endl;
	return 1;
}
// make index
int32_t WhiteBlackList_::MakeIndex(const char *file, const char *index_file)
{
	cerr << "[debug_sam] MakeIndex file : " << file << endl;
	FILE * fp_in = fopen(file, "r");
	if (!fp_in) 
	{
		cerr << "can't not open file:" << file;
		return -1;
	}

	vector<WhiteBlackList_TMP> whiteBlackList_vect;
	char line[10240];
	uint32_t reform_size = 0;
	int nn = 0;
	string blank = "_$##$_";
	while (fgets(line, 10240, fp_in)) 
	{
		nn++;
		if(nn % 10000 == 0)
			cerr << "[debug_sam] nn = " << nn << endl;
		string str_line = line;
		vector<string> parts_vect;
		parts_vect.clear();
		//boost::algorithm::split(parts_vect, str_line, boost::is_any_of( "_$##$_"));
		split_sam(parts_vect, str_line, blank.c_str());
		if(parts_vect.size() < 7)
		{
			cerr << nn << "th line"<< endl;
			cerr << "content:	" << line<< endl;
			cerr << "[debug_sam] ERROR : Format of file is error! .........FALSE" <<endl;
			cerr << "Format should be like this:" << endl;
			cerr << "QUERY	VRTYPE	POSITION	REFORM	ITEMQUERY	DATA	ISWAPPC" << endl;
			cerr << "e.g.," << endl;
			cerr << "匆匆那年	video	accu	匆匆那年	匆匆那年	匆匆那年	wap" << endl;
			continue;
			//return -1;
		}
		int32_t num_parts = parts_vect.size();

		WhiteBlackList_TMP node;
		string query = parts_vect[0];				// query
		string vrtype = parts_vect[1];				// vr
		string vrlabel = parts_vect[2];				// label
		string type = parts_vect[3].c_str();		// type
		string reform = parts_vect[4];				// reform
		string item_query = parts_vect[5];			// item query
		string data = parts_vect[6];				// data
		string iswap_pc = parts_vect[7].c_str();				// iswap_pc
		/*trim(query);
		trim(type);
		trim(vrtype);
		trim(reform);
		trim(item_query);
		trim(data);
		trim(iswap_pc);

		cerr << "[debug_sam] query = "<< query << ""<<  endl;
		cerr << "[debug_sam] vr = "<< vrtype << ""<<  endl;
		cerr << "[debug_sam] label = "<< vrlabel << ""<<  endl;
		cerr << "[debug_sam] type = "<< type << ""<<  endl;
		cerr << "[debug_sam] reform = "<< reform << ""<<  endl;
		cerr << "[debug_sam] item query  = "<< item_query << ""<<  endl;
		cerr << "[debug_sam] data = "<< data << ""<<  endl;
		cerr << "[debug_sam] iswap_pc = "<< iswap_pc << ""<<  endl;
	*/
		int q_len = query.length();					// query_len
		if(query.length() < 1 || query.length()> MAXQUERYLEN){continue;}
		char* buffer = NULL;
		string gReform;
		string gItemQuery;
		string gData;
/*
		if(reform != "#_#" && reform.find("###") == -1)
		{
			//change2qj(reform, buffer);
			gReform = buffer;
			delete [] buffer;
		}else
			gReform = reform;

		if(item_query != "#_#"&& item_query.find("###") == -1)
		{
			//change2qj(item_query, buffer);
			gItemQuery = buffer;
			delete [] buffer;
		}else
			gItemQuery = item_query;

		if(data != "#_#"&& data.find("###") == -1)
		{
			//change2qj(data, buffer);
			gData = buffer;
			delete [] buffer;
		}else
			gData = data;
*/
		gReform = reform;
		gData = data;
		gItemQuery = item_query;

		change2qj(query, buffer);
		string gQuery(buffer);
		delete [] buffer;

		//cerr << "[debug_sam] gQuery = "<< gQuery<<  endl;
		node.query = gQuery;					// 赋值给结构体, node.query
		Pos_Reform option_node(vrtype.c_str(),vrlabel.c_str(),  type.c_str(),gReform.c_str(), gItemQuery.c_str(), gData.c_str(), iswap_pc.c_str(), q_len);
		node.vr_pos_reform.push_back(option_node);		// node.vr_pos_reform
	
		if (node.vr_pos_reform.size() > 0) 
		{
			whiteBlackList_vect.push_back(node);			// 最终要进行排序，并进行DA 存储的vector：【whiteBlackList_vect】
		}

	}
	//将数据进行转换并输出到index文件中
	OutputIndexFile(whiteBlackList_vect, index_file);	//	将要进行DA存储的vector输出到索引文件 index_file

	fclose(fp_in);

	return 0;
}
int char_vect_push_(const char* content, vector<char>& char_vect_, uint32_t& end)
{
	const char * p = content;
	if(p == NULL)
		return -1;
	if(p == "")
		return 0;
	while (*p) 
	{
		char_vect_.push_back(*p);
		p++;
		end++;
	}
	char_vect_.push_back('\0');
	end++;

	return 0;
}	
//输出index文件
int32_t WhiteBlackList_::OutputIndexFile(
		std::vector<WhiteBlackList_TMP> & WhiteBlackList_vect,
		const char * index_file) {

	//sort(WhiteBlackList_vect.begin(), WhiteBlackList_vect.end());	// sort
	sort(WhiteBlackList_vect.begin(), WhiteBlackList_vect.end(),comp2);	// sort

	//进行数据转换
	vector<const char *> da_key_vect;
	map<string, uint32_t> da_key_map;
	vector<int32_t> da_value_vect;
	vector<char> char_vect;
	vector<int> char_vect_querylen;
	vector<char> char_vect_vr;
	vector<char> char_vect_label;
	vector<char> char_vect_type;
	vector<char> char_vect_reform;
	vector<char> char_vect_itemquery;
	vector<char> char_vect_data;
	vector<char> char_vect_iswappc;
	Pos_Reform vr_pos_reform_;
	//string vr_type_reform_itemquery_data_iswappc;
	vector<BEGIN_END> begin_end_vr;
	vector<BEGIN_END> begin_end_label;
	vector<BEGIN_END> begin_end_type;
	vector<BEGIN_END> begin_end_reform;
	vector<BEGIN_END> begin_end_itemquery;
	vector<BEGIN_END> begin_end_data;
	vector<BEGIN_END> begin_end_iswappc;
	uint32_t begin_vr = 0;
	uint32_t end_vr = begin_vr;
	uint32_t begin_label = 0;
	uint32_t end_label = begin_label;
	uint32_t begin_type = 0;
	uint32_t end_type = begin_type;
	uint32_t begin_reform = 0;
	uint32_t end_reform = begin_reform;
	uint32_t begin_itemquery = 0;
	uint32_t end_itemquery = begin_itemquery;
	uint32_t begin_data = 0;
	uint32_t end_data = begin_data;
	uint32_t begin_iswappc = 0;
	uint32_t end_iswappc = begin_iswappc;
	uint32_t num = 0;
	string prequery = "";
	string query;
	for (std::vector<WhiteBlackList_TMP>::iterator itr = WhiteBlackList_vect.begin();itr != WhiteBlackList_vect.end(); ++itr) 
	{
		query = itr->query;			// key_vrtype
		if(prequery == "")
			prequery = query;

//		cerr << "[debug_sam] query = " << query << endl;
		if(query == "")
			continue;
		if(prequery != query)
		{
			BEGIN_END b_e(begin_vr, end_vr - 1);
			begin_end_vr.push_back(b_e);// push the last query info.
			BEGIN_END b_e0(begin_label, end_label - 1);
			begin_end_label.push_back(b_e0);// push the last query info.
			BEGIN_END b_e1(begin_type,end_type-1);
			begin_end_type.push_back(b_e1);// push the last query info.
			BEGIN_END b_e2(begin_reform,end_reform -  1);
			begin_end_reform.push_back(b_e2);// push the last query info.
			BEGIN_END b_e3(begin_itemquery,end_itemquery -  1);
			begin_end_itemquery.push_back(b_e3);// push the last query info.
			BEGIN_END b_e4(begin_data,end_data -  1);
			begin_end_data.push_back(b_e4);// push the last query info.
			BEGIN_END b_e5(begin_iswappc,end_iswappc -  1);
			begin_end_iswappc.push_back(b_e5);// push the last query info.
	
			da_key_map[prequery] = num;	// record the 
			num++;
			begin_vr = end_vr;
			begin_label = end_label;
			begin_type = end_type;
			begin_reform = end_reform;
			begin_itemquery = end_itemquery;
			begin_data = end_data;
			begin_iswappc = end_iswappc;
/*			cerr << "[debug_sam] begin_vr = " << begin_vr << "\tend_vr = " << end_vr-1 << endl;
/*			cerr << "[debug_sam] begin_label = " << begin_label << "\tend_label = " << end_label-1 << endl;
			cerr << "[debug_sam] begin_type = " << begin_type << "\tend_type = " << end_type-1 << endl;
			cerr << "[debug_sam] begin_reform = " << begin_reform << "\tend_reform = " << end_reform-1 << endl;
			cerr << "[debug_sam] begin_itemquery = " << begin_itemquery << "\tend_itemquery = " << end_itemquery-1 << endl;
			cerr << "[debug_sam] begin_data = " << begin_data << "\tend_data = " << end_data-1 << endl;
			cerr << "[debug_sam] begin_iswappc = " << begin_iswappc << "\tend_iswappc = " << end_iswappc-1 << endl;
*/			
			char_vect_querylen.push_back(prequery.length());
		}

		for(int i = 0; i < itr->vr_pos_reform.size(); i++)
		{
//			cerr << "[debug_sam] i = " << i << endl;
			vr_pos_reform_ = itr->vr_pos_reform[i];
			//vr_type_reform_itemquery_data_iswappc = vr_pos_reform_.vr + "\t" + vr_pos_reform_.pos_type + "\t" + vr_pos_reform_.reform ;
//			cerr << "[debug_sam] vr_type_reform : " << vr_type_reform_itemquery_data_iswappc<< endl; 

			char_vect_push_(vr_pos_reform_.vr.c_str(), char_vect_vr, end_vr);
			char_vect_push_(vr_pos_reform_.label.c_str(), char_vect_label, end_label);
			char_vect_push_(vr_pos_reform_.pos_type.c_str(), char_vect_type, end_type);
			char_vect_push_(vr_pos_reform_.reform.c_str(),char_vect_reform,end_reform);
			
			char_vect_push_(vr_pos_reform_.item_query.c_str(), char_vect_itemquery, end_itemquery);
			char_vect_push_(vr_pos_reform_.data.c_str(), char_vect_data, end_data);
			char_vect_push_(vr_pos_reform_.iswappc.c_str(), char_vect_iswappc, end_iswappc);
			
		}
		prequery = query;
	}
	BEGIN_END b_e(begin_vr, end_vr - 1);
	begin_end_vr.push_back(b_e);// push the last query info.
	BEGIN_END b_e0(begin_label, end_label - 1);
	begin_end_label.push_back(b_e0);// push the last query info.
	BEGIN_END b_e1(begin_type,end_type-1);
	begin_end_type.push_back(b_e1);// push the last query info.
	BEGIN_END b_e2(begin_reform,end_reform -  1);
	begin_end_reform.push_back(b_e2);// push the last query info.
	
	BEGIN_END b_e3(begin_itemquery,end_itemquery -  1);
	begin_end_itemquery.push_back(b_e3);// push the last query info.
	BEGIN_END b_e4(begin_data,end_data -  1);
	begin_end_data.push_back(b_e4);// push the last query info.
	BEGIN_END b_e5(begin_iswappc,end_iswappc -  1);
	begin_end_iswappc.push_back(b_e5);// push the last query info.
	

	da_key_map[query] = num;	// record the 
	num++;
	char_vect_querylen.push_back(query.length());
	
// test
//cerr << "[debug_sam] yanzheng .................." << endl;
for(int i = 0; i < begin_end_vr.size(); i++)
{
/*	cerr << "[debug_sam] begin_vr = " << begin_end_vr[i].begin << "\tend_vr = " <<begin_end_vr[i].end << endl; 
	cerr << "[debug_sam] begin_type = " << begin_end_type[i].begin << "\tend_type = " <<begin_end_type[i].end << endl; 
	cerr << "[debug_sam] begin_reform = " << begin_end_reform[i].begin << "\tend_reform = " <<begin_end_reform[i].end << endl; 
*/
	/*cerr << "[debug_sam] begin_vr = " << begin_end_vr[i].begin << "\tend_vr = " <<begin_end_vr[i].end << endl; 
	cerr << "[debug_sam] begin_vr = " << begin_end_vr[i].begin << "\tend_vr = " <<begin_end_vr[i].end << endl; 
	cerr << "[debug_sam] begin_vr = " << begin_end_vr[i].begin << "\tend_vr = " <<begin_end_vr[i].end << endl; 

	*/
}
	// 构建双数组
	for(map<string, uint32_t>::iterator itr = da_key_map.begin(); itr != da_key_map.end(); ++itr) 
	{
		da_key_vect.push_back(itr->first.c_str());			// query 为 key
		da_value_vect.push_back(itr->second);				// position i, 指向begin，end
	}

	if(white_black_da.build(da_key_vect.size(),				// white_black_da 为事先声明的全局DA 变量
				&da_key_vect[0], 0, 
				&da_value_vect[0]) != 0)
	{
		fprintf(stderr, "build da error!\n");
		return -1;
	}

	//************************************************** 写输出文件 *************************/
//	cerr << "[debug_sam] Begin to write index file in Index_da.cpp !" << endl;
	FILE * fp_index = fopen(index_file, "wb");
	if (NULL == fp_index) 
	{
		fprintf(stderr, "Can not open file [%s]!\n", index_file);
		return -1;
	}

	// 保留file size字段
	uint32_t index_file_size = 0;
	fwrite(&index_file_size, 
			sizeof(index_file_size), 1, fp_index);			// 首先：
										// 写入1个 index_file_size 数据，其实是整个索引文件的起始位置；
//	cerr << "[debug_sam] index_file_size : " << index_file_size << endl; // sam

	// ***********************************		写入da size
	uint32_t da_size =
		white_black_da.size() * white_black_da.unit_size();
	fwrite(&da_size, sizeof(da_size), 1, fp_index);				// 写入1个 da_size 的长度 

//	cerr << "[debug_sam] da_size : " << da_size << endl; // sam
    // ***********************************		写入 da
	fclose(fp_index);

	if (white_black_da.save(index_file, "ab") != 0) 			// 将white_black_da 内容写入 index_file
	{
		fprintf(stderr, "save index error!\n");
		return -1;
	}

	//************************************		 写入begin, end 
	fp_index = fopen(index_file, "ab");
	// begin_end_vr
	begin_end_len = begin_end_vr.size();
	cerr << "[debug_sam] begin_end_len_vr : " << begin_end_len << endl; // sam
	fwrite(&begin_end_len, sizeof(begin_end_len), 1, fp_index);		// 写入1个 begin_end.size() 长度数据 
	fwrite(&(begin_end_vr[0]), sizeof(BEGIN_END), 				// 写入 begin_end.size() 个 begin_end 数据
			begin_end_vr.size(), fp_index);
	// begin_end_label
	begin_end_len = begin_end_label.size();
	cerr << "[debug_sam] begin_end_len_label : " << begin_end_len << endl; // sam
	fwrite(&begin_end_len, sizeof(begin_end_len), 1, fp_index);		// 写入1个 begin_end.size() 长度数据 
	fwrite(&(begin_end_label[0]), sizeof(BEGIN_END), 				// 写入 begin_end.size() 个 begin_end 数据
			begin_end_label.size(), fp_index);
	// begin_end_type
	begin_end_len = begin_end_type.size();
	cerr << "[debug_sam] begin_end_len_type : " << begin_end_len << endl; // sam
	fwrite(&begin_end_len, sizeof(begin_end_len), 1, fp_index);		// 写入1个 begin_end.size() 长度数据 
	fwrite(&(begin_end_type[0]), sizeof(BEGIN_END), 				// 写入 begin_end.size() 个 begin_end 数据
			begin_end_type.size(), fp_index);

	// begin_end_reform
	begin_end_len = begin_end_reform.size();
	cerr << "[debug_sam] begin_end_len_reform : " << begin_end_len << endl; // sam
	fwrite(&begin_end_len, sizeof(begin_end_len), 1, fp_index);		// 写入1个 begin_end.size() 长度数据 
	fwrite(&(begin_end_reform[0]), sizeof(BEGIN_END), 				// 写入 begin_end.size() 个 begin_end 数据
			begin_end_reform.size(), fp_index);
	// begin_end_itemquery
	begin_end_len = begin_end_itemquery.size();
	cerr << "[debug_sam] begin_end_len_itemquery : " << begin_end_len << endl; // sam
	fwrite(&begin_end_len, sizeof(begin_end_len), 1, fp_index);		// 写入1个 begin_end.size() 长度数据 
	fwrite(&(begin_end_itemquery[0]), sizeof(BEGIN_END), 				// 写入 begin_end.size() 个 begin_end 数据
			begin_end_itemquery.size(), fp_index);
	
	// begin_end_data
	begin_end_len = begin_end_data.size();
	cerr << "[debug_sam] begin_end_len_data : " << begin_end_len << endl; // sam
	fwrite(&begin_end_len, sizeof(begin_end_len), 1, fp_index);		// 写入1个 begin_end.size() 长度数据 
	fwrite(&(begin_end_data[0]), sizeof(BEGIN_END), 				// 写入 begin_end.size() 个 begin_end 数据
			begin_end_data.size(), fp_index);
	// begin_end_iswappc
	begin_end_len = begin_end_iswappc.size();
	cerr << "[debug_sam] begin_end_len_iswappc : " << begin_end_len << endl; // sam
	fwrite(&begin_end_len, sizeof(begin_end_len), 1, fp_index);		// 写入1个 begin_end.size() 长度数据 
	fwrite(&(begin_end_iswappc[0]), sizeof(BEGIN_END), 				// 写入 begin_end.size() 个 begin_end 数据
			begin_end_iswappc.size(), fp_index);

	//************************************		 写入char buf
	// vr	
	m_len_char = char_vect_vr.size();
	cerr << "[debug_sam] m_len_char_vr : " << m_len_char << endl; // sam
	fwrite(&m_len_char, sizeof(m_len_char), 1, fp_index);			// 写入1个 m_len_char 长度数据 
	fwrite(&(char_vect_vr[0]), sizeof(char_vect_vr[0]), 				// 写入 char_vect.size() 个 char_vect元素
			m_len_char, fp_index);
	// label
	m_len_char = char_vect_label.size();
	cerr << "[debug_sam] m_len_char_label : " << m_len_char << endl; // sam
	fwrite(&m_len_char, sizeof(m_len_char), 1, fp_index);			// 写入1个 m_len_char 长度数据 
	fwrite(&(char_vect_label[0]), sizeof(char_vect_label[0]), 				// 写入 char_vect.size() 个 char_vect元素
			m_len_char, fp_index);
	// type
	m_len_char = char_vect_type.size();
	cerr << "[debug_sam] m_len_char_type : " << m_len_char << endl; // sam
	fwrite(&m_len_char, sizeof(m_len_char), 1, fp_index);
	fwrite(&(char_vect_type[0]), sizeof(char_vect_type[0]), 				// 写入 char_vect.size() 个 char_vect元素
			m_len_char, fp_index);
	// reform 
	m_len_char = char_vect_reform.size();
	cerr << "[debug_sam] m_len_char_reform : " << m_len_char << endl; // sam
	fwrite(&m_len_char, sizeof(m_len_char), 1, fp_index);
	fwrite(&(char_vect_reform[0]), sizeof(char_vect_reform[0]), 				// 写入 char_vect.size() 个 char_vect元素
			m_len_char, fp_index);
	// item query 
	m_len_char = char_vect_itemquery.size();
	cerr << "[debug_sam] char_vect_itemquery = " << m_len_char << endl;
	fwrite(&m_len_char, sizeof(m_len_char), 1, fp_index);
	fwrite(&(char_vect_itemquery[0]), sizeof(char_vect_itemquery[0]), 				// 写入 char_vect.size() 个 char_vect元素
			m_len_char, fp_index);
	//data 
	m_len_char = char_vect_data.size();
	cerr << "[debug_sam] char_vect_data = " << m_len_char << endl;
	fwrite(&m_len_char, sizeof(m_len_char), 1, fp_index);
	fwrite(&(char_vect_data[0]), sizeof(char_vect_data[0]), 				// 写入 char_vect.size() 个 char_vect元素
			m_len_char, fp_index);
	// is wap pc 
	m_len_char = char_vect_iswappc.size();
	cerr << "[debug_sam] char_vect_iswappc = " << m_len_char << endl;
	fwrite(&m_len_char, sizeof(m_len_char), 1, fp_index);
	fwrite(&(char_vect_iswappc[0]), sizeof(char_vect_iswappc[0]), 				// 写入 char_vect.size() 个 char_vect元素
			m_len_char, fp_index);
	
	// querylen
	m_len_char = char_vect_querylen.size();
	cerr << "[debug_sam] char_vect_querylen = " << m_len_char << endl;
	fwrite(&m_len_char, sizeof(m_len_char), 1, fp_index);
	fwrite(&(char_vect_querylen[0]), sizeof(char_vect_querylen[0]), 				// 写入 char_vect.size() 个 char_vect元素
			m_len_char, fp_index);
	

	// 写入文件总长度
	fflush(fp_index);
	fclose(fp_index);
	
	fp_index = fopen(index_file, "r+b");
	struct stat file_info;
	if (fstat(fileno(fp_index), &file_info) == -1) 
	{
		fprintf(stderr, 
				"can not get index file info from file [%s]!\n", index_file);
		return -1;
	}
	index_file_size = file_info.st_size;
	fclose(fp_index);
	
	fp_index = fopen(index_file, "ab");
	fwrite(&index_file_size, 
			sizeof(index_file_size), 1, fp_index);			// 写入index_file_size ，即文件总长度

	cerr << "[debug_sam] index_file_size : " << index_file_size << endl; // sam
	fclose(fp_index);
	cerr << "NOTE : MakeIndex over, and write index file successfully!"<< "YEAH, you make it......... OK!" << endl;
	return 0;
}

// 读入索引
int32_t WhiteBlackList_::LoadIndex(const char *index_file, 
				Darts::DoubleArray* &da_, 
				BEGIN_END * &begin_end_vr, 
				BEGIN_END * &begin_end_label, 
				BEGIN_END * &begin_end_type, 
				BEGIN_END * &begin_end_reform, 
				BEGIN_END * &begin_end_itemquery, 
				BEGIN_END * &begin_end_data, 
				BEGIN_END * &begin_end_iswappc, 
				uint32_t &begin_end_length, 
				char* &buf_vr, 
				char* &buf_label, 
				char* &buf_type, 
				char* &buf_reform, 
				char* &buf_itemquery, 
				char* &buf_data, 
				char* &buf_iswappc, 
				int* &buf_querylen, 
				uint32_t &m_len_, 
				uint32_t &buf_vr_len, 
				uint32_t &buf_label_len, 
				uint32_t &buf_type_len, 
				uint32_t &buf_reform_len,
				uint32_t &buf_itemquery_len, 
				uint32_t &buf_data_len, 
				uint32_t &buf_iswappc_len, 
				uint32_t &buf_querylen_len)
{
	cerr << "[debug_sam] LoadIndex .........." << endl; // sam
	FILE *fp_in = fopen(index_file, "rb");				// 打开索引文件
	uint32_t index_file_size = 0;
	fread(&index_file_size, sizeof(index_file_size), 1, fp_in);	// 结合MakeIndex（）中的代码，这里指【索引起始位置】
        
	cerr << "[debug_sam]  index_file_size = " << index_file_size << endl;// sam
	// 清除原先的index
	ClearIndex();

    //读出 da
	uint32_t da_size = 0;
	if (fread(&da_size, sizeof(da_size), 1, fp_in) != 1) 		// 读入DA 索引数据的size，以便后面进行open操作
	{
		fprintf(stderr, "read da size error!\n");
		return -1;
	}
    	fclose(fp_in);
	cerr << "[debug_sam]  da_size =  " << da_size << endl;
	if(da_->open(index_file, "rb",					// 读入1个da_size大小的DA 索引数据，到 DA结构
				sizeof(index_file_size) + sizeof(da_size),
				da_size) != 0) 
	{
		fprintf(stderr, "read da error in file [%s]!\n", index_file);
		return -1;
	}
	cerr << "[debug_sam] darts address = " << da_ << endl;// sam

	fp_in = fopen(index_file, "rb");				// 重新打开索引文件，找到上次读到的位置 fp_in
	fseek(fp_in, 
			sizeof(index_file_size) + sizeof(da_size) + 
			da_size, SEEK_SET);
   	fprintf(stderr, "Load DA OK!\n");

	// begin_end_vr
	if(fread(&begin_end_len, 
				sizeof(begin_end_len), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read begin_end_len_vr!\n");
		return -1;
	}
	cerr << "[debug_sam] fread > begin_end_len : " << begin_end_len << endl;// sam
	begin_end_length = begin_end_len;
	begin_end_vr = new BEGIN_END[begin_end_len];			// 读 begin，end
	if(NULL == begin_end_vr) 
	{
		fprintf(stderr, "alloc begin_end_vr error!\n");
		return -1;
	}
	cerr << "[debug_sam] fread > begin_end_len_vr : " << begin_end_len << endl;// sam
	if(fread(begin_end_vr, 							// begin , end 
				sizeof(BEGIN_END), 
				begin_end_len, fp_in) != 
			(uint32_t)begin_end_len) 
	{
		fprintf(stderr, "read begin_end_vr error!\n");
		return -1;
	}
	// begin_end_label
	if(fread(&begin_end_len, 
				sizeof(begin_end_len), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read begin_end_len_type!\n");
		return -1;
	}
	cerr << "[debug_sam] fread > begin_end_len_label : " << begin_end_len << endl;// sam
	begin_end_label = new BEGIN_END[begin_end_len];			// 读 begin，end
	if(NULL == begin_end_label) 
	{
		fprintf(stderr, "alloc begin_end_label error!\n");
		return -1;
	}
	cerr << "[debug_sam] fread > begin_end_len_label : " << begin_end_len << endl;// sam
	if(fread(begin_end_label, 							// begin , end 
				sizeof(BEGIN_END), 
				begin_end_len, fp_in) != 
			(uint32_t)begin_end_len) 
	{
		fprintf(stderr, "read begin_end_label error!\n");
		return -1;
	}
	// begin_end_type
	if(fread(&begin_end_len, 
				sizeof(begin_end_len), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read begin_end_len_type!\n");
		return -1;
	}
	cerr << "[debug_sam] fread > begin_end_len_type : " << begin_end_len << endl;// sam
	begin_end_type = new BEGIN_END[begin_end_len];			// 读 begin，end
	if(NULL == begin_end_type) 
	{
		fprintf(stderr, "alloc begin_end_type error!\n");
		return -1;
	}
	if(fread(begin_end_type, 							// begin , end 
				sizeof(BEGIN_END), 
				begin_end_len, fp_in) != 
			(uint32_t)begin_end_len) 
	{
		fprintf(stderr, "read begin_end_type error!\n");
		return -1;
	}
	// begin_end_reform
	if(fread(&begin_end_len, 
				sizeof(begin_end_len), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read begin_end_len_reform!\n");
		return -1;
	}
	cerr << "[debug_sam] fread > begin_end_len_reform : " << begin_end_len << endl;// sam
	begin_end_reform = new BEGIN_END[begin_end_len];			// 读 begin，end
	if(NULL == begin_end_reform) 
	{
		fprintf(stderr, "alloc begin_end_reform error!\n");
		return -1;
	}
	if(fread(begin_end_reform, 							// begin , end 
				sizeof(BEGIN_END), 
				begin_end_len, fp_in) != 
			(uint32_t)begin_end_len) 
	{
		fprintf(stderr, "read begin_end_reform error!\n");
		return -1;
	}
	// begin_end_itemquery
	if(fread(&begin_end_len, 
				sizeof(begin_end_len), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read begin_end_len_itemquery!\n");
		return -1;
	}
	cerr << "[debug_sam] fread > begin_end_len_itemquery : " << begin_end_len << endl;// sam
	begin_end_itemquery = new BEGIN_END[begin_end_len];			// 读 begin，end
	if(NULL == begin_end_itemquery) 
	{
		fprintf(stderr, "alloc begin_end_itemquery error!\n");
		return -1;
	}
	if(fread(begin_end_itemquery, 							// begin , end 
				sizeof(BEGIN_END), 
				begin_end_len, fp_in) != 
			(uint32_t)begin_end_len) 
	{
		fprintf(stderr, "read begin_end_itemquery error!\n");
		return -1;
	}
	cerr << "[debug_sam] fread > begin_end_len_itemquery : " << begin_end_len << endl;// sam
	// begin_end_data
	if(fread(&begin_end_len, 
				sizeof(begin_end_len), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read begin_end_len_data!\n");
		return -1;
	}
	cerr << "[debug_sam] fread > begin_end_len_data : " << begin_end_len << endl;// sam
	begin_end_data = new BEGIN_END[begin_end_len];			// 读 begin，end
	if(NULL == begin_end_data) 
	{
		fprintf(stderr, "alloc begin_end_data error!\n");
		return -1;
	}
	if(fread(begin_end_data, 							// begin , end 
				sizeof(BEGIN_END), 
				begin_end_len, fp_in) != 
			(uint32_t)begin_end_len) 
	{
		fprintf(stderr, "read begin_end_data error!\n");
		return -1;
	}
	cerr << "[debug_sam] fread > begin_end_len_data : " << begin_end_len << endl;// sam
	// begin_end_iswappc
	if(fread(&begin_end_len, 
				sizeof(begin_end_len), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read begin_end_len_iswappc!\n");
		return -1;
	}
	cerr << "[debug_sam] fread > begin_end_len_iswappc: " << begin_end_len << endl;// sam
	begin_end_iswappc = new BEGIN_END[begin_end_len];			// 读 begin，end
	if(NULL == begin_end_iswappc) 
	{
		fprintf(stderr, "alloc begin_end_iswappc error!\n");
		return -1;
	}
	if(fread(begin_end_iswappc, 							// begin , end 
				sizeof(BEGIN_END), 
				begin_end_len, fp_in) != 
			(uint32_t)begin_end_len) 
	{
		fprintf(stderr, "read begin_end_iswappc error!\n");
		return -1;
	}
	cerr << "[debug_sam] fread > begin_end_len_iswappc : " << begin_end_len << endl;// sam
	
	// 读出char buf length
	// buf_vr
	if(fread(&m_len_char, 						// 继续读入一个长度值，即char_vect的长度值 m_len_char；
				sizeof(m_len_char), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read m_len_char!\n");
		return -1;
	}
	buf_vr_len = m_len_char;
	cerr << "[debug_sam] m_len_char_vr : " << m_len_char << endl;// sam
	m_len_ = m_len_char;
	buf_vr = new char[m_len_char];					// 定义buf_f 变量；
	if(NULL == buf_vr) 
	{
		fprintf(stderr, "alloc buf_vr error!\n");
		return -1;
	}
	
	if(fread(buf_vr, 							// 跟着读取一个m_len_char 大小的char_buf 数据，到 buf_ 结构中；
				sizeof(char), 
				m_len_char, fp_in) != 
			(uint32_t)m_len_char) 
	{
		fprintf(stderr, "read buf_vr error!\n");
		return -1;
	}
	// buf_label
	if(fread(&m_len_char, 						// 继续读入一个长度值，即char_vect的长度值 m_len_char；
				sizeof(m_len_char), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read m_len_char!\n");
		return -1;
	}
	cerr << "[debug_sam] m_len_char_label : " << m_len_char << endl;// sam
	buf_label_len = m_len_char;
	m_len_ = m_len_char;
	buf_label = new char[m_len_char];					// 定义buf_f 变量；
	if(NULL == buf_label) 
	{
		fprintf(stderr, "alloc buf_label error!\n");
		return -1;
	}
	
	if(fread(buf_label, 							// 跟着读取一个m_len_char 大小的char_buf 数据，到 buf_ 结构中；
				sizeof(char), 
				m_len_char, fp_in) != 
			(uint32_t)m_len_char) 
	{
		fprintf(stderr, "read buf_label error!\n");
		return -1;
	}
	// buf_type
	if(fread(&m_len_char, 						// 继续读入一个长度值，即char_vect的长度值 m_len_char；
				sizeof(m_len_char), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read m_len_char!\n");
		return -1;
	}
	cerr << "[debug_sam] m_len_char_type : " << m_len_char << endl;// sam
	buf_type_len = m_len_char;
	buf_type = new char[m_len_char];					// 定义buf_f 变量；
	if(NULL == buf_type) 
	{
		fprintf(stderr, "alloc buf_type error!\n");
		return -1;
	}
	
	if(fread(buf_type, 							// 跟着读取一个m_len_char 大小的char_buf 数据，到 buf_ 结构中；
				sizeof(char), 
				m_len_char, fp_in) != 
			(uint32_t)m_len_char) 
	{
		fprintf(stderr, "read buf_type error!\n");
		return -1;
	}
	// buf_reform
	if(fread(&m_len_char, 						// 继续读入一个长度值，即char_vect的长度值 m_len_char；
				sizeof(m_len_char), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read m_len_char!\n");
		return -1;
	}
	cerr << "[debug_sam] m_len_char_reform : " << m_len_char << endl;// sam
	buf_reform_len = m_len_char;
	buf_reform = new char[m_len_char];					// 定义buf_f 变量；
	if(NULL == buf_reform) 
	{
		fprintf(stderr, "alloc buf_reform error!\n");
		return -1;
	}
	
	if(fread(buf_reform, 							// 跟着读取一个m_len_char 大小的char_buf 数据，到 buf_ 结构中；
				sizeof(char), 
				m_len_char, fp_in) != 
			(uint32_t)m_len_char) 
	{
		fprintf(stderr, "read buf_reform error!\n");
		return -1;
	}
	// buf_itemquery
	if(fread(&m_len_char, 						// 继续读入一个长度值，即char_vect的长度值 m_len_char；
				sizeof(m_len_char), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read m_len_char!\n");
		return -1;
	}
	cerr << "[debug_sam] m_len_char_itemquery : " << m_len_char << endl;// sam
	buf_itemquery_len = m_len_char;
	m_len_ = m_len_char;
	buf_itemquery = new char[m_len_char];					// 定义buf_f 变量；
	if(NULL == buf_itemquery) 
	{
		fprintf(stderr, "alloc buf_itemquery error!\n");
		return -1;
	}
	
	if(fread(buf_itemquery, 							// 跟着读取一个m_len_char 大小的char_buf 数据，到 buf_ 结构中；
				sizeof(char), 
				m_len_char, fp_in) != 
			(uint32_t)m_len_char) 
	{
		fprintf(stderr, "read buf_itemquery error!\n");
		return -1;
	}

	// buf_data
	if(fread(&m_len_char, 						// 继续读入一个长度值，即char_vect的长度值 m_len_char；
				sizeof(m_len_char), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read m_len_char!\n");
		return -1;
	}
	cerr << "[debug_sam] m_len_char_data : " << m_len_char << endl;// sam
	buf_data_len = m_len_char;
	m_len_ = m_len_char;
	buf_data = new char[m_len_char];					// 定义buf_f 变量；
	if(NULL == buf_data) 
	{
		fprintf(stderr, "alloc buf_data error!\n");
		return -1;
	}
	
	if(fread(buf_data, 							// 跟着读取一个m_len_char 大小的char_buf 数据，到 buf_ 结构中；
				sizeof(char), 
				m_len_char, fp_in) != 
			(uint32_t)m_len_char) 
	{
		fprintf(stderr, "read buf_data error!\n");
		return -1;
	}
	// buf_iswappc
	if(fread(&m_len_char, 						// 继续读入一个长度值，即char_vect的长度值 m_len_char；
				sizeof(m_len_char), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read m_len_char!\n");
		return -1;
	}
	cerr << "[debug_sam] m_len_char_iswappc : " << m_len_char << endl;// sam
	buf_iswappc_len = m_len_char;
	m_len_ = m_len_char;
	buf_iswappc = new char[m_len_char];					// 定义buf_f 变量；
	if(NULL == buf_iswappc) 
	{
		fprintf(stderr, "alloc buf_iswappc error!\n");
		return -1;
	}
	
	if(fread(buf_iswappc, 							// 跟着读取一个m_len_char 大小的char_buf 数据，到 buf_ 结构中；
				sizeof(char), 
				m_len_char, fp_in) != 
			(uint32_t)m_len_char) 
	{
		fprintf(stderr, "read buf_iswappc error!\n");
		return -1;
	}

	
	// querylen
	if(fread(&m_len_char, 						// 继续读入一个长度值，即char_vect的长度值 m_len_char；
				sizeof(m_len_char), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read m_len_char!\n");
		return -1;
	}
	cerr << "[debug_sam] buf_querylen_len : " << m_len_char << endl;// sam
	buf_querylen_len = m_len_char;
	buf_querylen = new int[m_len_char];					// 定义buf_f 变量；
	if(NULL == buf_querylen) 
	{
		fprintf(stderr, "alloc buf_querylen error!\n");
		return -1;
	}
	
	if(fread(buf_querylen, 							// 跟着读取一个m_len_char 大小的char_buf 数据，到 buf_ 结构中；
				sizeof(int), 
				m_len_char, fp_in) != 
			(uint32_t)m_len_char) 
	{
		fprintf(stderr, "read buf_querylen error!\n");
		return -1;
	}
	fprintf(stderr, "Load Char Buf OK!\n");
	
/*	if(fread(&m_len_struct, sizeof(m_len_struct), 1, fp_in) 
			!=  1)// m_len_strcut, pos_reform
	{
		fprintf(stderr, "read m_len_struct error!!!\n");
		return -1;
	}
	pos_reform_buf_ = new Pos_Reform[m_len_struct];					// 定义buf_f 变量；
	if(NULL == pos_reform_buf_) 
	{
		fprintf(stderr, "alloc pos_reform_buf_ error!!!\n");
		return -1;
	}
	
	cerr << "[debug_sam] m_len_struct : " << m_len_struct << endl;// sam
	if(fread(pos_reform_buf_, 							// 跟着读取一个m_len_struct 大小的pos_reform 数据，到 buf_ 结构中；
				sizeof(Pos_Reform), 
				m_len_struct, fp_in) != 
			(uint32_t)m_len_struct) 
	{
		fprintf(stderr, "read pos_reform_buf_ error!!!\n");
		return -1;
	}
	m_len_struct_ = m_len_struct;
*/
	fclose(fp_in);
	fp_in = NULL;

	return 0;
}
int WhiteBlackList_::change2qj(string query, char* &buffer)
{
	int q_len = query.length();					// query_len
	if(query.length() < 1 || query.length()> MAXQUERYLEN){return -1;}
	buffer = new char[MAXSTRLEN];
	int len = ec->t2sgchar(query.c_str(), (gchar_t*)buffer, MAXSTRLEN/2, true);
	if(len>=MAXSTRLEN/2 || len < 0){return -1;}
	buffer[len*2] = '\0';
	return 0;
}
/********************************************************/
// make index
int32_t WhiteBlackList_::MakeIndex(const char *file, const char *index_file,const char* tag)
{
	cerr << "[debug_sam] MakeIndex file of tag: " << tag << "\tfile:" << file << endl;
	ifstream fp_in;
	fp_in.open(file, ios::in);
	if (!fp_in) 
	{
		cerr << "can't not open file:" << file;
		return -1;
	}
	vector<ST> vect;
	char line[20480];
	uint32_t reform_size = 0;
	int nn = 1;
	ST v;
	string blank = "##_##";
	string str_line;
	while (getline(fp_in, str_line)) 
	{
		spaceGary::StringTrim(str_line);
		vector<string> parts_vect;
		parts_vect.clear();
		//boost::algorithm::split(parts_vect, str_line, boost::is_any_of( "_$##$_"));
		split_sam(parts_vect, str_line, blank.c_str());
		if(parts_vect.size() < 2)
		{
			cerr << "lineth = " << nn++ << endl;
			cerr << "content = " << line << endl;
			cerr << "parts_vect.size = " << parts_vect.size() << endl;
			for(int i = 0;i < parts_vect.size(); i++)
				cerr << parts_vect[i] << "@##@";
			cerr << endl;
			cerr << "[debug_sam] ERROR : Format of file is error! .........FALSE" <<endl;
			cerr << "Format should be like this:" << endl;
			cerr << "QUERY##_##EXTEND_CONTENT" << endl;
			cerr << "e.g.," << endl;
			cerr << "匆匆那年##_##上映时期 主演" << endl;
			continue;
		}
		nn ++;
		int32_t num_parts = parts_vect.size();
		WhiteBlackList_TMP node;
		string query = parts_vect[0];				// query
		string extend = parts_vect[1];				// vr
		spaceGary::StringTrim(query);
		spaceGary::StringTrim(extend);
		//cerr << "query = " << query << ";\textend = " << extend << endl;

		int q_len = query.length();					// query_len
		if(query.length() < 1 || query.length()> MAXQUERYLEN){continue;}
		char *buffer = NULL;
		change2qj(query.c_str(), buffer);
		string gQuery(buffer);
		delete [] buffer;
		//change2qj(extend.c_str(), buffer);
		//string gExtend(buffer);
		v.query = gQuery;
		//v.extend = gExtend;
		v.extend = extend;
		//cerr << "query = " << gQuery << endl;
		//cerr << "extend = " << extend << endl;
		vect.push_back(v);
		
	}
	cerr << "tag: " << tag << endl;
	//将数据进行转换并输出到index文件中
	if(string(tag) == "uniq")
	{
		cerr << "[NOTE] GET Into OutputIndexFile(vect, index_file, uniq)" << endl;
		OutputIndexFile(vect, index_file, "uniq");	//	将要进行DA存储的vector输出到索引文件 index_file
	}
	else
		OutputIndexFile(vect, index_file);	// 这个函数有问题，对于第一个索引项打印出来的是乱码；待查证；	

	fp_in.close();

	return 0;
}
//输出index文件
int32_t WhiteBlackList_::OutputIndexFile_common(std::vector<ST> & vect,const char * index_file, string isUniq) 
{
	sort(vect.begin(), vect.end(), comp);	// sort
	vector<char> char_vect_extend;
	vector<string> parts;
	map<string, uint32_t> dic_map;
	string query;
	string extend;
	uint32_t start = 0;
	uint32_t end = start;
	for(int i = 0; i < vect.size(); i++)
	{
		parts.clear();
		seg->Segment_(
			vect[i].query,
			parts);
		if(parts.size() < 1)
		{
			cerr << "query = " << vect[i].query << endl;
			cerr <<"[note] segment size lt 1" << endl;
			continue;
		}
		string dest_norm = "";
		for(int i = 0; i < parts.size(); i++)
		{
			dest_norm += parts[i];
		}
		query = dest_norm;
		//query = vect[i].query;
		extend = vect[i].extend;
		dic_map[query] = start;
		char_vect_push_(extend.c_str(), char_vect_extend, end);
		start += extend.length() + 1;
	}
	map<string,uint32_t>::iterator itr;
	vector<const char*> da_key_vect;
	vector<int32_t> da_value_vect;
	for(map<string, uint32_t>::iterator itr = dic_map.begin(); itr != dic_map.end(); ++itr) 
	{
		da_key_vect.push_back(itr->first.c_str());			// query 为 key
		da_value_vect.push_back(itr->second);				// position i, 指向begin，end
	}

	if(extend_da.build(da_key_vect.size(), &da_key_vect[0], 0, &da_value_vect[0]) != 0)
	{
		fprintf(stderr, "build da error!\n");
		return -1;
	}

	// 写输出文件
	FILE * fp_index = fopen(index_file, "wb");
	if (NULL == fp_index) 
	{
		fprintf(stderr, "Can not open file [%s]!\n", index_file);
		return -1;
	}
	// 保留file size字段
	uint32_t index_file_size = 0;
	fwrite(&index_file_size, 
			sizeof(index_file_size), 1, fp_index);
	// 写入da size
	uint32_t da_size =
		extend_da.size() * extend_da.unit_size();
	fwrite(&da_size, sizeof(da_size), 1, fp_index);
	fclose(fp_index);
	if (extend_da.save(index_file, "ab") != 0)
	{
		fprintf(stderr, "save index error!\n");
		return -1;
	}

	fp_index = fopen(index_file, "ab");
	uint32_t size_ = char_vect_extend.size();
	fwrite(&size_, sizeof(size_), 1, fp_index);
	fwrite(&(char_vect_extend[0]), sizeof(char),
			char_vect_extend.size(), fp_index);
	
	// 写入文件总长度
	fflush(fp_index);
	fclose(fp_index);
	
	fp_index = fopen(index_file, "r+b");
	struct stat file_info;
	if (fstat(fileno(fp_index), &file_info) == -1) 
	{
		fprintf(stderr, 
				"can not get index file info from file [%s]!\n", index_file);
		return -1;
	}
	index_file_size = file_info.st_size;
	fclose(fp_index);
	
	fp_index = fopen(index_file, "ab");
	fwrite(&index_file_size, 
			sizeof(index_file_size), 1, fp_index);			// 写入index_file_size ，即文件总长度

	fclose(fp_index);
	cout << "output index file over .........." << endl;	
	return 0;
}


//输出index文件
int32_t WhiteBlackList_::OutputIndexFile(std::vector<ST> & vect,const char * index_file, string isUniq) 
{
	sort(vect.begin(), vect.end(), comp);	// sort
	vector<char> char_vect_extend;
	vector<string> parts;
	map<string, uint32_t> dic_map;
	string query;
	string extend;
	uint32_t start = 0;
	uint32_t end = start;
	for(int i = 0; i < vect.size(); i++)
	{
		parts.clear();
		seg->Segment_(
			vect[i].query,
			parts);
		if(parts.size() < 1)
		{
			cerr << "query = " << vect[i].query << endl;
			cerr <<"[note] segment size lt 1" << endl;
			continue;
		}
		string dest_norm = "";
		for(int i = 0; i < parts.size(); i++)
		{
			dest_norm += parts[i];
		}
		query = dest_norm;
		//query = vect[i].query;
		extend = vect[i].extend;
		dic_map[query] = start;
		char_vect_push_(extend.c_str(), char_vect_extend, end);
		start = end;
		//start = end + 2;
		//start += extend.length() + 1;
	}
	map<string,uint32_t>::iterator itr;
	vector<const char*> da_key_vect;
	vector<int32_t> da_value_vect;
	for(map<string, uint32_t>::iterator itr = dic_map.begin(); itr != dic_map.end(); ++itr) 
	{
		da_key_vect.push_back(itr->first.c_str());			// query 为 key
		da_value_vect.push_back(itr->second);				// position i, 指向begin，end
	}

	if(extend_da.build(da_key_vect.size(), &da_key_vect[0], 0, &da_value_vect[0]) != 0)
	{
		fprintf(stderr, "build da error!\n");
		return -1;
	}

	// 写输出文件
	FILE * fp_index = fopen(index_file, "wb");
	if (NULL == fp_index) 
	{
		fprintf(stderr, "Can not open file [%s]!\n", index_file);
		return -1;
	}
	// 保留file size字段
	uint32_t index_file_size = 0;
	fwrite(&index_file_size, 
			sizeof(index_file_size), 1, fp_index);
	// 写入da size
	uint32_t da_size =
		extend_da.size() * extend_da.unit_size();
	fwrite(&da_size, sizeof(da_size), 1, fp_index);
	fclose(fp_index);
	if (extend_da.save(index_file, "ab") != 0)
	{
		fprintf(stderr, "save index error!\n");
		return -1;
	}

	fp_index = fopen(index_file, "ab");
	uint32_t size_ = char_vect_extend.size();
	fwrite(&size_, sizeof(size_), 1, fp_index);
	fwrite(&(char_vect_extend[0]), sizeof(char),
			char_vect_extend.size(), fp_index);
	
	// 写入文件总长度
	fflush(fp_index);
	fclose(fp_index);
	
	fp_index = fopen(index_file, "r+b");
	struct stat file_info;
	if (fstat(fileno(fp_index), &file_info) == -1) 
	{
		fprintf(stderr, 
				"can not get index file info from file [%s]!\n", index_file);
		return -1;
	}
	index_file_size = file_info.st_size;
	fclose(fp_index);
	
	fp_index = fopen(index_file, "ab");
	fwrite(&index_file_size, 
			sizeof(index_file_size), 1, fp_index);			// 写入index_file_size ，即文件总长度

	fclose(fp_index);
	cout << "output index file over .........." << endl;	
	return 0;
}

//输出index文件
int32_t WhiteBlackList_::OutputIndexFile(std::vector<ST> & vect,const char * index_file) 
{
	sort(vect.begin(), vect.end(), comp);	// sort
	vector<char> char_vect_extend;
	map<string, uint32_t> dic_map;
	string query;
	string extend;
	uint32_t start = 0;
	uint32_t end = start;
	string prequery = "";
	for(int i = 0; i < vect.size(); i++)
	{
		query = vect[i].query;
		if(dic_map.find(query.c_str()) != dic_map.end())	// 对于重复的query，其extend 内容进行叠加;且用 ||| 分割开；
		{
			spaceGary::StringTrim(vect[i].extend);
			extend += "|||" + vect[i].extend;
		}else{
		   	if(prequery != "")
			{
				//cout << "query:" << query<< endl;
				//cout << "extend:" << extend << endl;
				char_vect_push_(extend.c_str(), char_vect_extend, end);
				start = end;
				//start = end + 2;
				//start += extend.length() + 1;
			}else
			{
			}
			dic_map[query] = start;
			spaceGary::StringTrim(vect[i].extend);
			extend = vect[i].extend;
		}
		if(i == vect.size() - 1)
		{
			//cout << "query:" << query<< endl;
			//cout << "extend:" << extend << endl;
			char_vect_push_(extend.c_str(), char_vect_extend, end);
		}
		prequery = query;
		//cerr << "extend = " << extend << endl;
	}
	map<string,uint32_t>::iterator itr;
	vector<const char*> da_key_vect;
	vector<int32_t> da_value_vect;
	for(map<string, uint32_t>::iterator itr = dic_map.begin(); itr != dic_map.end(); ++itr) 
	{
		da_key_vect.push_back(itr->first.c_str());			// query 为 key
		da_value_vect.push_back(itr->second);				// position i, 指向begin，end
	}

	if(extend_da.build(da_key_vect.size(), &da_key_vect[0], 0, &da_value_vect[0]) != 0)
	{
		fprintf(stderr, "build da error!\n");
		return -1;
	}

	// 写输出文件
	FILE * fp_index = fopen(index_file, "wb");
	if (NULL == fp_index) 
	{
		fprintf(stderr, "Can not open file [%s]!\n", index_file);
		return -1;
	}
	// 保留file size字段
	uint32_t index_file_size = 0;
	fwrite(&index_file_size, 
			sizeof(index_file_size), 1, fp_index);
	// 写入da size
	uint32_t da_size =
		extend_da.size() * extend_da.unit_size();
	fwrite(&da_size, sizeof(da_size), 1, fp_index);
	fclose(fp_index);
	if (extend_da.save(index_file, "ab") != 0)
	{
		fprintf(stderr, "save index error!\n");
		return -1;
	}

	fp_index = fopen(index_file, "ab");
	uint32_t size_ = char_vect_extend.size();
	fwrite(&size_, sizeof(size_), 1, fp_index);
	fwrite(&(char_vect_extend[0]), sizeof(char),
			char_vect_extend.size(), fp_index);
	
	// 写入文件总长度
	fflush(fp_index);
	fclose(fp_index);
	
	fp_index = fopen(index_file, "r+b");
	struct stat file_info;
	if (fstat(fileno(fp_index), &file_info) == -1) 
	{
		fprintf(stderr, 
				"can not get index file info from file [%s]!\n", index_file);
		return -1;
	}
	index_file_size = file_info.st_size;
	fclose(fp_index);
	
	fp_index = fopen(index_file, "ab");
	fwrite(&index_file_size, 
			sizeof(index_file_size), 1, fp_index);			// 写入index_file_size ，即文件总长度

	fclose(fp_index);
	cout << "output index file over .........." << endl;	
	return 0;
}
int32_t WhiteBlackList_::LoadIndex(const char *index_file, 
									Darts::DoubleArray* &da_, 
									char* &buf_extend,
									uint32_t& buf_extend_len)
{
	FILE *fp_in = fopen(index_file, "rb");
	if(fp_in == NULL)
	{
		cerr << "[ERROR] Fail to open DA index file" << endl;
		return -1;
	}
	uint32_t index_file_size = 0;
	fread(&index_file_size, sizeof(index_file_size), 1, fp_in);

    //读出 da
	uint32_t da_size = 0;
	if (fread(&da_size, sizeof(da_size), 1, fp_in) != 1)
	{
		fprintf(stderr, "read da size error!\n");
		return -1;
	}
    fclose(fp_in);
	if(da_->open(index_file, "rb",
				sizeof(index_file_size) + sizeof(da_size),
				da_size) != 0) 
	{
		fprintf(stderr, "read da error in file [%s]!\n", index_file);
		return -1;
	}
	fp_in = fopen(index_file, "rb");
	fseek(fp_in, 
			sizeof(index_file_size) + sizeof(da_size) + 
			da_size, SEEK_SET);
   	fprintf(stderr, "Load DA OK!\n");
	// buf_extend_len
	if(fread(&buf_extend_len, 
				sizeof(buf_extend_len), 1, fp_in) != 1) 
	{
		fprintf(stderr, "can read buf_extend_len!\n");
		return -1;
	}
	buf_extend = new char[buf_extend_len];			// 读 begin，end
	if(NULL == buf_extend) 
	{
		fprintf(stderr, "alloc buf_extend error!\n");
		return -1;
	}
	if(fread(buf_extend, 							// begin , end 
				sizeof(char), 
				buf_extend_len, fp_in) != 
			(uint32_t)buf_extend_len) 
	{
		fprintf(stderr, "read buf_extend error!\n");
		return -1;
	}
	
	fclose(fp_in);
	fp_in = NULL;
	return 0;
}
int32_t WhiteBlackList_::ClearIndex() 
{
	//white_black_da.clear();

	m_len_char = 0;
	if (NULL != m_char_buf) 
	{
		delete[] m_char_buf;
		m_char_buf = NULL;
	}
	
	return 0;
}


};
