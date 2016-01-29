#include "GraphSegmentor.h"

using namespace std;

const static int MAXSTRLEN=1024;

GraphSegmentor* GraphSegmentor::ddinstance;

GraphSegmentor::GraphSegmentor() {
	dictree = analysis::DicTree::instance();
//	dictree->loadDic();
	ec = EncodingConvertor::getInstance();
}

GraphSegmentor::~GraphSegmentor() {
}

GraphSegmentor* GraphSegmentor::getInstance(){
        if(!ddinstance){
                ddinstance=new GraphSegmentor();
        }
        return ddinstance;
}

void GraphSegmentor::releaseInstance(){
        if(ddinstance){
                delete ddinstance;
		ddinstance = NULL;
        }
}

void GraphSegmentor::gbk_utf16le(char* inbuf, size_t inlen, char* outbuf) {
        iconv_t cd = iconv_open( "UTF-16LE//IGNORE", "GBK");
        bzero( outbuf, inlen*4);
        char *in = inbuf;
        char *out = outbuf;
        size_t outlen = inlen*4;
        iconv(cd, &in, (size_t *)&inlen, &out,&outlen);
        //outlen为outbuf未被占用的剩余字节数
        if( outlen>=sizeof(bchar_t) )
                *((bchar_t *) out) = L'\0';
        iconv_close(cd);
}

void GraphSegmentor::utf16le_gbk(char* inbuf, size_t inlen, char* outbuf) {
        iconv_t cd = iconv_open( "GBK//IGNORE", "UTF-16LE");
        bzero( outbuf, inlen*4);
        char *in = inbuf;
        char *out = outbuf;
        size_t outlen = inlen*4;
        iconv(cd, &in, (size_t *)&inlen, &out,&outlen);
        //outlen为outbuf未被占用的剩余字节数
        if( outlen>=sizeof(char) )
                *((char *) out) = '\0';
        iconv_close(cd);
}

int GraphSegmentor::Segment(analysis::TGraph* segmentor,const char* Query, analysis::ResultInfo* Info, int Length, char* Query_utf16) {
	if (Query == NULL) {
		//std::cerr << "Null word for wordsegment" << std::endl;;
		return -1;
	 }

	int len = strlen(Query);
	 if (len >= MAXSTRLEN) {
		 //std::cerr << "Word for wordsegment is too long" << std::endl;
		 return -1;
	 }
	bchar_t zero[1] = {L'\0'};
	char tmp[MAXSTRLEN];
	strcpy(tmp,Query);
    	gbk_utf16le(tmp, strlen(tmp), Query_utf16);
//	segmentor->open(dictree);
//	segmentor->extSeg(true);
	int ret = segmentor->graphSegment((bchar_t*)Query_utf16, bcscspn((bchar_t*)Query_utf16,zero), Info, Length, false);
	return ret;

}

std::string GraphSegmentor::GetTermText(char* Query_utf16, analysis::ResultInfo* Info, const int Num) {
	if (Query_utf16 == NULL || Info == NULL) {
        //std::cerr << "Null word for getTerm" << std::endl;
        return "";
	}
	char buf[MAXSTRLEN];
	char bufQuanjiao[MAXSTRLEN];
	utf16le_gbk(Query_utf16 + Info[Num].pos*2, Info[Num].tlen*2, buf);
	int len = ec->dbc2gchar(buf, (gchar_t*)bufQuanjiao, MAXSTRLEN/2, true);
	if(len>=MAXSTRLEN/2 || len < 0){return false;}
        bufQuanjiao[len*2] = '\0';
	return string(bufQuanjiao);
}
