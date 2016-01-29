#include "EditDist.h"
#include <iostream>
#include "segment/Segment.h"
using namespace std;

int main(int args, char* argv[])
{
	string str1 = argv[1];
	string str2 = argv[2];

	cout<< "str1 = " << str1 << endl;
	cout<< "str2 = " << str2 << endl;
	EditDist ed;
	SEGMENT_1 *seg;
	seg = SEGMENT_1::getInstance();
	seg->Init();
	cout << ed.Calculate(str1, str2, seg) << endl;

	SEGMENT_1::releaseInstance();
	return 0;
}
