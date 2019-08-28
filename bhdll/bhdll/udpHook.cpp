#include "stdafx.h"

#include "detours.h"

#include "starLibrary.h"
#include "dllmain.h"
#include "udpHook.h"

#include <boost/thread.hpp>

static boost::mutex m;
int udpSended=0;
HHOOK ghook = 0;
HMODULE ws2,kernel32;

int (WINAPI *  __recvFromOriginal) (
	__in         SOCKET s,
	__out        char *buf,
	__in         int len,
	__in         int flags,
	__out        struct sockaddr *from,
	__inout_opt  int *fromlen
	);
int (WINAPI *  __sendToOriginal) (
	__in SOCKET s,
	__in const char * buf,
	__in int len,
	__in int flags,
	__in const struct sockaddr * to,
	__in int tolen
	);

int WINAPI __sendToHooked (
						   __in SOCKET s,
						   __in const char * buf,
						   __in int len,
						   __in int flags,
						   __in const struct sockaddr * to,
						   __in int tolen
						   )
{
	if(!isInGame())
	{
		wstring server = getServer();
		if(
			0 &&
			(
			server == L"lemon.brainclan.com"
			||
			server == L"58.180.42.12"
			)
			)
		{
			boost::mutex::scoped_lock s(m);
			udpSended++;
		}
	}
	return __sendToOriginal(s,buf,len,flags,to,tolen);
}

int WINAPI __recvFromHooked (
							 __in         SOCKET s,
							 __out        char *buf,
							 __in         int len,
							 __in         int flags,
							 __out        sockaddr*from,
							 __inout_opt  int *fromlen
							 )
{
	if(!isInGame())
	{
		wstring server = getServer();
		if(
			0 &&
			(
			server == L"lemon.brainclan.com"
			||
			server == L"58.180.42.12"
			)
			)
		{
			while(true)
			{
				Sleep(10);
				boost::mutex::scoped_lock s(m);
				if(udpSended>0)
				{
					udpSended--;
					break;
				}
			}

			buf[0]=0x05;
			buf[1]=0x00;
			buf[2]=0x00;
			buf[3]=0x00;
			buf[4]=0x74;
			buf[5]=0x65;
			buf[6]=0x6E;
			buf[7]=0x62;

			((unsigned char*)from)[0]=0x02;
			((unsigned char*)from)[1]=0x00;
			((unsigned char*)from)[2]=0x17;
			((unsigned char*)from)[3]=0xE0;
			((unsigned char*)from)[4]=0xDB;
			((unsigned char*)from)[5]=0xF1;
			((unsigned char*)from)[6]=0x33;
			((unsigned char*)from)[7]=0x12;

			if(fromlen)
				*fromlen=8;
			return 8;
		}
	}
	return __recvFromOriginal(s,buf,len,flags,from,fromlen);
}

LRESULT CALLBACK hookFunc(int code, WPARAM wParam, LPARAM lParam)
{
	UnhookWindowsHookEx(ghook);

	SCUdpHookDll();

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)__sendToOriginal,(PVOID)__sendToHooked);
	DetourAttach(&(PVOID&)__recvFromOriginal,(PVOID)__recvFromHooked);
	DetourTransactionCommit();

	return CallNextHookEx(ghook,code,wParam,lParam);
}
void SCUdpHook()
{
	return;
	if(::FindWindow(_T("SWarClass"),0) && hModuleGlobal && !ghook)
	{
		ghook = SetWindowsHookEx(WH_GETMESSAGE,hookFunc,hModuleGlobal,getWindowThreadID());
	}
}

void SCUdpHookDll()
{
	return;
	ws2 = LoadLibrary(L"wsock32.dll");
	kernel32 = LoadLibrary(L"kernel32.dll");

	__sendToOriginal = (int (WINAPI * ) (
		__in SOCKET s,
		__in const char * buf,
		__in int len,
		__in int flags,
		__in const struct sockaddr * to,
		__in int tolen
		))GetProcAddress(ws2,"sendto");
	__recvFromOriginal = (int (WINAPI * ) (
		__in         SOCKET s,
		__out        char *buf,
		__in         int len,
		__in         int flags,
		__out        struct sockaddr *from,
		__inout_opt  int *fromlen
		))GetProcAddress(ws2,"recvfrom");
}