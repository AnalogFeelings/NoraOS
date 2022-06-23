#include <ke/ke.h>
#include <rtl/debug.h>
#include <kdcom/kdcom.h>

VOID KeBugCheckEx(UINT Error, UINT Code0, UINT Code1, UINT Code2, UINT Code3) {
	KiInterruptDisable;

	RtlDebugPrint("\n[==================== BUGCHECK ====================]\n");
    RtlDebugPrint("The system has halted to prevent further corruption.\n");
    
	KdPrintFormat("\n[==================== BUGCHECK ====================]\n");
    KdPrintFormat("The system has halted to prevent further corruption.\n");

	RtlDebugPrint("\n*** STOP: 0x%p (0x%p:0x%p:0x%p:0x%p)\n", Error, Code0, Code1, Code2, Code3);
	KdPrintFormat("\n*** STOP: 0x%p (0x%p:0x%p:0x%p:0x%p)\n", Error, Code0, Code1, Code2, Code3);

	KiProcessorHalt;
	__builtin_unreachable();
}
