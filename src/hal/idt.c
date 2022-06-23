#include <hal/idt.h>

__attribute__((aligned(0x10))) struct IDT_ENTRY IDT[256] = { 0 };
struct IDT_PTR IdtPointer = { 0 };

extern VOID *HalIsrTable[];

VOID HalIDTInit(VOID) {
	IdtPointer.Size = sizeof(IDT) - 1;
	IdtPointer.Address = (ULONG64)IDT;

	for (UCHAR vec = 0; vec < 32; vec++) {
		HalIDTSetDescriptor(vec, HalIsrTable[vec], 0);
	}

	asm volatile("lidtq %0"
				 :
				 : "m"(IdtPointer));
}

VOID HalIDTSetDescriptor(UCHAR Vector, VOID *Handler, UCHAR Ist) {
	ULONG64 isr = (ULONG64)Handler;

	IDT[Vector].Offset1 = (USHORT)isr;
	IDT[Vector].Selector = 0x08;
	IDT[Vector].Ist = Ist;
	IDT[Vector].TypeAttributes = 0x8E;
	IDT[Vector].Offset2 = (USHORT)(isr >> 16);
	IDT[Vector].Offset3 = (UINT)(isr >> 32);
	IDT[Vector].Reserved = 0;
}
