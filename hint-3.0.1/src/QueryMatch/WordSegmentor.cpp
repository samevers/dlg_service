#include "WordSegmentor.h"

using namespace std;

const static int MAXSTRLEN=1024;

WordSegmentor* WordSegmentor::ddinstance;

WordSegmentor::WordSegmentor() {
	dictree = analysis::DicTree::instance();
//	dictree->loadDic();
	iqdic = analysis::IQDic::instance();
//	iqdic->loadDic();
	ec = EncodingConvertor::getInstance();
}

WordSegmentor::~WordSegmentor() {
}

WordSegmentor* WordSegmentor::getInstance(){
        if(!ddinstance){
                ddinstance=new WordSegmentor();
        }
        return ddinstance;
}

void WordSegmentor::releaseInstance(){
        if(ddinstance){
                delete ddinstance;
		ddinstance = NULL;
        }
}

void WordSegmentor::gbk_utf16le(char* inbuf, size_t inlen, char* outbuf) const{
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

void WordSegmentor::utf16le_gbk(char* inbuf, size_t inlen, char* outbuf) const{
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

bool WordSegmentor::haveSecond(uint32_t termID) {
    if((termID & 0x0f000000) != 0x0f000000)
        return false;
    uint32_t type = termID & 0xf0000000; 
    if(type == 0x30000000 || type == 0x40000000 || type == 0x60000000 || type == 0xa0000000)
        return true;
    return false;
}

bool WordSegmentor::isSecondTerm(uint32_t termID) {
    if((termID & 0x0f000000) != 0x0e000000) {
        return false;
    }
    uint32_t type = termID & 0xf0000000;
    if(type == 0x30000000 || type == 0x40000000 || type == 0x60000000 || type == 0xa0000000)
    {
        return true;
    }
    return false;
}

int WordSegmentor::Segment(analysis::QuerySegmentor *Wordseg, const char* Query, analysis::WordSegmentorInfo& Request, analysis::WordSegmentorInfo& Parsed, std::vector<string>& TermList){
	memset(&Request,0,sizeof(Request));
//	memset(&Parsed,0,sizeof(Parsed));
	if (Query == NULL) {
		//cerr << "Null word for wordsegment" << endl;;
		return -1;
	 }

	int len = strlen(Query);
//	cout << "Len: " << len << endl;
	 if (len >= MAXSTRLEN) {
		 //cerr << "Word for wordsegment is too long" << endl;
		 return -1;
	 }
	bchar_t zero[1] = {L'\0'};
	char buffer[MAXSTRLEN*4];
	char tmp[MAXSTRLEN];

	memset(&tmp,0,sizeof(tmp));
        memset(&buffer,0,sizeof(buffer));
        Request.phrasesCount = 1;
	strcpy(tmp,Query);
	
        gbk_utf16le(tmp, strlen(tmp), buffer);

	bchar_t * gdest = (bchar_t*)buffer;
        len = bcscspn(gdest,zero);
        //query传给分词的串保持一致的格式
        for (int i=0; i<len; i++) {
                if ( gdest[i]==0x3000 ) {
                        gdest[i] = phraseSep_utf16;
                        Request.phrasesCount++;
                }
        }
        bcscpy(Request.queryString,(bchar_t*)buffer);
//	Wordseg->open(dictree,iqdic);
	timeval begin;
//    getTimeNow(&begin, NULL);
	Wordseg->segment(&Request, Parsed);

	for (int i=0; i<Parsed.termsCount; i++) {
		TermList.push_back(GetTermText(Request,Parsed,i));
	}
	return 0;

}

std::string WordSegmentor::GetTermText(const analysis::WordSegmentorInfo& Request, const analysis::WordSegmentorInfo& Parsed, int Num) {
	string destString = "";
	if(isSecondTerm(Parsed.terms[Num])) {
	    return destString; //second term , return empty
	}
	bchar_t zero[1] = {L'\0'};
	char dest[MAXSTRLEN*4];
//	char buf[MAXSTRLEN];
	char bufQuanjiao[MAXSTRLEN];
//	memset(buf,0,MAXSTRLEN);
	if(haveSecond(Parsed.terms[Num])) {
		utf16le_gbk((char *)(Request.queryString) + Parsed.termPositions[Num] *2 , (Parsed.termLength[Num] + Parsed.termLength[Num+1])*2, dest);
	}
	else {
		utf16le_gbk((char *)(Request.queryString) + Parsed.termPositions[Num] *2 , Parsed.termLength[Num]*2, dest);
//		WordSegmentor::dictree->getTermText(Parsed.terms[Num],(bchar_t*)buf);
//		utf16le_gbk(buf, bcscspn((bchar_t*)buf,zero)*2, dest);
	}
        int len = ec->dbc2gchar(dest, (gchar_t*)bufQuanjiao, MAXSTRLEN/2, true);
        if(len>=MAXSTRLEN/2 || len < 0){return false;}
        bufQuanjiao[len*2] = '\0';
	destString = string(bufQuanjiao);
        return destString;
}
