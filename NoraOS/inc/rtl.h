#pragma once

#include <nos_types.h>
#include <intrin.h>

typedef CHAR* VA_LIST;

VOID* __memset(VOID* m1, UCHAR v, __int64 n);
VOID* __memcpy(VOID* dst, VOID* src, int n);
LONG __memcmp(VOID* m1, VOID* m2, int n);

#define RtlZeroMemory(a, b) (__memset(a, 0, b))
#define RtlSetMemory(a, b, c) (__memset(a, b, c))
#define RtlCopyMemory(a, b, c) (__movsb(a, b, c))
#define RtlCompareMemory(a, b, c) (__memcmp(a, b, c))

#define RtlUpperChar(c)               ((c) >= 'a' && (c) <= 'z' ? (c) - ' ' : (c))
#define RtlLowerChar(c)               ((c) >= 'A' && (c) <= 'Z' ? (c) + ' ' : (c))

#define RtlStringLength                 wcslen
#define RtlAnsiStringLength             strlen
#define RtlAnsiStringCompare            strcmp

VOID RtlReverseString(PWSTR String);
VOID RtlIntToString(LONG Value, PWSTR Buffer, ULONG Base);
LONG RtlStringToInt(PWCHAR Buffer);
VOID RtlFormatBufferFromArgumentList(PWSTR Buffer, PWSTR Format, VA_LIST List);
VOID RtlFormatBuffer(PWSTR Buffer, PWSTR Format, ...);
LONG RtlCompareString(PCWSTR String1, PCWSTR String2, BOOLEAN CaseInsensitive);
LONG RtlCompareStringLength(PCWSTR  String1, PCWSTR  String2, BOOLEAN CaseInsensitive, ULONG64 Length);
LONG RtlCompareAnsiString(PCSTR String1, PCSTR String2, BOOLEAN CaseInsensitive);
LONG RtlCompareAnsiStringLength(PCSTR String1, PCSTR String2, BOOLEAN CaseInsensitive, ULONG64 Length);

ULONG lstrlenW(PCWSTR String);
LONG lstrcmpW(PCWSTR String1, PCWSTR String2);
LONG lstrcmpiW(PCWSTR String1, PCWSTR String2);
LONG lstrncmpW(PCWSTR String1, PCWSTR String2, ULONG Characters);
LONG lstrncmpiW(PCWSTR String1, PCWSTR String2, ULONG Characters);
VOID lstrcpyW(PWSTR DestinationString, PWSTR SourceString);
VOID lstrncpyW(PWSTR DestinationString, PWSTR SourceString, ULONG Characters);
VOID lstrcatW(PWSTR  DestinationString, PCWSTR SourceString);
VOID lstrncatW(PWSTR  DestinationString, PCWSTR SourceString, ULONG  Characters);
PWSTR lstrchrW(PWSTR String, WCHAR Character);
PWSTR lstrchriW(PWSTR String, WCHAR Character);
PWSTR lstrstrW(PWSTR String, PWSTR Substring);
PWSTR lstrstriW(PWSTR String, PWSTR Substring);

wchar_t* wcsncpy(wchar_t* destination, const wchar_t* source, size_t num);