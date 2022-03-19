#pragma once

#include <nos_types.h>

VOID IoOutputByte(USHORT Port, UCHAR Value);
UCHAR IoReadByte(USHORT Port);
VOID IoOutputWord(USHORT Port, USHORT Value);
USHORT IoReadWord(USHORT Port);
VOID IoOutputDWord(USHORT Port, UINT Value);
UINT IoReadDWord(USHORT Port);
