#include <hal/vid.h>
#include <ke/stivale.h>
#include <mm/mm.h>
#include <rtl/debug.h>
#include <rtl/mem.h>
#include <stivale2.h>
#include <hal/gdt.h>

STATIC UCHAR Stack[32768];

static struct stivale2_header_tag_framebuffer FramebufferHdrTag = {
	.tag = {.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID, .next = 0},
	.framebuffer_width = 0,
	.framebuffer_height = 0,
	.framebuffer_bpp = 0};

__attribute__((section(".stivale2hdr"),
			   used)) static struct stivale2_header stivale_hdr = {
	.entry_point = 0,
	.stack = (ULONG64_PTR)Stack + sizeof(Stack),
	.flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
	.tags = (ULONG64_PTR)&FramebufferHdrTag};

VOID KiSystemStartup(struct stivale2_struct *Stivale2Struct) {
	HalGDTInit();
	struct stivale2_struct_tag_framebuffer *FrameBuffer =
		KiGetStivaleTag(Stivale2Struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    struct stivale2_struct_tag_memmap *MemoryMap = KiGetStivaleTag(Stivale2Struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	HalVidInit(FrameBuffer);
    MmPhysInit(MemoryMap);
    for(int i = 0; i <= 90; i++) {
        RtlDebugPrint("%d\n", i);
    }
	for (;;)
		;
}
