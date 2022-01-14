#include <hal/vid.h>
#include <ke/stivale.h>
#include <stivale2.h>

STATIC UCHAR Stack[32768];

static struct stivale2_header_tag_framebuffer FramebufferHdrTag = {
	.tag = {.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID, .next = 0},
	.framebuffer_width = 0,
	.framebuffer_height = 0,
	.framebuffer_bpp = 0};

__attribute__((section(".stivale2hdr"),
			   used)) static struct stivale2_header stivale_hdr = {
	.entry_point = 0,
	.stack = (uintptr_t)Stack + sizeof(Stack),
	.flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
	.tags = (uintptr_t)&FramebufferHdrTag};

VOID KiSystemStartup(struct stivale2_struct *Stivale2Struct) {
	struct stivale2_struct_tag_framebuffer *FrameBuffer =
		KiGetStivaleTag(Stivale2Struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	HalVidInit(FrameBuffer);
	for (INT i = 0; i < 10; i++)
		HalVidPrint("Hello World\n");
	for (;;)
		;
}