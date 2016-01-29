#include "Segment.h"

using namespace std;
const int32_t MAXLEN_ = 20480;

SEGMENT_1* SEGMENT_1::ddinstance;

SEGMENT_1::SEGMENT_1():
    m_graph_segment(65536)
{
	EncodingConvertor::initializeInstance(); 
    analysis::DicTree::instance()->loadDic();
    m_graph_segment.open(analysis::DicTree::instance());
}

SEGMENT_1::~SEGMENT_1()
{
	Release();
}

SEGMENT_1* SEGMENT_1::getInstance(){
	if(!ddinstance){
		ddinstance=new SEGMENT_1();
	}
	return ddinstance;
}
void SEGMENT_1::releaseInstance(){
	if(ddinstance){
		delete ddinstance;
		ddinstance=NULL;
	}
}



int32_t SEGMENT_1::Init()
{
    //m_graph_segment.open(analysis::DicTree::instance());
}
int32_t SEGMENT_1::Release()
{
}

int32_t SEGMENT_1::Segment_(
		const string& query,
		std::vector<std::string>& parts)
{
	parts.clear();
    char dest[80000];// 25600];
    char buf[MAXLEN_];
    bchar_t zero[1] = {L'\0'};
    if(query.length()<2) //³¤¶È¹ý¶Ì
		return 0;
	if(query.length() > MAXLEN_)
	{
		cerr << "[NOTE] query is too long" << endl;
		return -1;
	}
    analysis::ResultInfo info[MAXLEN_];
	StringUtils::gbk_utf16le(
			strcpy(buf,query.c_str()), 
			query.length(), 
			dest);
    int r = m_graph_segment.graphSegment(
			(bchar_t*)dest,
			bcscspn((bchar_t*)dest,zero),
			info,
			MAXLEN_);

    for (int i = 0; i < r; i++) 
	{
		StringUtils::utf16le_gbk(
				dest+info[i].pos*2, 
				info[i].tlen*2, 
				buf);
		string word = buf;
		parts.push_back(string(buf));
    }
	return 0;
}



