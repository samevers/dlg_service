#ifndef UTILS_HEAD_H__
#define UTILS_HEAD_H__

namespace SAMUTILS{

inline int abs_(int a, int b)
{
	return a<b ? (b-a):(a-b);
}

inline double abs_(double a, double b)
{
	return a<b ? (b-a):(a-b);
}

};

#endif
