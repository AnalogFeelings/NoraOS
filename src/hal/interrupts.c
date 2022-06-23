#include <ke/ke.h>

__attribute__((noreturn))
VOID HalDispatchInterrupts(VOID) {
	KeBugCheck(0x2);
}
