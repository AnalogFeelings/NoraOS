#pragma once

#include <nos_types.h>
#include <stivale2.h>
#include <cpuid.h>

typedef volatile BOOLEAN KSPIN_LOCK, *PKSPIN_LOCK;

#define KiCpuid __get_cpuid

EXTERN VOID KiProcessorHalt(VOID);
EXTERN VOID KiPauseProcessor(VOID);
EXTERN VOID KiInterruptDisable(VOID);
EXTERN VOID KiInterruptEnable(VOID);

VOID *KiGetStivaleTag(struct stivale2_struct *Stivale2Struct, ULONG64 Id);

INLINE VOID KeAcquireSpinlock(PKSPIN_LOCK Lock);
INLINE VOID KeDropSpinlock(PKSPIN_LOCK Lock);

VOID KeBugCheckEx(ULONG64 Error, ULONG64 Code0, ULONG64 Code1, ULONG64 Code2, ULONG64 Code3);
#define KeBugCheck(A) KeBugCheckEx(A, 0, 0, 0, 0);
