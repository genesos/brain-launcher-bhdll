#include <stdafx.h>

#include "../../lib/CxImage/ximage.h"
#include "../../lib/GenesisLib/file.h"
#include "../../themida/ThemidaSDK/Include/c/ThemidaSDK.h"
#include "detours.h"

#include "lib_star_capture.h"

void * __StormOrdinalOriginal342;
void hookFuncCapture()
{
	HMODULE storm;
	storm = LoadLibrary(L"storm.dll");
	__StormOrdinalOriginal342 = (void*)((int)storm + 0x35620);

	if(__StormOrdinalOriginal342)
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)__StormOrdinalOriginal342,(PVOID)__StormOrdinalOriginal342Hooked);
		DetourTransactionCommit();
	}
}
void dehookFuncCapture()
{
	HMODULE storm;
	storm = LoadLibrary(L"storm.dll");
	__StormOrdinalOriginal342 = (void*)((int)storm + 0x35620);

	if(__StormOrdinalOriginal342)
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)__StormOrdinalOriginal342,(PVOID)__StormOrdinalOriginal342Hooked);
		DetourTransactionCommit();
	}
}
const void*OrigDrawTextBlit = (void*)0x0041F585;

__declspec(naked)
void HookedDrawTextBlit16()
{
	__asm
	{
		MOV EBX,DWORD PTR SS:[EBP-0x20]
		MOV EDX,ESI
			IMUL EDX,DWORD PTR SS:[EBP-0x28]
		PUSH EAX
			IMUL EAX,EAX,2
			ADD EDX,EAX
			POP EAX
			CMP DWORD PTR DS:[EDX+EBX],0
			JMP OrigDrawTextBlit
	}
}

__declspec(naked)
void HookedDrawTextBlit24()
{
	__asm
	{
		MOV EBX,DWORD PTR SS:[EBP-0x20]
		MOV EDX,ESI
			IMUL EDX,DWORD PTR SS:[EBP-0x28]
		PUSH EAX
			IMUL EAX,EAX,3
			ADD EDX,EAX
			POP EAX
			CMP DWORD PTR DS:[EDX+EBX],0
			JMP OrigDrawTextBlit
	}
}
__declspec(naked)
void HookedDrawTextBlit32()
{
	__asm
	{
		MOV EBX,DWORD PTR SS:[EBP-0x20]
		MOV EDX,ESI
			IMUL EDX,DWORD PTR SS:[EBP-0x28]
		PUSH EAX
			IMUL EAX,EAX,4
			ADD EDX,EAX
			POP EAX
			CMP DWORD PTR DS:[EDX+EBX],0
			JMP OrigDrawTextBlit
	}
}
namespace
{
	char bufDrawTextBuf[5];
	char *bufDrawTextPtr = (char*)0x0041F576;
}
void hookDrawTextBlit()
{
	char buf[5];
	buf[0] = 0xE9;

	HWND window = FindWindow(_T("SWarClass"),NULL);
	int bpp = GetDeviceCaps(GetDC(window),BITSPIXEL);
	RECT windowRect;
	bool getRectResult = GetWindowRect(window,&windowRect);
	if(windowRect.right - windowRect.left < 650)
		return;

	if(bpp == 16)
		*(int*)(buf + 1) = (int)HookedDrawTextBlit16 - (int)0x41F57B;
	else if(bpp == 24)
		*(int*)(buf + 1) = (int)HookedDrawTextBlit24 - (int)0x41F57B;
	else if(bpp == 32)
		*(int*)(buf + 1) = (int)HookedDrawTextBlit32 - (int)0x41F57B;
	else 
		return;

	REP(i, 4)
		bufDrawTextBuf[i] = *(bufDrawTextPtr + i);

	SIZE_T written;
	WriteProcessMemory(GetCurrentProcess(), bufDrawTextPtr, buf, 5, &written);
}
void dehookDrawTextBlit()
{
	SIZE_T written;
	WriteProcessMemory(GetCurrentProcess(), bufDrawTextPtr, bufDrawTextBuf, 5, &written);
}
bool isWMode()
{
	HWND window = FindWindow(_T("SWarClass"),NULL);
	if(!window)
		return false;
	RECT windowRect;
	bool getRectResult = GetWindowRect(window,&windowRect);
	if(windowRect.right - windowRect.left < 650)
		return false;
	return true;
}
void __hookedCapture(char * scFileName)
{
	if(!scFileName)
		return;
	if(!*scFileName)
		return;
	wstring wscFileName = encodeUNI(scFileName);
	wstring path = getModuleAbsPath(0) + L"\\" + wscFileName;
	CxImage img;
	bool succ = img.Load(path.c_str(), CXIMAGE_FORMAT_PCX);
	if(!succ)
		return;

	DeleteFile(path.c_str());
	{
		wstring newpath = path.substr(0, path.length() - 4) + L".png";
		img.Save(newpath.c_str(), CXIMAGE_FORMAT_PNG);
	}

	{
		const BYTE first = 1;
		const BYTE deFirst = ~first;
		BYTE hex = 0;

		const int txtX = 520, txtY = 470;
		img.IncreaseBpp(24);
		RGBQUAD textColor;

		textColor.rgbBlue = 0xff;
		textColor.rgbGreen = 0xdf;
		textColor.rgbRed = 0xdf;
		img.DrawString(0, txtX+1, txtY+1, L"Signatured By Mini", textColor, L"tahoma", 15, 100);
		img.DrawString(0, txtX+1, txtY-1, L"Signatured By Mini", textColor, L"tahoma", 15, 100);
		img.DrawString(0, txtX-1, txtY+1, L"Signatured By Mini", textColor, L"tahoma", 15, 100);
		img.DrawString(0, txtX-1, txtY-1, L"Signatured By Mini", textColor, L"tahoma", 15, 100);

		textColor.rgbBlue = 0xff;
		textColor.rgbGreen = 0x4f;
		textColor.rgbRed = 0x4f;
		img.DrawString(0, txtX, txtY, L"Signatured By Mini", textColor, L"tahoma", 15, 100);

		REP(y, img.GetHeight() - 1)
			REP(x, img.GetWidth() - 1)
		{
			RGBQUAD pixel = img.GetPixelColor(x, y);

			if(x % 500 == 0 && y % 100 == 0)
			{
				VM_START;
				hex = ~(hex ^ (((pixel.rgbRed ^ pixel.rgbGreen ^ pixel.rgbBlue) & deFirst) >> ((x + y * 5) % 5)));
				VM_END;
			}
			else if(x % 500 == 4 && y % 100 == 4)
			{
				CODEREPLACE_START;
				hex = ~(hex ^ (((pixel.rgbRed ^ pixel.rgbGreen ^ pixel.rgbBlue) & deFirst) >> ((x + y * 5) % 6)));
				CODEREPLACE_END;
			}
			else
				hex = ~(hex ^ (((pixel.rgbRed ^ pixel.rgbGreen ^ pixel.rgbBlue) & deFirst) >> ((x + y * 5) % 7)));

			pixel.rgbGreen &= deFirst;
			pixel.rgbGreen |= hex & first;

			img.SetPixelColor(x, y, pixel);
		}

		wstring newpath = path.substr(0, path.length() - 4) + L".mini.png";
		img.Save(newpath.c_str(), CXIMAGE_FORMAT_PNG);
	}

	string strFileName = scFileName;
	scFileName[strFileName.length() - 2] = 'n';
	scFileName[strFileName.length() - 1] = 'g';
}
static char* scFileName;
static void* oldBack;
__declspec(naked)
void __StormOrdinalOriginal342HookedAfter()
{
	__asm
	{
		pushad;
	}
	__hookedCapture(scFileName);
	__asm
	{
		popad;
		jmp oldBack;
	}
}
__declspec(naked)
void __StormOrdinalOriginal342Hooked()
{
	__asm
	{
		mov scFileName, ecx;
		pop oldBack;
		push __StormOrdinalOriginal342HookedAfter;
		jmp __StormOrdinalOriginal342;
	}
}
