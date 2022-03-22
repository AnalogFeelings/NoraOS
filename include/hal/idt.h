#pragma once

#include <nos_types.h>

struct IDT_ENTRY {
	USHORT Offset1;
	USHORT Selector;
	UCHAR Ist;
	UCHAR TypeAttributes;
	USHORT Offset2;
	UINT Offset3;
	UINT Reserved;
} __attribute__((packed));

struct IDT_PTR {
	USHORT Size;
	ULONG64 Address;
} __attribute__((packed));

VOID HalIDTInit(VOID);
VOID HalIDTSetDescriptor(UCHAR Vector, VOID* Handler, UCHAR Ist);
