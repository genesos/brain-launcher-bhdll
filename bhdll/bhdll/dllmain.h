#pragma once
#include "stdafx.h"

typedef void (WINAPI * CBvoid)(void);
typedef void (WINAPI * CBint)(int);
typedef void (WINAPI * CBlpstr)(LPSTR);

void startSign();
void __fastcall checkSign(int playerNum, char * str);
void WINAPI GotTextCommand(LPSTR text);
wstring getAbsPath();
const string decodeUNI(const wstring& in);
string setSignHeader();
inline void CheckGameStatus(int GameStatus);
inline void CheckLobbyStatus(int LobbyStatus);
inline void CheckBattleNetStatus( int BattleNetStatus );
void WINAPI HookDrawStaticText();
wstring getBinaryOriginalName();
DWORD WINAPI threadMaphack(LPVOID lpParam);
bigint makeReturnSign(bigint in);
const char * getMyName();
void WINAPI HookDrawAfter();
LRESULT CALLBACK hookFunc(int code, WPARAM wParam, LPARAM lParam);
DWORD getWindowThreadID();
void __DRAWAFTERFUNCTIONDetour();
bool getHasUpdate();

void doOpenDisconnectDialog(bool noText = false);
void doCloseDisconnectDialog();
int httpRequest (const char* hostname, const char* uri, const char* parameters, string& message);
DWORD _GetThreadId(HANDLE Thread);
DWORD _GetThreadBaseAddress(HANDLE Thread);
DWORD _GetThreadPriority(HANDLE Thread);

extern HMODULE hModuleGlobal;
extern int bDisconnectDialog;
extern DWORD hMainThreadId;
extern HANDLE hMainThread;
