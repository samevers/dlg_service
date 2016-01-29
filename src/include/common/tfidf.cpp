#include "tf-idf.h"

using namespace std;


int main(int argc, char** argv)
{
	TF_IDF tf_idf;
	string trainfile = argv[1];
	ifstream infile;
	infile.open(trainfile.c_str(), ios::in);
	if(!infile)
	{
		cerr << "[ERROR] Fail to open input file." << endl;
		return -1;
	}
	tf_idf.Train_data_count(infile);
	tf_idf.Tf_Idf_Count();

	string outputfile = trainfile + ".tf_idf";
	tf_idf.Output_Tf_Idf(outputfile);

	return 0;
}
