#pragma once

#include <nos_types.h>
#include <stivale2.h>

VOID HalVidInit(struct stivale2_struct_tag_framebuffer *VidFramebuffer);
VOID HalVidClearScreen(UINT Color);
VOID HalVidSetTextColor(UINT Color);
VOID HalVidPrintAt(PCSTR String, INT X, INT Y);
VOID HalVidPrintC(CHAR c);
VOID HalVidPrint(PCSTR String);
VOID HalVidScroll(VOID);
