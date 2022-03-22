#include <ke/ke.h>
#include <rtl/debug.h>
#include <kdcom/kdcom.h>

VOID KeBugCheckEx(ULONG64 Error, ULONG64 Code0, ULONG64 Code1, ULONG64 Code2, ULONG64 Code3) {
    RtlDebugPrint("\n*** STOP:(%p)(%p:%p:%p:%p)\n", Error, Code0, Code1, Code2, Code3);
	KdPrintFormat("\n*** STOP:(%p)(%p:%p:%p:%p)\n", Error, Code0, Code1, Code2, Code3);
    while(1) {
        KiInterruptDisable();
        KiProcessorHalt();
    }
}
