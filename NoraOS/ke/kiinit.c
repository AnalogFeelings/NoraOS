#include <nos_types.h>
#include <kdcom.h>

VOID KiSystemStartup(VOID) {
	KdInit(0x3F8); // COM 1
	KdPrint(L"Hello World!\n");
	KdPrint(L"Going to halt\n");
	for (;;)
		__halt();
}