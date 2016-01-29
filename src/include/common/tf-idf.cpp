#include "tf-idf.h"

using namespace std;

TF_IDF::TF_IDF()
{
	num_doc = 0;
}
TF_IDF::~TF_IDF()
{
	tf.clear();
	idf.clear();
	tf_idf.clear();
}

int TF_IDF::Train_data_count(ifstream& infile)
{
	if(!infile)
	{
		cerr << "[ERROR] Fail to open input file." << endl;
		return -1;
	}

	string line;
	string word;
	vector<string> vec;
	map<string, int> tmp;
	map<string, int>::iterator iter;
	while(getline(infile, line))
	{
		spaceGary::StringTrim(line);
		spaceGary::StringSplit(line, vec, " ");
		num_doc++;

		tmp.clear();
		// tf
		for(int i = 0; i < vec.size(); i++)
		{
			word = vec[i];
			tf[word]++;
			tmp[word] = 1;
		}
	
		// idf	
		for(iter = tmp.begin(); iter != tmp.end(); iter++)
		{
			df[iter->first]++;
		}
	}
	return 0;
}


int TF_IDF::Tf_Idf_Count()
{
	string word;
	map<string, int32_t>::iterator iter;
	//int32_t tf;
	//int32_t df;
	for(iter = tf.begin(); iter != tf.end(); iter++)
	{
		word = iter->first;
		tf_idf[word] = (double)tf[word]/((double)df[word] + 0.1);
	}
	return 0;
}

int TF_IDF::Output_Tf_Idf(string& outputfile)
{
	ofstream outfile;
	outfile.open(outputfile.c_str(), ios::out);
	if(!outfile)
	{
		cerr << "[ERROR] Fail to build output file." << endl;
		return -1;
	}

	map<string, double>::iterator iter;
	for(iter = tf_idf.begin(); iter != tf_idf.end(); iter++)
	{
		outfile << iter->first<<":"<< iter->second << endl;
	}
	outfile.close();
}

