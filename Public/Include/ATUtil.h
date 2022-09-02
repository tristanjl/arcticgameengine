#ifndef AT_UTIL_H
#define AT_UTIL_H

template<class T> void ATSwap(T &t1, T &t2)
{
	T tTemp = t1;
	t1 = t2;
	t2 = tTemp;
}

#endif // AT_UTIL_H
