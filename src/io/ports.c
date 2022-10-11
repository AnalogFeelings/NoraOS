#include <io/ports.h>

VOID IoOutputByte(USHORT Port, UCHAR Value) {
	asm volatile("out %0, %1" : : "d"(Port), "a"(Value));
}

UCHAR IoReadByte(USHORT Port) {
	UCHAR ReturnValue;
	asm volatile("in %0, %1" : "=a"(ReturnValue) : "d"(Port));
	return ReturnValue;
}

VOID IoOutputWord(USHORT Port, USHORT Value) {
	asm volatile("out %0, %1" : : "d"(Port), "a"(Value));
}

USHORT IoReadWord(USHORT Port) {
	USHORT ReturnValue;
	asm volatile("in %0, %1" : "=a"(ReturnValue) : "d"(Port));
	return ReturnValue;
}

VOID IoOutputDWord(USHORT Port, UINT Value) {
	asm volatile("out %0, %1" : : "d"(Port), "a"(Value));
}

UINT IoReadDWord(USHORT Port) {
	UINT ReturnValue;
	asm volatile("in %0, %1" : "=a"(ReturnValue) : "d"(Port));
	return ReturnValue;
}
