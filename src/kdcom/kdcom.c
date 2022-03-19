#include <io/ports.h>
#include <kdcom/kdcom.h>

STATIC BOOLEAN KdSerialInitialized = FALSE;

VOID KdInitSerial(VOID) {
    IoOutputByte(COM1 + COM_INTERRUPT_ENABLE_REG, 0x00); //Disable all interrupts.

    IoOutputByte(COM1 + COM_LINE_CONTROL_REG, 0x80); //Enable DLAB. This is to set the baud divisor.
    IoOutputByte(COM1 + COM_DATA_REG, 0x03); //Set divisor to 3. 115200 / 3 = 38400 baud.
    IoOutputByte(COM1 + COM_INTERRUPT_ENABLE_REG, 0x00); //Divisor high byte.

    IoOutputByte(COM1 + COM_LINE_CONTROL_REG, 0x03); //8 bits, no parity, 1 stop bit.
    IoOutputByte(COM1 + COM_INT_IDENT_FIFO_CR, 0xC7); //Enable FIFO.
    IoOutputByte(COM1 + COM_MODEM_CONTROL_REG, 0x0B); //Enable IRQs.

    IoOutputByte(COM1 + COM_MODEM_CONTROL_REG, 0x1E); //Enable loopback mode. We are going to test the UART.
    IoOutputByte(COM1 + COM_DATA_REG, 0xAE); //Send 0xAE and test if the UART returns the same byte.

    if(IoReadByte(COM1 + COM_DATA_REG) != 0xAE) {
        KdSerialInitialized = FALSE;
        return; //Failure! UART is defective.
    }

    IoOutputByte(COM1 + COM_MODEM_CONTROL_REG, 0x0F); //Success. Set mode back to normal.
    KdSerialInitialized = TRUE;
    return;
}

STATIC BOOLEAN KdIsTransmitEmpty(VOID) {
    return IoReadByte(COM1 + COM_LINE_STATUS_REG) & 0x20;
}

VOID KdPutChar(CHAR c) {
    if(KdSerialInitialized == FALSE) return;

    while(KdIsTransmitEmpty() == FALSE);

    IoOutputByte(COM1, c);
}

VOID KdPrint(PCSTR String) {
    if(KdSerialInitialized == FALSE) return;

    while(*String != '\0') {
        if(*String == '\n') {
            KdPutChar('\r');
        }
        KdPutChar(*String++);
    }
}
