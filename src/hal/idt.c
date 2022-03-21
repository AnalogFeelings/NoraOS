#include <hal/idt.h>

struct IDT_ENTRY IDT[256] = {0};
struct IDT_PTR IdtPointer = {0};

VOID HalIDTInit(VOID) {
	IdtPointer.Size = sizeof(IDT) - 1;
	IdtPointer.Address = (ULONG64)IDT;

	asm volatile("lidtq %0" : : "m"(IdtPointer));
}
