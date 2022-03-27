#pragma once

#include <nos_types.h>

struct GDT_ENTRY {
	USHORT Limit0;
	USHORT Base0;
	UCHAR Base1;
	UCHAR AccessByte;
	UCHAR Limit1Flags;
	UCHAR Base2;
} __attribute__((packed));

struct GDT_POINTER {
	USHORT Size;
	ULONG64 Offset;
} __attribute__((packed));

struct GDT {
	struct GDT_ENTRY Entry[3];
} __attribute__((packed));

EXTERN VOID HalLoadGDT(struct GDT_POINTER *GdtPtr);
VOID HalGDTInit(VOID);
