#include "normalize.h"
/*
* 2012-10-31
* huyonghua@baidu.com
*/

int codetype;
map<string,int> stopwd; 
const unsigned int MAX_PAGE_LEN = 512000;


int load_stopwd()
{
	string path = "./data/tieba/stopword/stopword.txt";
	ifstream dictst;
	dictst.open(path.c_str(),ios::in);
	if(!dictst)
	{
		return -1;
	}

	string line;
	while(getline(dictst,line))
	{
		if(line.length() > 0)
			stopwd[line]++;
	}
	dictst.close();
	return 0;
}
int normalize_init()
{
/*	if(load_stopwd() == -1)
	{
		//UB_LOG_WARNING("Failed to load stopword dict.\n");
		return -1;
	}
*/
	return 0;
}

void normalize_release()
{
	stopwd.clear();
}

// return the smaller
int miner(int a,int b)
{
	if(a <= b)
	{
		return a;
	}else
		return b;
}

//template<class T>
void swap(float *a,float *b)
{

	float temp = *a;
	*a = *b;
	*b = temp;

}

//template<class T>
int Partition(char* array[],float R[],int low,int high)
{
	float pivot = R[low];
	//char* term = array[low];
	while(low < high)
	{
		while(low < high && R[high] <= pivot)
			high--;
		swap(R[low],R[high]);
		swap(array[low],array[high]);
		while(low < high && R[low] >= R[high])
			low++;
		swap(R[high],R[low]);
		swap(array[high],array[low]);
	}

	return low;
}

//template<class T>
void QSort(char* array[],float R[], int low,int high)
{

	if(low < high)
	{
		int position = Partition(array,R,low,high);
		QSort(array,R,low,position - 1);
		QSort(array,R,position + 1,high);
	}
}


int is_digit(const char* str)
{
	//if(*str >= 0x61 && *str <= 0x7A)
	//if(*str >= 0x41 && *str <= 0x5A)
	if(*str >= 0x30 && *str <= 0x39)
	{
		return 1;
	}else
	{
		return 0;
	}
}

// return the smaller that > 0
int miner2(int a,int b)
{
	if(a <= b)
	{
		if(a != -1)
			return a;
		else if(b != -1)
			return b;
		else
			return -1;
	}else
	{
		if(b != -1)
			return b;
		else if(a != -1)
			return a;
		else
			return -1;
	}
}

// return the smaller
int miner3(int a,int b, int c)
{
	int d; 
	d = (b<=c?b:c);

	if(a <= d)
	{
		return a;
	}else
		return d;
}

// 
// ???url ????(url???token??url)
string doUrl(string& str, vector<string> url_vec)
{
	int loc;
	string wd;
	for(unsigned int i = 0; i < url_vec.size(); i++)
	{
		wd = url_vec[i];
		if(wd.length() > 0)
			while((loc = str.find(wd.c_str())) != -1 )
			{
				str.replace(loc,wd.length(),"");
			}
	}

	return str;
}

// ????????
void replace_stopwd(string& str)
{
	string wd;
	int loc;
	map<string,int>::iterator iter;
	for( iter = stopwd.begin(); iter != stopwd.end(); iter++)
	{
		wd = iter->first;

		if(wd.length() > 0)
			while((loc = str.find(wd.c_str())) != -1)
			{
				str = str.replace(loc,wd.length(),"");
				//line = line.substr(0,loc) + line.substr(loc + wd.length(), line.length() - loc - wd.length() );
			}
	}
}

string html_content(string line)
{
	string img = "<img ";
	string span = "<span class=";
	string href = "<a href=";
	string out;
	string url;

	int loc;
	int loc_end;
	int tag = 0;

	string html1;
	string set1[] = {"<img ","<embed "};
	int size1 = 2;
	for(int i = 0; i < size1; i++)
	{
		html1 = set1[i];
		while((loc = line.find(html1.c_str(),tag) ) != -1 )
		{
			loc_end = line.find(">",loc+html1.length());
			if(loc_end != -1)
			{
				url = line.substr(loc+html1.length(),loc_end - loc - html1.length());
				int loc1 = url.find("src=");
				if(loc1 != -1)
				{
					int loc2= url.find("\"",loc1 + 5);
					if(loc2 != -1)
					{
						string url = url.substr(loc1 + 5,loc2 - loc1 - 5);
						out += url + " ";
					}else
					{
						break;
					}
				}else
					break;
				//out += url + " ";
				tag = loc_end + 1;
			}else
			{
				break;
			}
		}
	}

	string html;
	string set[] = {"<img ","<span class=","<a href=","<embed "};
	int size = 4;
	for(int i = 0; i < size; i++)
	{
		html = set[i];
		while((loc = line.find(html.c_str()) ) != -1 )
		{
			if((loc_end = line.find(">",loc+html.length())) != -1 )
			{
				string html_temp = line.substr(loc,loc_end + 1 - loc);
				line.replace(loc,html_temp.length(),"");
			}
		}
	}

	out += line;
	//UB_LOG_WARNING("%s \n",out.c_str());
	return out;
}


// ?? url(demo)
// ?http:,https:?www.??ftp: ??????url,?????????"??<???
// samhu.
void url_punc(string& line)
{
	vector<string> url_vec;
	string substr;
	string url;

	//line =  doUrl(line,url_vec);
	//line = html_content(line);
	replace_stopwd(line);
}

// samhu.
// return: 0 ???????;1 ????;
int  text_normalize(const char * oritext, char * destbuff, int bufsize)
{
	//
	unsigned int size = bufsize;
	string line = oritext;
	string line_predo = "";
	char* line_chomp = (char*)malloc(4*size*sizeof(char));
	if( line_chomp == NULL )
	{
		//UB_LOG_WARNING("Error: Failed in malloc operation, in text_textnormalize().\n");
		return -1;
	}
	char* tempp = (char*)malloc(4*size*sizeof(char));
	if( tempp == NULL )
	{
		free(line_chomp);
		line_chomp = NULL;
		return -1;
	}
	strcpy(tempp,oritext);
	string line_tmp = line_chomp;

	// ?????????¡®\0¡¯?;
	if(line_tmp.find("\\0") == line_tmp.length()-2)
	{
		line_tmp = line_tmp.substr(0,line_tmp.length() - 2);
	}

	// ???????
	line_tmp = line_chomp;

	url_punc(line_tmp);														// ??url,???????

	int loc;
	while((loc = line_tmp.find("  ")) != -1)								// ?????????????
	{
		line_tmp.replace(loc,2," ");
	}

	if(line_predo.length() < 4*size)
		strcpy(tempp,line_predo.c_str());
	else
	{
		free(line_chomp);
		line_chomp = NULL;
		free(tempp);
		tempp = NULL;
		ul_ccode_conv_close();
		return -1;
	}

	line_predo = line_chomp;

	if(line_predo.length() < 5 || (unsigned int)bufsize < line_predo.length())		// line??< 5 ??
	{
		free(line_chomp);
		line_chomp = NULL;
		free(tempp);
		tempp = NULL;
		return -1;
	}

	// outcome
	if((unsigned int)bufsize >= line_predo.length())
		strcpy(destbuff,line_predo.c_str());
	else
	{
		free(line_chomp);
		line_chomp = NULL;
		free(tempp);
		tempp = NULL;
		ul_ccode_conv_close();
		return -1;
	}
	// release
	free(line_chomp);
	line_chomp = NULL;
	free(tempp);
	tempp = NULL;
	return line_predo.length();
}
