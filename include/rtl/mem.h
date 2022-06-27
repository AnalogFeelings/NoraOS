#pragma once

#include <nos_types.h>

#define RtlCopyMemory __builtin_memcpy
#define RtlSetMemory __builtin_memset
#define RtlZeroMemory(A, B) RtlSetMemory(A, 0, B)
#define RtlCompareMemory __builtin_memcmp

VOID *memcpy16(VOID *Dest, CONST VOID *Source, SIZE_T Size);
VOID *memcpy32(VOID *Dest, CONST VOID *Source, SIZE_T Size);
VOID *memcpy64(VOID *Dest, CONST VOID *Source, SIZE_T Size);

VOID memset16(VOID *Dest, USHORT Value, SIZE_T Size);
VOID memset32(VOID *Dest, UINT Value, SIZE_T Size);
VOID memset64(VOID *Dest, ULONG64 Value, SIZE_T Size);

VOID RtlStringCopy(PCHAR Dest, PCSTR Source);
ULONG64 RtlStringLength(PCSTR String);
