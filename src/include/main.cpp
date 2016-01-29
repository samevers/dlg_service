#include "EditDist.h"
#include <iostream>
using namespace std;

int main(int args, char* argv[])
{
	string str1 = argv[1];
	string str2 = argv[2];

	cout<< "str1 = " << str1 << endl;
	cout<< "str2 = " << str2 << endl;
	EditDist ed;
	cout << ed.Calculate(str1, str2) << endl;

	return 0;
}
