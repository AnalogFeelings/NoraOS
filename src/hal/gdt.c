#include <hal/gdt.h>

struct GDT HalGlobalDescriptorTable = { 0 };
struct GDT_POINTER HalGdtPointer = { 0 };

VOID HalGDTInit(VOID) {
	HalGlobalDescriptorTable.Entry[1].AccessByte = 0x9a;
	HalGlobalDescriptorTable.Entry[1].Limit1Flags = 0x20;

	HalGlobalDescriptorTable.Entry[2].AccessByte = 0x92;

	HalGdtPointer.Size = sizeof(struct GDT) - 1;
	HalGdtPointer.Offset = (ULONG64)&HalGlobalDescriptorTable;

	HalLoadGDT(&HalGdtPointer);
}
