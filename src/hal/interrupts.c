#include <ke/ke.h>

VOID HalDispatchInterrupts(VOID) {
    KeBugCheck(0x2);
}
