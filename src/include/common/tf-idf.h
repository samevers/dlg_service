#ifndef TF_IDF_HEAD_H__
#define TF_IDF_HEAD_H__

#include "general.h"
#include "include/gary_common/gary_string.h"

class TF_IDF{
	private:
		;
	public:
		int32_t num_doc;
		std::map<std::string, int32_t> tf;
		std::map<std::string, int32_t> df;
		std::map<std::string, double> idf;
		std::map<std::string, double> tf_idf;
	public:
		TF_IDF();
		~TF_IDF();

		int Train_data_count(std::ifstream& infile);
		int Tf_Idf_Count();
		int Output_Tf_Idf(std::string& outputfile);
};
#endif
