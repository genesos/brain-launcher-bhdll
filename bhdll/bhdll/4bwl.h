#pragma once

#define BWLAPI 4
#define STARCRAFTBUILD 13

/*  STARCRAFTBUILD
-1   All
0   1.04
1   1.08b
2   1.09b
3   1.10
4   1.11b
5   1.12b
6   1.13f
7   1.14
8   1.15
9	1.15.1
10	1.15.2
11	1.15.3
12	1.16.0
13	1.16.1
*/
struct ExchangeData
{
	int iPluginAPI;
	int iStarCraftBuild;
	bool bConfigDialog;                 //Is Configurable
	bool bNotSCBWmodule;                //Inform user that closing BWL will shut down your plugin
};

extern "C" __declspec(dllexport) void GetPluginAPI(ExchangeData &Data);
extern "C" __declspec(dllexport) void GetData(char *name, char *description, char *updateurl);
extern "C" __declspec(dllexport) bool OpenConfig();
extern "C" __declspec(dllexport) bool ApplyPatchSuspended(HANDLE hProcess, DWORD dwProcessID);
extern "C" __declspec(dllexport) bool ApplyPatch(HANDLE hProcess, DWORD dwProcessID);
extern "C" __declspec(dllexport) void CheckImgSignature(wchar_t *fileName, int *ret, wchar_t* retStr);
extern int isBrainLauncher;
