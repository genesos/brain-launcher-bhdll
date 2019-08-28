#include "stdafx.h"

#include "starLibrary.h"
#include "dllmain.h"

int GetPlrRace(int id)
{
	return *(char *)(36 * id + 0x57EF0E);//0x57EF0E
}
int GetSelectedPlayerID()
{
	int result; // eax@2
	char **ptr=(char**)0x597208;//0x5971f0

	if ( *ptr)
		result = *(char *)((*ptr) + 76);
	else
		result = 11;

	return result;
}
int getMyID()
{
	int myId=-1;
	REP(i,7)
		if(string(getMyName())==string(GetPlrName(i)))
			myId=i;
	return myId;
}
const char * getMyName()
{
	char const *myName=(char*)0x0057ee9c;//0x0057ee9c
	return myName;
}
DWORD getWindowThreadID()
{
	HWND Window=::FindWindow(_T("SWarClass"),0);
	DWORD WindowThreadId  =0;
	if(Window)
		return GetWindowThreadProcessId(Window,0);
	return 0;
}
LPCSTR GetPlrName(int plr_id)
{
	const char *target=(const char *)0x57EEEB;
	return target+plr_id*36;
}
bool IsSC()
{
	return getBinaryOriginalName()==_T("StarCraft.exe");
}
wstring getServer()
{
	CRegistry regMyReg;
	regMyReg.Open( _T("Software\\Battle.net\\Configuration"), HKEY_CURRENT_USER );
	CRegEntry &reg=regMyReg[_T("Battle.net Gateways")];

	if(reg.IsBinary())
	{
		int len = reg.GetBinaryLength();
		auto_ptr<BYTE> b(new BYTE[len]);
		auto_ptr<wchar_t> bn(new wchar_t[len]);
		reg.GetBinary(b.get(), len);
		REP(i, len - 1)
			bn.get()[i] = (wchar_t)b.get()[i];
		reg.SetMulti(bn.get(), len);
	}

	bool bb=reg.IsMultiString();
	if(bb)
	{
		int len=reg.MultiCount();
		if(len>=2)
		{
			int idx;
			wstring idxStr(reg.MultiGetAt(1));

			wstringstream ss;
			ss<<idxStr.c_str();
			ss>>idx;
			idx--;

			if(len-1>=2+3*idx)
				return reg.MultiGetAt(2+3*idx);
			if(len >= 3)
				return reg.MultiGetAt(3);
		}
	}
	return _T("");
}
wstring getBinaryOriginalName()
{
	wstring in;
	in=getAbsFullPath();
	DWORD len,unused;
	if(len=GetFileVersionInfoSize(in.c_str(),&unused))
	{
		auto_ptr<WCHAR> versionTmp(new WCHAR[len+1]);
		if(GetFileVersionInfo(in.c_str(),0,len,versionTmp.get()))
		{
			UINT *pVersion;
			UINT bufLen = 0;
			TCHAR *retBuf,queryStr[256];
			VerQueryValue(versionTmp.get(),_T("\\VarFileInfo\\Translation"),(LPVOID*)&pVersion, &bufLen);
			wsprintf(queryStr, _T("\\StringFileInfo\\%04x%04x\\OriginalFileName"), LOWORD(*pVersion), HIWORD(*pVersion));
			if(VerQueryValue(versionTmp.get(), queryStr, (LPVOID*)&retBuf, (UINT*)&bufLen)!=0)
			{
				return wstring(retBuf);
			}
		}
	}
	return _T("None");
}
bool isInGame()
{
	return (*(int*)0x6556e0 || *(int*)0x68C140);//0x68C128 
}
bool isInBattleNet()
{
	bool battleLogin = false;
	const char *battleUserName = (char*) 0x190452c8;
	if(!IsBadReadPtr((void*)battleUserName, 1))
		battleLogin = *battleUserName;
	return battleLogin;
}
string getBattleNetUserID()
{
	const char *battleUserName = (char*) 0x190452c8;
	if(!IsBadReadPtr((void*)battleUserName, 1))
		return battleUserName;
	return string();
}
bool isInLobby()
{
	return *(int*)0x596890;
}
wstring getAbsFullPath()
{
	TCHAR Path[MAX_PATH];
	TCHAR ExePath[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Dir[_MAX_DIR];

	GetModuleFileName(0,ExePath,MAX_PATH);

	return ExePath;
}
std::wstring getAbsPath()
{
	TCHAR Path[MAX_PATH];
	TCHAR ExePath[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Dir[_MAX_DIR];

	GetModuleFileName(hModuleGlobal,ExePath,MAX_PATH);
	_wsplitpath(ExePath,Drive,Dir,NULL,NULL);
	wsprintf(Path,_T("%s%s"),Drive,Dir);

	return Path;
}
