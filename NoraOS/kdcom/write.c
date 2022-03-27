#include <kdcom.h>
#include <intrin.h>
#include <rtl.h>

STATIC BOOLEAN KdiReadyToSend(USHORT Port) {
	return (__inbyte(Port + COM_LINE_STATUS_REG) & COM_LS_THRE) == COM_LS_THRE;
}

STATIC UCHAR KdiSendByte(USHORT Port, CHAR Byte) {
	while (!KdiReadyToSend(Port))
		;
	__outbyte(Port + COM_DATA_REG, Byte);
}

STATIC VOID KdiWrite(USHORT Port, ULONG32 Length, PVOID Buffer) {
	for (ULONG32 i = 0; i < Length; i++) {
		KdiSendByte(Port, ((WCHAR*)Buffer)[i]);
	}
}

VOID KdPrint(PWSTR Format, ...) {
	WCHAR Buffer[512];
	VA_LIST Args;
	__crt_va_start(Args, Format);
	RtlFormatBufferFromArgumentList(Buffer, Format, Args);
	__crt_va_end(Args);
	KdiWrite(DebugPort, (ULONG32)RtlStringLength(Buffer), Buffer);
}