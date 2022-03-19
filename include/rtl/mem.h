#pragma once

#include <nos_types.h>

#define RtlCopyMemory __builtin_memcpy
#define RtlSetMemory __builtin_memset
#define RtlZeroMemory(A, B) RtlSetMemory(A, 0, B)
#define RtlCompareMemory __builtin_memcmp

VOID RtlStringCopy(CHAR *Dest, PCSTR Source);
ULONG64 RtlStringLength(PCSTR String);
