#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>
#include "unicode.h"
#include <WinNls.h>
#include <OleAuto.h>

const string decodeUNI(const wstring& in)
{
	string ret;
	char *sTime=NULL;
	int nLen = WideCharToMultiByte(CP_ACP, 0, in.c_str(), -1, sTime, 0, NULL, NULL);
	sTime = new char[nLen+1];
	WideCharToMultiByte(CP_ACP, 0, in.c_str(), -1, sTime, nLen+1, NULL, NULL);
	ret=sTime;
	delete[] sTime;
	return ret;
}
const wstring encodeUNI(const string& in)
{
	wstring ret;
	WCHAR* bstr;
	int nLen = MultiByteToWideChar(CP_ACP, 0, in.c_str(), in.length(), NULL, NULL);
	bstr = SysAllocStringLen(NULL, nLen);
	MultiByteToWideChar(CP_ACP, 0, in.c_str(), in.length(), bstr, nLen);
	ret=bstr;
	SysFreeString(bstr); 
	return ret;
}
const string decodeUTF8toUNI(const wstring& in)
{
	string ret;
	char *sTime=NULL;
	int nLen = WideCharToMultiByte(CP_UTF8, 0, in.c_str(), -1, sTime, 0, NULL, NULL);
	sTime = new char[nLen+1];
	WideCharToMultiByte(CP_UTF8, 0, in.c_str(), -1, sTime, nLen+1, NULL, NULL);
	ret=sTime;
	delete[] sTime;
	return ret;
}
const wstring encodeUTF8toUNI(const string& in)
{
	wstring ret;
	string work = in;
	WCHAR* bstr;

	if(work[0] == -17
		&& work[1] == -69
		&& work[2] == -65
		)
		work = work.substr(3);

	int nLen = MultiByteToWideChar(CP_UTF8, 0, work.c_str(), work.length(), NULL, NULL);
	bstr = SysAllocStringLen(NULL, nLen);
	MultiByteToWideChar(CP_UTF8, 0, work.c_str(), work.length(), bstr, nLen);
	ret=bstr;
	SysFreeString(bstr); 
	return ret;
}
