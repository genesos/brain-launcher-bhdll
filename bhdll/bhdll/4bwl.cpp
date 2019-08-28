#include "stdafx.h"

#include "4bwl.h"
#include "dllmain.h"
#include "dlltype.h"
#include "themida/ThemidaSDK/Include/c/ThemidaSDK.h"
#include "lib/CxImage/ximage.h"

#include "tchar.h"

int isBrainLauncher=0;
extern HMODULE hModuleGlobal;

extern "C" __declspec(dllexport) void GetPluginAPI(ExchangeData &Data)
{
	VM_START;
	if(Data.iPluginAPI==300 && Data.iStarCraftBuild==200)
		isBrainLauncher=1;
	VM_END;
	Data.iPluginAPI = BWLAPI;
	Data.iStarCraftBuild = STARCRAFTBUILD;
	Data.bConfigDialog = false;
	Data.bNotSCBWmodule = false;
	//BWL Gets version from Resource - VersionInfo
}

extern "C" __declspec(dllexport) void GetData(char *name, char *description, char *updateurl)
{
	//if necessary you can add Initialize function here
	//possibly check CurrentCulture (CultureInfo) to localize your DLL due to system settings
	stringstream ss;
	ss<<"[" << getDllHeader() << "] for 1.16.1 ";

	if(isBrainLauncher)
	{
		if(typeCheckMaphack)
		{
			strcpy(name,      ss.str().c_str());
			strcpy(description, (APPVER+"\r\n\r\nAuthor Genesis\r\n\r\nMINI Hack Checker.\r\n\r\n - F5 - Lag On/Off\r\n - PrintScreen - Screen Shot with PNG").c_str());
			strcpy(updateurl,   "http://mini.brainclan.com/bh/");
		}
		else
		{
			strcpy(name,      ss.str().c_str());
			strcpy(description, (APPVER + "\r\nAnti Hack Plug-in for any server").c_str());
			strcpy(updateurl,   "");
		}
	}
	else
	{
		strcpy(name,      ss.str().c_str());
		strcpy(description, "");
		strcpy(updateurl,   "");
	}
}
extern "C" __declspec(dllexport) bool OpenConfig()
{
	return true; //everything OK
}
HMODULE hModule;
extern "C" __declspec(dllexport) bool ApplyPatchSuspended(HANDLE hProcess, DWORD dwProcessID)
{
	if(!isBrainLauncher)
		return true;
	wstring dll;
	dll = getModuleAbsFullPath(hModuleGlobal);

	hModule = RemoteLoadLibraryNT(dwProcessID, dll.c_str());
	return true; //everything OK
}
extern "C" __declspec(dllexport) bool ApplyPatch(HANDLE hProcess, DWORD dwProcessID)
{
	if(!isBrainLauncher || hModule)
		return true;
	DWORD timeFrom=GetTickCount();
	wstring dll;

	dll = getModuleAbsFullPath(hModuleGlobal);
	hModule = RemoteLoadLibraryNT(dwProcessID, dll.c_str());

	while(hModule)
	{
		hModule = RemoteLoadLibraryNT(dwProcessID, dll.c_str());

		if(hModule)
		{
			/*
			DWORD p=GetLastError();
			WCHAR windows_error_string[256];
			FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM, 0, p, 0, windows_error_string, 255, NULL);
			MessageBox(0,windows_error_string,0,0);
			*/
			break;
		}

		HWND hWnd = ::FindWindow(_T("SWarClass"), NULL);
		if (hWnd == NULL)
			continue;
//		hModule = RemoteLoadLibrary(hWnd, dll.c_str(),10);
		if(hModule)
			break;

		if(GetTickCount()-timeFrom>15000)
			break;
	}
	return true;
}
extern "C" __declspec(dllexport) void CheckImgSignature(wchar_t *fileName, int* ret, wchar_t* retStr)
{
	retStr[0] = 0;
	*ret = 0;

	if(!fileName)
	{
		wcscpy(retStr, L"파일을 열 수가 없습니다. 파일을 확인해주세요");
		*ret = 0;
		return;
	}

	CxImage img;
	if(!img.Load(fileName, CXIMAGE_FORMAT_PNG))
	{
		wcscpy(retStr, L"파일을 열 수가 없습니다. 파일을 확인해주세요");
		*ret = 0;
		return;
	}
	if(img.GetBpp() != 24)
	{
		wcscpy(retStr, L"정상적으로 암호화된 *.MINI.PNG 파일이 아닙니다. 파일을 확인해주세요");
		*ret = 0;
		return;
	}
	if(img.GetWidth() != 640 || img.GetHeight() != 480)
	{
		wcscpy(retStr, L"정상적으로 암호화된 *.MINI.PNG 파일이 아닙니다. 파일을 확인해주세요");
		*ret = 0;
		return;
	}

	const BYTE first = 1;
	const BYTE deFirst = ~first;
	BYTE hex = 0;

	bool done = true;
	REP(y, img.GetHeight() - 1)
		REP(x, img.GetWidth() - 1)
	{
		RGBQUAD pixel = img.GetPixelColor(x, y);

		if(x % 500 == 0 && y % 100 == 0)
		{
			VM_START;
			hex = ~(hex ^ (((pixel.rgbRed ^ pixel.rgbGreen ^ pixel.rgbBlue) & deFirst) >> ((x + y * 5) % 7)));
			VM_END;
		}
		else if(x % 500 == 4 && y % 100 == 4)
		{
			CODEREPLACE_START;
			hex = ~(hex ^ (((pixel.rgbRed ^ pixel.rgbGreen ^ pixel.rgbBlue) & deFirst) >> ((x + y * 5) % 7)));
			CODEREPLACE_END;
		}
		else
			hex = ~(hex ^ (((pixel.rgbRed ^ pixel.rgbGreen ^ pixel.rgbBlue) & deFirst) >> ((x + y * 5) % 7)));
		if((pixel.rgbGreen & first) != (hex & first))
			done = false;
	}
	if(done)
	{
		wcscpy(retStr, L"캡춰가 변조, 위조되지 않았습니다!");
		*ret = 0;
	}
	else
	{
		wcscpy(retStr, L"캡춰가 위조되어 있습니다!");
		*ret = 0;
	}
	*ret = 1;
}
