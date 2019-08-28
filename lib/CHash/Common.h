#ifndef _COMMON_H_
#define _COMMON_H_

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.

//#ifndef _QWORD_DEFINED
//#define _QWORD_DEFINED
//typedef __int64 QWORD, *LPQWORD;
//#endif

typedef unsigned long DWORD;

#define MAKEQWORD(a, b)	\
	((QWORD)( ((QWORD) ((DWORD) (a))) << 32 | ((DWORD) (b))))

#define LODWORD(l) \
	((DWORD)(l))
#define HIDWORD(l) \
	((DWORD)(((QWORD)(l) >> 32) & 0xFFFFFFFF))

// Read 4K of data at a time (used in the C++ streams, Win32 I/O, and assembly functions)
#define MAX_BUFFER_SIZE	4096

// Map a "view" size of 10MB (used in the filemap function)
#define MAX_VIEW_SIZE	10485760

typedef const char * LPCSTR;
typedef char * LPSTR;
typedef const wchar_t * LPCWSTR;
typedef wchar_t * LPWSTR;

#ifdef UNICODE
typedef LPCWSTR LPCTSTR;
typedef LPWSTR LPTSTR;
#else
typedef LPCSTR LPCTSTR;
typedef LPSTR LPTSTR;
#endif

typedef void *HANDLE;
typedef unsigned char BYTE;

#endif
