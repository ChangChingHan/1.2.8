#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef _DEBUG
const DWORD MS_VC_EXCEPTION=0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

static void __dbg_SetThreadName( DWORD dwThreadID, char* threadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
}


static void __dbg_printf (const char * format,...)
{
#define MAX_DBG_MSG_LEN (1024)
	char buf[MAX_DBG_MSG_LEN];
	va_list ap;

	va_start(ap, format);

	_vsnprintf(buf, sizeof(buf), format, ap);
	OutputDebugStringA(buf);

	va_end(ap);
}
#define DBG __dbg_printf
#else
static void __dbg_printf (const char * format,...) {}
static void __dbg_SetThreadName (DWORD dwThreadID, char* threadName) {}
#define DBG  1?((void)(NULL)):__dbg_printf
#endif

#define _FLP_ "%s:%d: "
#define _FL_ __FUNC__, __LINE__

#define _FFLP_ "%s:" _FLP_
#define _FFL_ __FILE__, _FL_

// For example:
// DBG(_FLP_ "format string", _FL_, arg1, arg2);
// DBG(_FFLP_ "format string", _FFL, arg1, arg2);
#endif