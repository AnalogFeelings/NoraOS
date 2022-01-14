#pragma once

#include <nos_types.h>
#include <stivale2.h>

VOID HalVidInit(struct stivale2_struct_tag_framebuffer *VidFramebuffer);
VOID HalVidPrint(PCSTR String);