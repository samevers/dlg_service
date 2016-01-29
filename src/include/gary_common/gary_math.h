#ifndef __GARY_MATH_H_
#define __GARY_MATH_H_

#include <cmath>

//using namespace std;

namespace spaceGary
{

//最大值
template <typename T>
inline
T GMax(T t1, T t2)
{
	if (t1 >= t2)
	{
		return t1;
	}
	else 
	{
		return t2;
	}
}

//最小值
template <typename T>
inline 
T GMin(T t1, T t2)
{
	if (t1 <= t2)
	{
		return t1;
	}
	else 
	{
		return t2;
	}
}

};

#endif


