#include <io/ports.h>

VOID IoOutputByte(USHORT Port, UCHAR Value) {
    asm volatile ("outb %0, %1" : : "a"(Value), "Nd"(Port));
}

UCHAR IoReadByte(USHORT Port) {
    UCHAR ReturnValue;
	asm volatile ("inb %1, %0": "=a"(ReturnValue) : "Nd"(Port));
	return ReturnValue;
}

VOID IoOutputWord(USHORT Port, USHORT Value) {
    asm volatile ("outw %0, %1" : : "a"(Value), "Nd"(Port));
}

USHORT IoReadWord(USHORT Port) {
    USHORT ReturnValue;
	asm volatile ("inw %1, %0": "=a"(ReturnValue) : "Nd"(Port));
	return ReturnValue;
}

VOID IoOutputDWord(USHORT Port, UINT Value) {
    asm volatile ("outl %0, %1" : : "a"(Value), "Nd"(Port));
}

UINT IoReadDWord(USHORT Port) {
    UINT ReturnValue;
	asm volatile ("inl %1, %0": "=a"(ReturnValue) : "Nd"(Port));
	return ReturnValue;
}
