#include <ke/ke.h>
#include <rtl/debug.h>

VOID KeBugCheckEx(ULONG64 Error, ULONG64 Code0, ULONG64 Code1, ULONG64 Code2, ULONG64 Code3) {
    RtlDebugPrint("*** STOP:(%p)(%p:%p:%p:%p)\n", Error, Code0, Code1, Code2, Code3);
    while(1) {
        KiInterruptDisable();
        KiProcessorHalt();
    }
}
