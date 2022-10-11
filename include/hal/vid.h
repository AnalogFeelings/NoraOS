#pragma once

#include <nos_types.h>
#include <limine.h>

struct FRAMEBUFFER {
	PUINT VideoAddress;
	ULONG64 Pitch, Bpp;
	USHORT Width, Height;
	ULONG64 TextX, TextY;
	UINT TextColor;
	UINT BackgroundColor;
};

VOID HalVidInit(struct limine_framebuffer *VidFramebuffer);
VOID HalVidClearScreen(UINT Color);
VOID HalVidSetTextColor(UINT Color);
VOID HalVidPrintC(CHAR c);
VOID HalVidPrint(PCSTR String);
VOID HalVidScroll(VOID);
