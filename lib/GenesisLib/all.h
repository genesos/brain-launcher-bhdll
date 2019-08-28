#pragma once

#pragma warning(disable : 4819)
#pragma warning(disable : 4996)
#pragma warning(disable : 4200)

#include "file.h"
#include "string.h"

#include <boost/typeof/typeof.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/bind.hpp>
#include <sstream>
#include <string>
#include <vector>
//#include <map>

using boost::shared_ptr;
using boost::bind;
using namespace std;

#define REP(i,to) for(int i=0,i##_end = (int)(to);i<=i##_end;i++)
#define RREP(i,to) for(int i=(to);i>=0;i--)

#define REPBY(i,to,by) for(int i=0,i##_end = (int)(to);i<=i##_end;i+=(by))
#define RREPBY(i,to,by) for(int i=(to);i>=0;i-=(by))

#define REPTO(i,from,to) for(int i=(from),i##_end = (int)(to);i<=i##_end;i++)
#define RREPTO(i,from,to) for(int i=(to);i>=(from);i--)

#define REPTOBY(i,from,to,by) for(int i=(from),i##_end = (int)(to);i<=i##_end;i+=(by))
#define RREPTOBY(i,from,to,by) for(int i=(to);i>=(from);i-=(by))

#define FOR_EACH(type, cont, it)	for (type::iterator it = cont.begin(), it##_end = cont.end(); it != it##_end; ++it)
#define FOR_EACH_IT(type, cont)		FOR_EACH(type, cont, it)

#define FOR_EACH_RIT(type, cont)	for (type::reverse_iterator rit = cont.rbegin(), rit##_end = cont.rend(); rit != rit##_end; ++rit)

#define EACH(i,xx2) REP(i,((int)(xx2).size()) - 1)
#define RANGE(it,itLow,itHigh)  for(auto it = itLow;it!=itHigh;++it)
#define BEGIN(it,x) for(auto it=(x).begin();it!=(x).end();++it)
#define RBEGIN(it,x) for(auto it=(x).rbegin(),;it!=(x).rend();++it)
#define CLR(i) memset(&i,0,sizeof(i))
template <class T>inline T sqr(T in){return in*in;}
template <class T,class T2>inline T diff(T in,T2 in2){return abs(in-in2);}
template <class T>inline pair<T,T>minmax(T a, T b){if(a<b)return make_pair(a,b);return make_pair(b,a);}
template<typename T>
inline T minAt(const T target,const T low)
{
	if(target>low)
		return target;
	return low;
}
template<typename T>
inline T maxAt(const T target,const T high)
{
	if(target<high)
		return target;
	return high;
}
template<typename T>
inline T range(const T low,const T target,const T high)
{
	return maxAt(minAt(target,low),high);
}
template<class T> 
inline T rangeLoop(T from, T input, T to)
{
	if(input < from)
	{
		int size = to - from + 1;
		int remind = (from - input - 1) % size;
		return to - remind;
	}
	else if(to < input)
	{
		int size = to - from + 1;
		int remind = (input - to - 1) % size;
		return from + remind;
	}
	else 
		return input;
}
template<class T> 
inline T divideCeil(T input, T divide)
{
	return (input + divide - 1) / divide;
}
template<typename T>
inline bool inRange(const T low,const T target,const T high)
{
	if(low > target)
		return false;
	if(target > high)
		return false;
	return true;
}
template <class T>
struct _set_pair{
public:
	T &a,&b;
	_set_pair(T&_a,T&_b):a(_a),b(_b){};
	inline const pair<T,T> operator=(const pair<T,T> in)const
	{
		a=in.first;
		b=in.second;
		return in;
	}
};
template <class T>
inline const _set_pair<T> set_pair(T&_a,T&_b){return _set_pair<T>(_a,_b);}
