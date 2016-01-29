#include "EditDist.h"

using namespace std;

EditDist::EditDist()
{
	;
}
EditDist::~EditDist()
{
	;
}

int min(int i , int j)
{
	return (i > j ? j:i);
}


int EditDist::edit(vector<string>& str1, vector<string>& str2)
{
	int max1 = str1.size();
	int max2 = str2.size();

	int **ptr = new int*[max1 + 1];
	for(int i = 0; i < max1 + 1; i++) // the zero column is the init number, from 0 to max1. The same with the rows. So, there are (max1 + 1)*(max2 + 1) cells, in total.
	{
		ptr[i] = new int[max2 + 1];
	}
	for(int i = 0; i < max1 + 1; i++) //  there is (max1 + 1) cells in total in a column.
	{
		ptr[i][0] = i;
	}
	for(int j = 0; j < max2 + 1; j++) //  there is (max2 + 1) cells in total in a row.
	{
		ptr[0][j] = j;
	}
	for(int i = 1; i < max1 + 1; i++)
	{
		for(int j = 1; j < max2 + 1; j++)
		{
			int d;
			int tmp = min(ptr[i][j-1] + 1,ptr[i-1][j] + 1);
			if(str1[i-1] == str2[j-1])
			{
				d = 0;
			}else
			{
				d = 1;
			}
			ptr[i][j] = min(tmp, ptr[i-1][j-1] + d);
		}
	}
	// distance is = 
	int dis = ptr[max1][max2];
	for(int i = 0; i < max1 + 1; i++)
	{
		delete [] ptr[i];
		ptr[i] = NULL;
	}

	// delete **ptr
	delete [] ptr;
	ptr = NULL;

	// return distance
	return dis;
}

int32_t EditDist::Calculate(string str1, string str2, SEGMENT_1* seg)
{
	vector <string> vec1;
	vector <string> vec2;

	// vec1
	seg->Segment_(
			str1,
			vec1);
	if(vec1.size() < 1)
	{
		cerr <<"[note] segment size lt 1" << endl;
		return 0;
	}
	// vec2
	seg->Segment_(
			str2,
			vec2);
	if(vec2.size() < 1)
	{
		cerr <<"[note] segment size lt 1" << endl;
		return 0;
	}

	int32_t distance = 0;
	distance = edit(vec1,vec2);

	return distance;
}

bool SortByValue(const int v1, const int v2)
{
	return v1 < v2;
}

int32_t EditDist::EditSort(string str1, vector<string>& cadidate, map<int, string>& cadidate_sort, vector<int>& sortED, SEGMENT_1 * seg)
{
	int32_t edis;
	map<string, int> uniq_cadidate;
	for(int i = 0; i < cadidate.size(); i++)
	{

		edis = Calculate(str1, cadidate[i], seg);
		if(cadidate_sort.find(edis) != cadidate_sort.end())
		{
			if(uniq_cadidate.find(cadidate[i]) == uniq_cadidate.end())
			{
				cadidate_sort[edis] += cadidate[i] + "###";
				uniq_cadidate[cadidate[i] ] ++;
			}

		}else
		{
			cadidate_sort[edis] = cadidate[i] + "###";
			sortED.push_back(edis);
			uniq_cadidate[cadidate[i] ] ++;
		}
	}
	
	// sort
	sort(sortED.begin(), sortED.end(), SortByValue);

	return 0;

}


