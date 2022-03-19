#pragma once

#include <nos_types.h>

#define COM1 0x3F8

#define COM_DATA_REG 0
#define COM_INTERRUPT_ENABLE_REG 1

#define COM_LSB_BAUD_RATE 0
#define COM_MSB_BAUD_RATE 1

#define COM_INT_IDENT_FIFO_CR 2
#define COM_LINE_CONTROL_REG 3
#define COM_MODEM_CONTROL_REG 4
#define COM_LINE_STATUS_REG 5
#define COM_MODEM_STATUS_REG 6
#define COM_SCRATCH_REG 7

VOID KdInitSerial(VOID);
VOID KdPutChar(CHAR c);
VOID KdPrint(PCSTR String);
