#include "stdafx.h"

#include "file.h"
#include "all.h"

#include <string>
#include <tchar.h>
#include <boost/shared_ptr.hpp>
#include <sstream>
#include <iomanip>

using namespace std;

#pragma comment(lib,"version.lib")

wstring getBinaryVersion(wstring fileName)
{
	DWORD len,unused;
	if(len=GetFileVersionInfoSize(fileName.c_str(),&unused))
	{
		auto_ptr<WCHAR> versionTmp(new WCHAR[len+1]);
		if(GetFileVersionInfo(fileName.c_str(),0,len,versionTmp.get()))
		{
			VS_FIXEDFILEINFO* pFineInfo = NULL;
			UINT bufLen = 0;
			// buffer로 부터 VS_FIXEDFILEINFO 정보를 가져옵니다.
			if(VerQueryValue(versionTmp.get(),_T("\\"),(LPVOID*)&pFineInfo, &bufLen) !=0)
			{    
				WORD majorVer, minorVer, buildNum, revisionNum;
				majorVer = HIWORD(pFineInfo->dwFileVersionMS);
				minorVer = LOWORD(pFineInfo->dwFileVersionMS);
				buildNum = HIWORD(pFineInfo->dwFileVersionLS);
				revisionNum = LOWORD(pFineInfo->dwFileVersionLS);

				wstringstream ss;
				ss	<<majorVer<<"."
					<<minorVer<<"."
					<<buildNum<<"."
					<<revisionNum;
				return ss.str();
			}
		}
	}
	return _T("None");
}
int logRealFirst=1;
std::wstring getModuleAbsPath( HMODULE target )
{
	TCHAR Path[MAX_PATH];
	TCHAR ExePath[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Dir[_MAX_DIR];

	GetModuleFileName(target,ExePath,MAX_PATH);
	_wsplitpath(ExePath,Drive,Dir,NULL,NULL);
	wsprintf(Path,_T("%s%s"),Drive,Dir);

	return Path;
}
std::wstring getModuleExeName(HMODULE target)
{
	TCHAR Path[MAX_PATH];
	TCHAR fileName[MAX_PATH];
	TCHAR ext[MAX_PATH];

	GetModuleFileName(target,Path,MAX_PATH);
	_wsplitpath(Path,NULL,NULL,fileName,ext);
	wsprintf(Path,_T("%s%s"),fileName,ext);

	return Path;
}
std::wstring getModuleAbsFullPath(HMODULE target)
{
	TCHAR ExePath[MAX_PATH];

	GetModuleFileName(target,ExePath,MAX_PATH);

	return ExePath;
}
bool isFile(std::wstring fileName)
{
	std::ifstream file;
	file.open(fileName.c_str(), std::ios::in | std::ios::binary);
	return file.is_open();
}
int fileSize(std::wstring fileName)
{
	FILE* file = _wfopen(fileName.c_str(), L"rb");
	fpos_t pos;

	fseek(file, 0, SEEK_END);
	fgetpos(file, &pos);
	fclose(file);
	return (int)pos;
}
/*
DWORD fileGetCRC(std::wstring fileName)
{
	DWORD retDword;
	CCrc32Static::FileCrc32Streams(fileName.c_str(), retDword);
	return retDword;
}
*/
wstring urlEncode(std::string binary)
{
	std::wstringstream ret;
	REP(i, binary.length() - 1)
	{
		ret<<L"%";
		ret<<std::setfill(L'0')<<std::setw(2)<<std::hex<<(int)binary[i];
	}
	return ret.str();
}
std::string urlDecode(std::wstring binary)
{
	std::string ret;
	REPBY(i, binary.length() - 1, 3)
	{
		if(i+2 >= (int)binary.length())
			break;
		if(binary[i] != L'%')
			break;
		if(!isxdigit(binary[i + 1]))
			break;
		if(!isxdigit(binary[i + 2]))
			break;
		int hex;
		swscanf(binary.c_str()+i+1, L"%02X", &hex);
		if(!inRange(0, hex, 255))
			break;
		ret.push_back((char)((unsigned char)hex));
	}
	return ret;
}
