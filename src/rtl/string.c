#include <rtl/mem.h>

VOID RtlStringCopy(CHAR *Dest, PCSTR Source) {
	for (; (*Dest = *Source); Source++, Dest++)
		;
}

ULONG64 RtlStringLength(PCSTR String) {
	ULONG64 Len = 0;
	while (String[Len] != '\0')
		++Len;
	return Len;
}
