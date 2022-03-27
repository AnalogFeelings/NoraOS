#include <kdcom.h>
#include <intrin.h>

USHORT DebugPort = 0;

VOID KdInit(USHORT SerialPort) {
	DebugPort = SerialPort;
	__outbyte(SerialPort + COM_INTERRUPT_ENABLE_REG, 0); // Disable all interrupts
	__outbyte(SerialPort + COM_LINE_CONTROL_REG, COM_LC_DLAB); // Enable DLAB to set thhe baur divisor
	__outbyte(SerialPort + COM_LSB_BAUD_RATE, 1); // Set the baudrate [Low Byte]
	__outbyte(SerialPort + COM_MSB_BAUD_RATE, 0); //				  [High Byte]
	__outbyte(SerialPort + COM_LINE_CONTROL_REG, 3); // 8 bits no parity
	__outbyte(SerialPort + COM_INT_IDENT_FIFO_CR, 0xC7); // Enable FIFO
	__outbyte(SerialPort + COM_MODEM_CONTROL_REG, 0xB); // IRQs enabled
}