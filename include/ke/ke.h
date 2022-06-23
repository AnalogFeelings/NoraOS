#pragma once

#include <nos_types.h>
#include <stivale2.h>
#include <cpuid.h>

typedef volatile BOOLEAN KSPIN_LOCK, *PKSPIN_LOCK;

#define KiCpuid __get_cpuid

#define KiProcessorHalt asm volatile("1: hlt; jmp 1b")
#define KiPauseProcessor asm volatile("pause")
#define KiInterruptDisable asm volatile("cli")
#define KiInterruptEnable asm volatile("sti")

VOID *KiGetStivaleTag(struct stivale2_struct *Stivale2Struct, ULONG64 Id);

INLINE VOID KeAcquireSpinlock(PKSPIN_LOCK Lock);
INLINE VOID KeDropSpinlock(PKSPIN_LOCK Lock);

VOID KeBugCheckEx(UINT Error, UINT Code0, UINT Code1, UINT Code2, UINT Code3);
#define KeBugCheck(A) KeBugCheckEx(A, 0, 0, 0, 0);
