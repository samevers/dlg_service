#include "dialogue.h"
using namespace std;

const char* TEACHER_FILE = "../../data/LearnDlg/learn.log";

const char* dir_path = "../../";
int main(int arg, char** argv)
{
	DIALOGUE dlg;
	cerr << "[INFO] Begin to Init dlg" << endl;
	if(dlg.Init(dir_path) == -1)
	{
		cerr << "[ERROR] Fail to dlg.init()" << endl;
		return -1;
	}
	cerr << "[INFO] Init dlg OK" << endl;
	//string query = argv[1];
	string query;

	// Build a teacher file.
	/*ofstream FILE_TEACHER;
	FILE_TEACHER.open(TEACHER_FILE,ios::out);
	if(!FILE_TEACHER)
	{
		cerr << "[test.cpp][ERROR] Fail to build new learning file." << endl;
		return -1;
	}
	*/
	cerr << "[PLEASE INPUT A QUERY]:" << endl;

	while(true)
	{
		cerr << "[INPUT]:" << endl;
		while(cin >> query)
		{
			if(query == "exit")
			{
				cerr << "[THANKS FOR VISITING HUZI PARK]" << endl << "[WELCOME ANY TIME TO BACK]"<< endl << "[I'LL MISS U.]" << endl << "[C U ~]" << endl;
				return 0;
			}
			dlg.IndexOutcome(query.c_str());//, FILE_TEACHER);
		}
	}
	//FILE_TEACHER.close();

	return 0;
}
