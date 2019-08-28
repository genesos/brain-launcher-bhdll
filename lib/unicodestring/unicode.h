#pragma once

#include <string>
using namespace std;

#ifdef _UNICODE
typedef wstring tstring;
#else
typedef string tstring;
#endif

const wstring encodeUNI(const string& sTime);
const string decodeUNI(const wstring& in);
const wstring encodeUTF8toUNI(const string& sTime);
const string decodeUTF8toUNI(const wstring& in);