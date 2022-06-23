#include <io/ports.h>
#include <kdcom/kdcom.h>
#include <nos_info.h>

STATIC BOOLEAN KdSerialInitialized = FALSE;

VOID KdInitSerial(VOID) {
	IoOutputByte(COM1 + COM_INTERRUPT_ENABLE_REG, 0x00); // Disable all interrupts.

	IoOutputByte(COM1 + COM_LINE_CONTROL_REG, 0x80);	 // Enable DLAB. This is to set the baud divisor.
	IoOutputByte(COM1 + COM_DATA_REG, 0x03);			 // Set divisor to 3. 115200 / 3 = 38400 baud.
	IoOutputByte(COM1 + COM_INTERRUPT_ENABLE_REG, 0x00); // Divisor high byte.

	IoOutputByte(COM1 + COM_LINE_CONTROL_REG, 0x03);  // 8 bits, no parity, 1 stop bit.
	IoOutputByte(COM1 + COM_INT_IDENT_FIFO_CR, 0xC7); // Enable FIFO.
	IoOutputByte(COM1 + COM_MODEM_CONTROL_REG, 0x0B); // Enable IRQs.

	IoOutputByte(COM1 + COM_MODEM_CONTROL_REG, 0x1E); // Enable loopback mode. We are going to test the UART.
	IoOutputByte(COM1 + COM_DATA_REG, 0xAE);		  // Send 0xAE and test if the UART returns the same byte.

	if (IoReadByte(COM1 + COM_DATA_REG) != 0xAE) {
		KdSerialInitialized = FALSE;
		return; // Failure! UART is defective.
	}

	IoOutputByte(COM1 + COM_MODEM_CONTROL_REG, 0x0F); // Success. Set mode back to normal.
	KdSerialInitialized = TRUE;

	KdPrint("\n==============================================\n");
	KdPrintFormat("NoraOS Version %d.%d.%d commit-%s-%s\n", NOS_VERSION_MAJOR, NOS_VERSION_MINOR, NOS_VERSION_PATCH, NOS_COMMIT_HASH, NOS_BRANCH);
	KdPrintFormat("Serial running at %d baud.\n", (INT)(115200 / 3)); //TODO: make the bootloader be able to pass the divisor value into the OS?
	KdPrint("==============================================\n");
}

STATIC BOOLEAN KdIsTransmitEmpty(VOID) {
	return IoReadByte(COM1 + COM_LINE_STATUS_REG) & 0x20;
}

VOID KdPutChar(CHAR c) {
	if (KdSerialInitialized == FALSE)
		return;

	while (KdIsTransmitEmpty() == FALSE)
		;

	IoOutputByte(COM1, c);
}

STATIC VOID KdPrintf(PCSTR fmt, va_list args) {
	while (*fmt) {
		if (*fmt == '%') {
			fmt++;
			if (*fmt == '%') {
				KdPutChar('%');
			}
			if (*fmt == 's') {
				KdPrint(va_arg(args, PCSTR));
			}
			if (*fmt == 'S') {
				PCSTR str = va_arg(args, PCSTR);
				SIZE_T len = va_arg(args, size_t);
				for (SIZE_T i = 0; i < len; i++) {
					KdPutChar(str[i]);
				}
			}
			if (*fmt == 'x' || *fmt == 'p') {
				CHAR string[17] = { 0 };
				ULONG64 number = va_arg(args, ULONG64);
				for (INT i = 16; i > 0; number >>= 4) {
					string[--i] = "0123456789abcdef"[number & 0x0f];
				}
				KdPrint(string);
			}
			if (*fmt == 'd') {
				CHAR string[21] = { 0 };
				ULONG64 number = va_arg(args, ULONG64);
				for (INT i = 20; i > 0;) {
					string[--i] = number % 10 + '0';
					number /= 10;
				}
				SIZE_T counter = 0;
				while (string[counter] == '0' && counter < 19) {
					counter++;
				}
				KdPrint(&string[counter]);
			}
		} else {
			KdPutChar(*fmt);
		}
		fmt++;
	}
}

VOID KdPrintFormat(PCSTR Fmt, ...) {
	if (KdSerialInitialized == FALSE)
		return;

	va_list Args;
	va_start(Args, Fmt);
	KdPrintf(Fmt, Args);
	va_end(Args);
}

VOID KdPrint(PCSTR String) {
	if (KdSerialInitialized == FALSE)
		return;

	while (*String != '\0') {
		if (*String == '\n') {
			KdPutChar('\r');
		}
		KdPutChar(*String++);
	}
}
