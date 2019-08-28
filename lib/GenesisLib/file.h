#pragma once

#include "../unicodestring/unicode.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include <sstream>
#include <fstream>
using namespace std;

extern int logRealFirst;

template<typename T>
inline void logReal(wstring func,const T& txt)
{
	wofstream out;
	if(logRealFirst)
	{
		out.open((getModuleAbsPath()+(L"\\log.txt")).c_str());
		logRealFirst=0;
	}
	else
	{
		out.open((getModuleAbsPath()+(L"\\log.txt")).c_str(),ios_base::out|ios_base::app);
	}

	out << L"[" << func << L"] "<< txt << endl;
	out.close();
}
template<>
inline void logReal<wstringstream>(wstring func,const wstringstream& txt)
{
	logReal(func,txt.str().c_str());
}

#define LOG(dirIndex) logReal(__FUNCTIONW__,(dirIndex))

std::wstring getModuleAbsPath(HMODULE target=NULL);
std::wstring getModuleExeName(HMODULE target=NULL);
std::wstring getModuleAbsFullPath(HMODULE target=NULL);
std::wstring getBinaryVersion(std::wstring fileName);

bool isFile(std::wstring fileName);
int fileSize(std::wstring fileName);