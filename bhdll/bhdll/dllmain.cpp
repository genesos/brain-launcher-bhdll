// dllmain.cpp : DLL 응용 프로그램의 진입점을 정의합니다.

#include "stdafx.h"

#include "detours.h"
#include "dllmain.h"
#include "bigint.h"
#include "../../themida/ThemidaSDK/Include/c/ThemidaSDK.h"

#include "4bwl.h"
#include "starLibrary.h"
#include "udpHook.h"
#include "patch.h"
#include "lib_star_capture.h"
#include "dlltype.h"

//#include "lib/wdetector/wDetectorF.h"
#ifndef _EX
//#pragma comment(lib,"wDetector_kor.lib")
#else
//#pragma comment(lib,"wDetector.lib")
#endif

#include <tlhelp32.h>
#include <fstream>
#include <Winternl.h>
#include <boost/thread/condition.hpp>

typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID;
typedef LONG    KPRIORITY;
typedef struct _THREAD_BASIC_INFORMATION {

	NTSTATUS ExitStatus; PVOID TebBaseAddress; CLIENT_ID ClientId; KAFFINITY AffinityMask; KPRIORITY Priority; KPRIORITY BasePriority;

} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

LONG (WINAPI *_NtQueryInformationThread)(HANDLE, LONG, PVOID, ULONG, PULONG) = 0;

#pragma comment(lib,"version.lib")

HMODULE hHelper;
HMODULE hModuleGlobal;

deque<pair<string, DWORD> > appOnUsers;

const int PLAYER_COUNT          =  12;
const int  RACE_COUNT           =   3;
struct PlayerResources
{
	struct PlayerResource
	{
		int player[PLAYER_COUNT];
	};
	PlayerResource minerals;
	PlayerResource gas;
};
static PlayerResources* BWXFN_PlayerResources = (PlayerResources*) 0x0057F0F0;
struct Supplies
{
	struct SuppliesPerRace
	{
		struct SupplyValues
		{
			int player[PLAYER_COUNT];
		};
		SupplyValues available;
		SupplyValues used;
		SupplyValues max;
	};
	SuppliesPerRace race[RACE_COUNT];
};
static Supplies* BWXFN_Supplies    = (Supplies*) 0x00582144;

template<typename T1>
void doLog(T1 a1)
{
#ifdef _DEBUG
	std::wofstream of(wstring(getAbsPath()+_T("brain.bwl.log.txt")).c_str(),ios_base::app);
	of<<a1;
	of<<endl;
#endif
}
template<typename T1,typename T2>
void doLog(T1 a1,T2 a2)
{
#ifdef _DEBUG
	std::wofstream of(wstring(getAbsPath()+_T("brain.bwl.log.txt")).c_str(),ios_base::app);
	of<<a1;
	of<<a2;
	of<<endl;
#endif
}
template<typename T1,typename T2,typename T3>
void doLog(T1 a1,T2 a2,T3 a3)
{
#ifdef _DEBUG
	std::wofstream of(wstring(getAbsPath()+_T("brain.bwl.log.txt")).c_str(),ios_base::app);
	of<<a1;
	of<<a2;
	of<<a3;
	of<<endl;
#endif
}
template<typename T1,typename T2,typename T3,typename T4>
void doLog(T1 a1,T2 a2,T3 a3,T4 a4)
{
#ifdef _DEBUG
	std::wofstream of(wstring(getAbsPath()+_T("brain.bwl.log.txt")).c_str(),ios_base::app);
	of<<a1;
	of<<a2;
	of<<a3;
	of<<a4;
	of<<endl;
#endif
}

/*
int *IsReplay = (int *)0x006D0EFC;
int *pPause_MultiPlayer = (int *)0x006509AC;
BYTE * bPause_SinglePlayer = (BYTE *)0x006D120C; 
int *IsMultiPlayer = (int *)0x0057F098;

int GetReplayTime()
{
	__asm	{
		MOVZX ECX,BYTE PTR DS:[0x006D0F52]
		MOV EDX,DWORD PTR DS:[ECX*4+0x004FF90C]
		IMUL EDX,DWORD PTR DS:[0x0057F224]
		MOV EAX,0x10624DD3
			MUL EDX
			MOV ECX,EDX
			SHR ECX,6
			MOV EAX,ECX
	}
}
*/

void (__stdcall*DrawRect)(DWORD x,DWORD y,DWORD w,DWORD h,BYTE clr);
void (*BWPubTextOut)(const char* text,DWORD time);
int (*PGetSelectedPlayerID)();
void (*AddEvent_Actions)(CBint evnt);
void (*AddEvent_DrawStaticText)(CBvoid evnt);
void (*AddEvent_TextCommand)(CBlpstr event);
void (*AddEvent_GameStatus)(CBint event);

inline void hookAll();
inline void dehookAll();

struct blockData
{
	unsigned char *addr;
	vector<unsigned char> dat;
	vector<unsigned char> modifiedDat;
};
struct diffData
{
	vector<blockData> blocks;
	string hackName;
};

typedef boost::mutex::scoped_lock Lock;

vector<diffData> diffs;
vector<string> txtList, txtListBuffer;

void * __DRAWAFTERFUNCTIONDetourPtr=(void*)__DRAWAFTERFUNCTIONDetour;

namespace{
	boost::mutex M, MBuffer;
};
const DWORD defaultFont = 0x006CE0F4;

string mySign;
string mySignNeed;


//Static text
void SCDrawText(const char *text,int x1,int y1,DWORD font)
{	
	static DWORD Fn1 = 0x0041FB30;//0x0041FB50//0x0041FB20
	static DWORD Fn2 = 0x004202B0;//0x004202D0//0x004202A0
	__asm
	{					
		MOV EAX,font//0x006CE0DC
		MOV ECX,DWORD PTR DS:[EAX]
		CALL Fn1

		MOV EAX,DWORD PTR DS:[text]
		MOV ESI,x1
		PUSH y1
		CALL Fn2
	}
}

inline unsigned char *getPtr(const unsigned char * &fp)
{
	int tmp=0;
	for(int i=0;i<4;i++)
	{
		tmp<<=8;
		tmp|=(*fp ^ 0x7b);
		advance(fp,1);
	}
	return (unsigned char *)tmp;
}
inline unsigned char getByte(const unsigned char * &fp)
{
	unsigned char ret=*fp;
	advance(fp,1);
	return ret ^ 0x7b;
}
inline int getInt(const unsigned char * &fp)
{
	int tmp=0;
	for(int i=0;i<4;i++)
	{
		tmp<<=8;
		tmp|=(*fp ^ 0x7b);
		advance(fp,1);
	}
	return tmp;
}

static DWORD (WINAPI*RealGetTickCount)(VOID)=GetTickCount;

static void (* __TXTFUNCTION) (void)=
(void (*)(void))0x0048D1C0;//0x0048CE90//0x0048CE60
static void (* __DRAWFUNCTION) (void)=
(void (*)(void))0x0048cF60;//0x0048cc30//0x0048CE60
static void (* __DRAWAFTERFUNCTION) (void)=
(void (*)(void))0x0048d517;
//(void (*)(void))0x004d9530;


inline void printRealWithKeyboard(const char*input)
{
	//const int BWFXN_CreateUnit = 0x0048CE60 ;
	const int BWFXN_CreateUnit = 0x004f3280;//0x004f2f20//0x004F2EC0
	char* FLAG = (char*)0x68C144  ;//0x68C12C
	char tmp;
	if(isInGame())
	{
		tmp=*FLAG;

		*FLAG=2;
		__asm
		{
			MOV eax,input
			call dword ptr ds:[BWFXN_CreateUnit]
		}

		*FLAG=tmp;
	}
}
inline void sendTextInLobbylobby(const char*input)
{
	const int BWFXN_SendLobbyCallTarget = 0x004707D0;
	if(isInLobby())
	{
		__asm
		{
			pushad;
			mov edi, input;
			call [BWFXN_SendLobbyCallTarget];
			popad;
		}
	}
}
inline void printWithKeyboard(const char* in)
{
	Lock a(MBuffer);
	txtListBuffer.push_back(in);
}

__declspec(naked)
void __TXTFUNCTIONDetour()
{
	static int a;
	static char * b;

	__asm{
		mov a, eax;
		mov b, esi;
	}

	if(typeCheckMaphack && b && !IsBadReadPtr((void*)b, 1) && b[0])
	{
		checkSign(a,b);
		if(b && !IsBadReadPtr((void*)b, 1) && b[0])
		{
			__asm
			{
				mov eax,a;
				mov esi,b;
				jmp __TXTFUNCTION;
			}
		}
		else
			__asm retn 8; 
	}
	else
	{
		__asm
		{
			mov eax,a;
			mov esi,b;
			jmp __TXTFUNCTION;
		}
	}
}
 __declspec(naked)
void __DRAWFUNCTIONDetour()
{
	HookDrawStaticText();
	__asm
	{
		jmp __DRAWFUNCTION;
	}
}
__declspec(naked)
void __DRAWAFTERFUNCTIONDetour()
{
	HookDrawAfter();
	__asm
	{
		ret;
	}
}

inline void killStarcraft()
{
	PostMessage(FindWindow(_T("SWarClass"), NULL),WM_QUIT,NULL,NULL);
	Sleep(500);
	PostMessage(FindWindow(_T("SWarClass"), NULL),WM_CLOSE,NULL,NULL);
	Sleep(500);
	PostMessage(FindWindow(_T("SWarClass"), NULL),WM_DESTROY,NULL,NULL);
	Sleep(500);

	TerminateProcess(GetCurrentProcess(), 0);
}

inline bool check(string &name)
{
	int ret=0;
	vector<string> maybe;
	vector<string> exact;

	for(unsigned int i=0;i<diffs.size();i++)
	{
		int cnt=0;
		int diffCnt=0;
		int bMaybe=0;
		int bExact=1;

		diffData &diff=diffs[i];
		for(unsigned int j=0;j<diff.blocks.size();j++)
		{
			blockData &block=diff.blocks[j];
			unsigned char *addr=block.addr;
			for(unsigned int k=0;k<block.dat.size();k++)
			{
				if(bExact)
					if(*addr!=block.modifiedDat[k])
						bExact=0;
				if(*addr!=block.dat[k])
					diffCnt++;
				addr++;
				cnt++;
			}
		}
		if(cnt<=diffCnt*2)
		{
			bMaybe=1;
		}
		if(bExact)
		{
			name=diff.hackName;
			ret=2;
		}
		if(bMaybe)
		{
			name=diff.hackName;
			ret=1;
		}
	}
	return (bool)ret;
}

const volatile bool absTrue = true;
volatile bool absVola = true;
inline DWORD WINAPI threadCheckGameStatus(LPVOID lpParam)
{
	static int bOld=0;
	static int bLobbyOld = 0;
	static int bBattleNetOld = 0;
	{
		/*
		static int p = 1;
		if(++p % 100 == 0)
		{
			int a;
			CHECK_PROTECTION(a, 1);
			if(a != 1)
			{
				killStarcraft();
			}
		}
		*/

		int bNow=isInGame();
		if(bOld!=bNow)
		{
			bOld=bNow;
			CheckGameStatus(bOld);
		}
		int bLobbyNow = isInLobby();
		if(bLobbyOld!=bLobbyNow)
		{
			bLobbyOld=bLobbyNow;
			CheckLobbyStatus(bLobbyOld);
		}
		int bBattleNetNow = isInBattleNet();
		if(bBattleNetOld != bBattleNetNow)
		{
			bBattleNetOld = bBattleNetNow;
			CheckBattleNetStatus(bBattleNetOld);
		}
		//if(!FindWindow(0,_T("miniLauncher")))
		//	ExitProcess(0);

//		Sleep(1000);
	}
	return 0;
}

void printNewUpdate();
DWORD WINAPI threadMaphack(LPVOID lpParam)
{
	VM_START;
	if(typeCheckMaphack)
	{
		hMainThreadId = GetCurrentThreadId();
		const char *fp=PATCH_DATA;
		while(1)
		{
			diffData diff;
			REP(i, 256)
			{
				if(*(const char*)fp == 0x7b)
				{
					advance(fp,1);
					break;
				}
				diff.hackName.push_back(getByte((const unsigned char*&)fp));
			}
			if(diff.hackName.length() <= 0)
				break;
			do 
			{
				blockData block;
				block.addr=getPtr((const unsigned char*&)fp);
				if(block.addr == NULL)
					break;

				int len=getInt((const unsigned char*&)fp);
				REP(i,len-1)
					block.dat.push_back(getByte((const unsigned char*&)fp));
				REP(i,len-1)
					block.modifiedDat.push_back(getByte((const unsigned char*&)fp));

				diff.blocks.push_back(block);
			} while (1);
			diffs.push_back(diff);
		}
	}
	SetThreadPriority(GetCurrentThread(), -2);

	while(absTrue)
	{
		int ret;
		string tmp;
		/*
		doLog(_T("conn() : 1"));
		if(!conn.isCreated())
		{
			doLog(_T("conn() : 2"));
			conn.Create(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		}
		if(conn.isCreated()&&!conn.isConnected())
		{
			doLog(_T("conn() : 3"));
			conn.Connect("115.68.2.13",5000);
		}
		*/

		if(typeCheckMaphack)
		{
			if(ret=check(tmp))
			{
				stringstream ss;
				if(ret==1)
					ss<<"[" << getDllHeader() << "] HACK Founded, Maybe '"<<tmp<<"'";
				else
					ss<<"[" << getDllHeader() << "] HACK Founded, '"<<tmp<<"'";
				printWithKeyboard("/hack");
				printWithKeyboard("/quit");
				printWithKeyboard(ss.str().c_str());
				Sleep(2000);
				killStarcraft();
			}
		}
		threadCheckGameStatus(0);
		Sleep(500);
		printNewUpdate();
		{
			Lock a(MBuffer);
			Lock b(M);
			BEGIN(it, txtListBuffer)
				txtList.push_back(*it);
			txtListBuffer.clear();
		}
	}
	VM_END;
	return 0;
}

inline void CheckGameStatus( int GameStatus )
{
	switch(GameStatus)
	{
	case 0:
		if(!isInBattleNet())
			dehookAll();
		doOpenDisconnectDialog(true);
		break;
	case 1:
		if(!isInBattleNet())
			hookAll();
		appOnUsers.clear();

		Sleep(3000);

//		printWithKeyboard(setSignHeader().c_str());
		startSign();

		break;
	}
}
inline void CheckLobbyStatus( int LobbyStatus )
{
	switch(LobbyStatus)
	{
	case 0:
		break;
	case 1:
		Sleep(2000);
		sendTextInLobbylobby(APPVER.c_str());
		break;
	}
}
void hex2byte(const char *in, unsigned int len, byte *out)
{
	for (unsigned int i = 0; i < len; i+=2) {
		char c0 = in[i+0];
		char c1 = in[i+1];
		byte c = (
			((c0 & 0x40 ? (c0 & 0x20 ? c0-0x57 : c0-0x37) : c0-0x30)<<4) |
			((c1 & 0x40 ? (c1 & 0x20 ? c1-0x57 : c1-0x37) : c1-0x30))
			);
		out[i/2] = c;
	}
}

inline void CheckBattleNetStatus( int BattleNetStatus )
{
	switch(BattleNetStatus)
	{
	case 0:
		dehookAll();
		break;
	case 1:
		hookAll();

		wstring server = getServer();
		if(typeCheckMaphack)
		if(server == L"lemon.brainclan.com"
			|| server == L"58.180.42.12")
		{
			byte key[CryptoPP::AES::DEFAULT_KEYLENGTH];
			memset(key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH );
			byte iv[CryptoPP::AES::BLOCKSIZE];
			memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE );
			char* rawKey="f4150d4a1ac5708c29e437749045a39a";
			hex2byte(rawKey, strlen(rawKey), key);

			char* rawIv="2811da22377d62fcfdb02f29aad77d9e";
			hex2byte(rawIv, strlen(rawIv), iv);

			stringstream ss;
			ss << "," << time(0) << "," << APPVER;
			std::string plaintext = string("1,") + getBattleNetUserID() + ss.str();
			std::string ciphertext;
			std::string base64encodedciphertext;
			std::string decryptedtext;
			std::string base64decryptedciphertext;

			unsigned int plainTextLength = plaintext.length();

			CryptoPP::AES::Encryption 
				aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
			CryptoPP::CBC_Mode_ExternalCipher::Encryption 
				cbcEncryption(aesEncryption, iv);

			CryptoPP::StreamTransformationFilter 
				stfEncryptor(cbcEncryption, new CryptoPP::StringSink( ciphertext));
			stfEncryptor.Put(reinterpret_cast<const unsigned char*>
				(plaintext.c_str()), plainTextLength + 1);
			stfEncryptor.MessageEnd();

			CryptoPP::StringSource(ciphertext, true,
				new CryptoPP::Base64Encoder(
				new CryptoPP::StringSink(base64encodedciphertext)
				) // Base64Encoder
				); // StringSource

			string uri = string("/mini/?v=") + base64encodedciphertext;
			string ret;
			httpRequest("lemon.brainclan.com", uri.c_str(), "", ret);
			//=> base64encodedciphertext
		}
		break;
	}
	return;
}
void ungetThreadHandleAndID(const vector<pair<HANDLE,DWORD> > &in)
{
	BEGIN(it,in)
		CloseHandle(it->first);
}
vector<pair<HANDLE,DWORD> > getThreadHandleAndID()
{
	vector<pair<HANDLE,DWORD> > ret;

	HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
	THREADENTRY32 te32; 

	hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
	if ( hThreadSnap == INVALID_HANDLE_VALUE ) 
		return ret; 

	te32.dwSize = sizeof(THREADENTRY32 ); 

	if ( !Thread32First( hThreadSnap, &te32 ) ) {
		CloseHandle( hThreadSnap );    // Must clean up the
		return ret ;
	}

	do { 
		if ( te32.th32OwnerProcessID == GetCurrentProcessId() )
		{
			HANDLE threadHandle = OpenThread(THREAD_ALL_ACCESS,true,te32.th32ThreadID);
			if (threadHandle)
			{
				ret.push_back(make_pair(threadHandle,te32.th32ThreadID));
			}
		}
	} while( Thread32Next(hThreadSnap, &te32 ) ); 

	CloseHandle( hThreadSnap );
	return ret;
}
void SuspendAllThread()
{
	DWORD WindowThreadId = getWindowThreadID();

	vector<pair<HANDLE,DWORD> > threadHandles = getThreadHandleAndID();
	BEGIN(it,threadHandles)
	{
		if(it->second == WindowThreadId)
			continue;
		if(it->second == GetCurrentThreadId())
			continue;
		SuspendThread(it->first);
	}
	ungetThreadHandleAndID(threadHandles);
}

void ResumeAllThread()
{
	vector<pair<HANDLE,DWORD> > threadHandles = getThreadHandleAndID();
	BEGIN(it,threadHandles)
		ResumeThread(it->first);
	ungetThreadHandleAndID(threadHandles);
}
HHOOK khook = 0;
LRESULT CALLBACK KeyboardProc(      
							  int code,
							  WPARAM wParam,
							  LPARAM lParam
							  )
{
	if(code >= 0 && wParam == VK_F5 && lParam&0x80000000)
	{
		if(bDisconnectDialog)
			doCloseDisconnectDialog();
		else if(!bDisconnectDialog)
			doOpenDisconnectDialog();
	}
	if(code >= 0 && lParam&0x80000000)
	{
		if(hMainThread && (ResumeThread(hMainThread) != 0 || _GetThreadBaseAddress(hMainThread) == 0 || GetThreadPriority(hMainThread) < -2))
		{
			VM_START;
			killStarcraft();
			VM_END;
		}
	}
	return CallNextHookEx(khook, code, wParam, lParam);
}

WNDPROC pfnOrigBFFTreeProc  = NULL;
long CALLBACK
BFFTreeSubclProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(uMsg == WM_CONTEXTMENU)
	{
		return false;
	}
	if( uMsg == WM_INITMENUPOPUP ||uMsg == WM_INITMENU ) {
		HMENU hMenuPopup = (HMENU)wParam;
		MENUINFO menuInfo;
		GetMenuInfo(hMenuPopup, &menuInfo);

		menuInfo.fMask |= MIM_STYLE | MIM_APPLYTOSUBMENUS;
		menuInfo.dwStyle |= MNS_AUTODISMISS;
		menuInfo.dwStyle |= MNS_MODELESS;

		SetMenuInfo(hMenuPopup, &menuInfo);
		DestroyMenu(hMenuPopup);
		//return 0;
	}
	return CallWindowProc( pfnOrigBFFTreeProc, hWnd, uMsg, wParam, lParam );
}
HANDLE hMainThread = 0;
DWORD hMainThreadId = 0;

static BOOL (WINAPI *pTerminateThread_Original)(
	 __in HANDLE hThread,
	 __in DWORD dwExitCode
	 ) = TerminateThread;
BOOL WINAPI pTerminateThread_Hook(
	 __in HANDLE hThread,
	 __in DWORD dwExitCode
	 )
{
	BOOL ret;
	VM_START;
	if(GetCurrentThreadId() != hMainThreadId)
		if(_GetThreadId(hThread) && hMainThreadId && _GetThreadId(hThread) == hMainThreadId)
			return rand() % 2;
	ret =  pTerminateThread_Original(hThread, dwExitCode);
	if(absTrue)
		return ret;
	VM_END;
	return true;
}

static BOOL (WINAPI *pSetThreadPriority_Original)(
	 __in HANDLE hThread,
	 __in int nPriority
	 ) = SetThreadPriority;
BOOL WINAPI pSetThreadPriority_Hook(
	 __in HANDLE hThread,
	 __in int nPriority
	 )
{
	BOOL ret;
	VM_START;
	if(GetCurrentThreadId() != hMainThreadId)
		if(_GetThreadId(hThread) && hMainThreadId && _GetThreadId(hThread) == hMainThreadId)
			return rand() % 2;
	ret = pSetThreadPriority_Original(hThread, nPriority);
	if(absTrue)
		return ret;
	VM_END;
	return true;
}

static DWORD (WINAPI *pSuspendThread_Original)(
	 __in HANDLE hThread
	 ) = SuspendThread;
DWORD WINAPI pSuspendThread_Hook(
	 __in HANDLE hThread
	 )
{
	DWORD ret;
	VM_START;
	if(GetCurrentThreadId() != hMainThreadId)
		if(_GetThreadId(hThread) && hMainThreadId && _GetThreadId(hThread) == hMainThreadId)
			return rand() % 2;
	ret = pSuspendThread_Original(hThread);
	if(absTrue)
		return ret;
	VM_END;
	return true;
}
DWORD _GetThreadId(HANDLE Thread)
{
	THREAD_BASIC_INFORMATION tbi;
	NTSTATUS status;
	if(!_NtQueryInformationThread)
		_NtQueryInformationThread = (LONG (WINAPI *)(HANDLE, LONG, PVOID, ULONG, PULONG))GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryInformationThread");
	if(_NtQueryInformationThread)
	{
		status = _NtQueryInformationThread(Thread, (THREADINFOCLASS)0, &tbi, sizeof(tbi), NULL);
		if (status)
		{
		}
		return HandleToULong(tbi.ClientId.UniqueThread);
	}
	return 0;
}
DWORD _GetThreadBaseAddress(HANDLE Thread)
{
	THREAD_BASIC_INFORMATION tbi;
	NTSTATUS status;
	if(!_NtQueryInformationThread)
		_NtQueryInformationThread = (LONG (WINAPI *)(HANDLE, LONG, PVOID, ULONG, PULONG))GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryInformationThread");
	if(_NtQueryInformationThread)
	{
		status = _NtQueryInformationThread(Thread, (THREADINFOCLASS)0, &tbi, sizeof(tbi), NULL);
		if (NT_SUCCESS(status))
		{
			return (DWORD)tbi.TebBaseAddress;
		}
	}
	return 0;
}
DWORD _GetThreadPriority(HANDLE Thread)
{
	THREAD_BASIC_INFORMATION tbi;
	NTSTATUS status;
	if(!_NtQueryInformationThread)
		_NtQueryInformationThread = (LONG (WINAPI *)(HANDLE, LONG, PVOID, ULONG, PULONG))GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryInformationThread");
	if(_NtQueryInformationThread)
	{
		status = _NtQueryInformationThread(Thread, (THREADINFOCLASS)0, &tbi, sizeof(tbi), NULL);
		if (status)
		{
		}
		return (DWORD)tbi.Priority;
	}
	return 0;
}

deque<LPTHREAD_START_ROUTINE> lpfunc;
deque<LPVOID> lpfuncArg;
static void (* __STARMAINFUNCTION) (void)=
(void (*)(void))0x00405F5B;

static boost::mutex __STAR_M;
void __fastcall __STARMAINFUNCTIONDetour2()
{
	LPTHREAD_START_ROUTINE _lpfunc;
	LPVOID _lpfuncArg;

	{
		Lock a(__STAR_M);

		_lpfunc = lpfunc.front();
		_lpfuncArg = lpfuncArg.front();
		lpfunc.pop_front();
		lpfuncArg.pop_front();
	}

	_lpfunc(_lpfuncArg);
}
__declspec(naked)
void __STARMAINFUNCTIONDetour()
{
	__STARMAINFUNCTIONDetour2();
}
//#include <process.h>

inline HANDLE CreateThreadFakeStartAddress(LPTHREAD_START_ROUTINE lpfunc_, LPVOID arg_)
{
	return CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)lpfunc_,arg_,NULL,NULL);
	{
		Lock a(__STAR_M);

		lpfunc.push_back(lpfunc_);
		lpfuncArg.push_back(arg_);
	}

//	return (HANDLE)_beginthread((void (__cdecl*)(void*))0x00405F5B, 0, 0);
	return CreateThread(0, 0, (LPTHREAD_START_ROUTINE)0x00405F5B, 0, 0, 0);
}

inline void WINAPI threadNULLImpl(LPVOID lpParam)
{
	VM_START;
	SetThreadPriority(GetCurrentThread(), -2);
	HANDLE handle = (HANDLE)lpParam;
	volatile int p = 1;
	while(p)
	{
		//wstringstream wss;
		//wss<<_GetThreadId(handle);
		//MessageBox(0,wss.str().c_str(),0,0);
		
		if(handle && (ResumeThread(handle) != 0 || _GetThreadBaseAddress(handle) == 0 || GetThreadPriority(handle) < -2))
		{
			killStarcraft();
		}
		Sleep(1000);
	}
	VM_END;
	return;
}
volatile DWORD WINAPI threadNULL(LPVOID lpParam)
{
	threadNULLImpl(lpParam);
	return 0;
}
volatile DWORD WINAPI threadNULL2(LPVOID lpParam)
{
	threadNULLImpl(lpParam);
	return 1;
}
volatile DWORD WINAPI threadNULL3(LPVOID lpParam)
{
	threadNULLImpl(lpParam);
	return 2;
}
volatile DWORD WINAPI threadNULL4(LPVOID lpParam)
{
	threadNULLImpl(lpParam);
	return 3;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	hModuleGlobal=hModule;
	if(!IsSC())
	{
		DisableThreadLibraryCalls(hModule);
		DllMain2(hModule,ul_reason_for_call,lpReserved);
	}
	else
	{
//		wDetectorDllMain(hModule,ul_reason_for_call,lpReserved);
	}
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		if(!IsSC())
		{
#ifndef _DEBUG
			//llkhook=SetWindowsHookEx(WH_KEYBOARD_LL,LowLevelKeyboardProc,hModule,0);
#endif
			return true;
		}
		else
		{
			VM_START;

			if(typeCheckMaphack)
			{
				//DetourRestoreAfterWith();	
				//DetourTransactionBegin();
				//DetourAttach(&(PVOID&)__STARMAINFUNCTION,(PVOID)__STARMAINFUNCTIONDetour);
				//DetourTransactionCommit();


				HANDLE prevHandle;
				hMainThread = CreateThreadFakeStartAddress((LPTHREAD_START_ROUTINE)threadMaphack,(LPVOID)0);

				//hMainThread = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)threadMaphack,NULL,NULL,NULL);

				prevHandle = hMainThread;
				prevHandle = CreateThreadFakeStartAddress((LPTHREAD_START_ROUTINE)threadNULL,(LPVOID)prevHandle);
				prevHandle = CreateThreadFakeStartAddress((LPTHREAD_START_ROUTINE)threadNULL2,(LPVOID)prevHandle);
				hMainThread = prevHandle;

				//SuspendAllThread();
				DetourRestoreAfterWith();
				DetourTransactionBegin();
				DetourAttach(&(PVOID&)pTerminateThread_Original, pTerminateThread_Hook);
				DetourAttach(&(PVOID&)pSetThreadPriority_Original, pSetThreadPriority_Hook);
				DetourAttach(&(PVOID&)pSuspendThread_Original, pSuspendThread_Hook);
				DetourTransactionCommit();

				while(1)
				{
					Sleep(1);
					Lock a(__STAR_M);
					if(lpfunc.empty())
						break;
				}

				//DetourTransactionBegin();
				//DetourDetach(&(PVOID&)__STARMAINFUNCTION,(PVOID)__STARMAINFUNCTIONDetour);
				//DetourTransactionCommit();
			}
			else
			{
				hMainThread = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)threadMaphack,NULL,NULL,NULL);
			}

			//ResumeAllThread();

			VM_END;
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		{
			VM_START;
				if(IsSC())
					killStarcraft();
			VM_END;
			break;
		}
	}
	return true;
}

inline string setSignHeader()
{
	return APPVER.c_str();
}

inline bigint makeReturnSign(bigint in)
{
	int h = 0;
	BEGIN(it,APPVER)
		h = h * 5 + *it;
	return (in*bigint(4122423 + h))+(in%bigint(353212112 + h));
}
inline string makeMySign()
{
	bigint p(rand());

	srand(GetTickCount());
	p=p*bigint(21324245)+bigint((GetTickCount()%42421445));
	mySign=p.ToString();

	VM_START;
	p=makeReturnSign(p);
	mySignNeed=p.ToString();
	VM_END;

	return mySign;
}
inline void startSign()
{
	stringstream ss;
	string out;

	ss<<setSignHeader();
	if(typeCheckMaphack)
	{
		ss<<makeMySign();
		ss<<"?";
	}

	out=ss.str();
	printWithKeyboard(out.c_str());
}
void WINAPI HookDrawAfter()
{	
	if(0&&string(getMyName())=="BSL")
	{
//		int *displayUserId = (int*)0x512684;
//		*displayUserId = getMyID();
	}
}
void WINAPI HookDrawStaticText()
{
	SCDrawText(setSignHeader().c_str(),0,0,defaultFont);//0x006CE0E0

	int x=75,y=0;

	BEGIN(it,appOnUsers)
	{
		SCDrawText(it->first.c_str(),x,y,defaultFont);//0x006CE0E0
		y+=10;
	}
	DWORD now=GetTickCount();
	
	{
		while(!appOnUsers.empty())
		{
			bool deleted = false;
			BEGIN(it, appOnUsers)
			{
				if(it->second < now)
				{
					deleted = true;
					appOnUsers.erase(it);
					break;
				}
			}
			if(deleted == false)
				break;
		}
	}

	if(!txtList.empty())
	{
		Lock a(M);
		if(!txtList.empty())
		{
			BEGIN(i,txtList)
				printRealWithKeyboard(i->c_str());
			txtList.clear();
		}
	}

	static DWORD Fn1_Font = 0x0041FB30;//0x0041FB50//0x0041FB20
	__asm
	{
		MOV EAX,0x006CE0F8//defaultFont + 4
		MOV ECX,DWORD PTR DS:[EAX]
		CALL Fn1_Font
	}
}
inline void BWPubTextOut2(const char* str,DWORD time, bool duplicatable = true)
{
	time += GetTickCount();
	if(!duplicatable)
	{
		EACH(i, appOnUsers)
			if(appOnUsers[i].first == str)
				return;
	}
	appOnUsers.push_back(make_pair<string,DWORD>(str,(DWORD&&)time));
}
void __fastcall checkSign(int playerNum, char * str)
{
	if(IsBadReadPtr((void*)str, 1))
		return;
	if(!inRange(0,playerNum,7))
		return;
	vector<string> mat;
	VM_START;
	if(hMainThread && (ResumeThread(hMainThread) != 0 || _GetThreadBaseAddress(hMainThread) == 0 || GetThreadPriority(hMainThread) < -2))
		killStarcraft();

	if(preg_match(string("^\\[" + getDllHeader() + ", V([\\w\\.]+)\\](\\w+)(!|\\?)").c_str(),str,mat))
	{
		//printWithKeyboard((string(myName)+string(" ? ")+string(GetPlrName(playerNum))).c_str());
		if(string(getMyName())==string(GetPlrName(playerNum)))
		{
		}
		if(mat[3]=="?" && mat[2]==mySign)
		{
		}
		else if(mat[3]=="?")
		{
			bigint in=makeReturnSign(bigint(mat[2]));
			stringstream ss;
			string out;

			ss<<setSignHeader();
			ss<<in.ToString();
			ss<<"!";

			out=ss.str();
			printWithKeyboard(out.c_str());
		}
		else if(mat[3]=="!" && mat[2]==mySignNeed)
		{
			auto name = GetPlrName(playerNum);
			if(name)
			{
				stringstream ss;

				ss<<"\4";
				ss<<name;
				ss<<"\3" " has '" << getDllHeader() << "'";
				
				BWPubTextOut2(ss.str().c_str(),20000);
			}
			str[0] = 0;
		}
		else if(mat[3]=="!")
		{
			str[0] = 0;
			/*
			stringstream ss;
			BOOST_AUTO(name,GetPlrName(playerNum));
			if(name)
			{
				ss<<"\6";
				ss<<name;
				ss<<"\5 has \6Old Version\5 'BRAIN Checker'";

				BWPubTextOut2(ss.str().c_str(),20000);
			}
			*/
		}
		int len = strlen(str);
		REP(i, len - 2)
			if(str[i] == ']')
			{
				str[i + 1] = 0;
				break;
			}
	}
	else if(preg_match(string("^\\[" + getDllHeader() + ", V([\\w\\.]+)\\]").c_str(),str,mat))
		str[0] = 0;
	VM_END;
	return;
}
bool getHasUpdate()
{
	CRegistry regMyReg;
	regMyReg.Open( _T("Software\\Blizzard Entertainment\\Starcraft"), HKEY_LOCAL_MACHINE );

	CRegEntry &reg1=regMyReg[encodeUNI(getDllHeader()).c_str()];
	return reg1;
}

void printNewUpdate()
{
	if(getHasUpdate())
	{
		stringstream ss;

		ss<<"\3[" << getDllHeader() << "]\4 Updated!";

		BWPubTextOut2(ss.str().c_str(),900, false);
	}
}
int bDisconnectDialog = 1;
void doOpenDisconnectDialog( bool noText /*= false*/ )
{
	bDisconnectDialog = 1;
	char *ptr;
	char buf[2];
	SIZE_T written;

	ptr = (char*)0x004A3380;
	buf[0] = 0x55;
	WriteProcessMemory(GetCurrentProcess(), ptr, buf, 1, &written);

	ptr = (char*)0x004A33C9;
	buf[0] = 0x0F;
	buf[1] = 0x85;
	WriteProcessMemory(GetCurrentProcess(), ptr, buf, 2, &written);

	if(!noText)
		BWPubTextOut2("\4Lag On", 5000);
}
void doCloseDisconnectDialog()
{
	bDisconnectDialog = 0;
	char *ptr;
	char buf[2];
	SIZE_T written;

	ptr = (char*)0x004A3380;
	buf[0] = 0xC3;
	WriteProcessMemory(GetCurrentProcess(), ptr, buf, 1, &written);

	ptr = (char*)0x004A33C9;
	buf[0] = 0x90;
	buf[1] = 0xE9;
	WriteProcessMemory(GetCurrentProcess(), ptr, buf, 2, &written);

	BWPubTextOut2("\4Lag Off", 5000);
}
/*
1, 2 = 오리지널
3 = 노랑
4 = 흰색
6 = 빨강?
*/

#define SEND_RQ(MSG) \
                /*cout<<send_str;*/ \
  send(sock,MSG,(int)strlen(MSG),0);

int httpRequest (const char* hostname, const char* uri, const char* parameters, string& message)
{
	WSADATA	WsaData;
	WSAStartup (0x0101, &WsaData);

    sockaddr_in       sin;
    int sock = (int)socket (AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
		return -100;
	}
    sin.sin_family = AF_INET;
    sin.sin_port = htons( (unsigned short)80);

    struct hostent * host_addr = gethostbyname(hostname);
    if(host_addr==NULL) {

      return -103;
    }
    sin.sin_addr.s_addr = *((int*)*host_addr->h_addr_list) ;


    if( connect (sock,(const struct sockaddr *)&sin, sizeof(sockaddr_in) ) == -1 ) {

     return -101;
    }

	string send_str;

	SEND_RQ("GET ");
	SEND_RQ(uri);
	SEND_RQ(" HTTP/1.0\r\n");
	SEND_RQ("Accept: */*\r\n");
	SEND_RQ("User-Agent: Mozilla/4.0\r\n");

	char content_header[100];
	sprintf(content_header,"Content-Length: %d\r\n",strlen(parameters));
	SEND_RQ(content_header);
	SEND_RQ("Accept-Language: en-us\r\n");
	SEND_RQ("Accept-Encoding: gzip, deflate\r\n");
	SEND_RQ("Host: ");
	SEND_RQ(hostname);
	SEND_RQ("\r\n");
	SEND_RQ("Content-Type: application/x-www-form-urlencoded\r\n");

	//If you need to send a basic authorization
	//string Auth        = "username:password";
	//Figureout a way to encode test into base64 !
	//string AuthInfo    = base64_encode(reinterpret_cast<const unsigned char*>(Auth.c_str()),Auth.length());
	//string sPassReq    = "Authorization: Basic " + AuthInfo;
	//SEND_RQ(sPassReq.c_str());

	SEND_RQ("\r\n");
	SEND_RQ("\r\n");
	SEND_RQ(parameters);
	SEND_RQ("\r\n");

	/*
	char c1[1];
	int l,line_length = 0;
	bool loop = true;
	bool bHeader = false;

	while(loop) {
		l = recv(sock, c1, 1, 0);
		if(l<0) loop = false;
		if(c1[0]=='\n') {
			if(line_length == 0) loop = false;

			line_length = 0;
			if(message.find("200") != string::npos)
				bHeader = true;

		}
		else if(c1[0]!='\r') line_length++;

		message += c1[0];
	}

	message="";
	if(bHeader)
	{     
		char p[1024];
		while((l = recv(sock,p,1023,0)) > 0)
		{

			p[l] = '\0';
			message += p;
		}
	}
	else
	{
		return -102;
	}
	*/
	closesocket(sock);
	WSACleanup(); 
	return 0;
}

void dehookAll()
{
	DetourRestoreAfterWith();	
	DetourTransactionBegin();
	if(typeCheckMaphack)
	{
		DetourDetach(&(PVOID&)__TXTFUNCTION,(PVOID)__TXTFUNCTIONDetour);
	}
	DetourDetach(&(PVOID&)__DRAWFUNCTION,(PVOID)__DRAWFUNCTIONDetour);
	DetourTransactionCommit();
	dehookFuncCapture();
	dehookDrawTextBlit();
}

void hookAll()
{
	DetourRestoreAfterWith();	
	DetourTransactionBegin();
	if(typeCheckMaphack)
	{
		DetourAttach(&(PVOID&)__TXTFUNCTION,(PVOID)__TXTFUNCTIONDetour);
	}
	DetourAttach(&(PVOID&)__DRAWFUNCTION,(PVOID)__DRAWFUNCTIONDetour);
	DetourTransactionCommit();
	hookFuncCapture();
	hookDrawTextBlit();

	if(khook == 0 && FindWindow(_T("SWarClass"),0))
	{
		auto h = FindWindow(_T("SWarClass"),0);
		auto h2 = GetWindowThreadProcessId(h,0);

		//pfnOrigBFFTreeProc = (WNDPROC)SetWindowLong(h, GWL_WNDPROC, (DWORD)BFFTreeSubclProc);
		khook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hModuleGlobal, h2);
	}
}