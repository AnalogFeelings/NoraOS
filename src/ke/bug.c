#include <ke/ke.h>
#include <rtl/debug.h>
#include <kdcom/kdcom.h>

VOID KeBugCheckEx(ULONG64 Error, ULONG64 Code0, ULONG64 Code1, ULONG64 Code2, ULONG64 Code3) {
	KiInterruptDisable;

	RtlDebugPrint("\n!==== BUGCHECK ====!\nThe system has halted to prevent further corruption.\n");
	KdPrintFormat("\n!==== BUGCHECK ====!\nThe system has halted to prevent further corruption.\n");

	RtlDebugPrint("\n*** STOP:(0x%p)(0x%p:0x%p:0x%p:0x%p)\n", Error, Code0, Code1, Code2, Code3);
	KdPrintFormat("\n*** STOP:(0x%p)(0x%p:0x%p:0x%p:0x%p)\n", Error, Code0, Code1, Code2, Code3);

	KiProcessorHalt;
	__builtin_unreachable();
}
