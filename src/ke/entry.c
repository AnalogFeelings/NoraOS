#include <hal/gdt.h>
#include <hal/idt.h>
#include <hal/vid.h>
#include <kdcom/kdcom.h>
#include <ke/ke.h>
#include <mm/mm.h>
#include <rtl/debug.h>
#include <rtl/mem.h>
#include <stivale2.h>

STATIC UCHAR Stack[32768];

STATIC struct stivale2_header_tag_framebuffer FramebufferHdrTag = {
	.tag = {.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID, .next = 0},
	.framebuffer_width = 0,
	.framebuffer_height = 0,
	.framebuffer_bpp = 0};

__attribute__((section(".stivale2hdr"), used))
STATIC struct stivale2_header stivale_hdr = {
	.entry_point = 0,
	.stack = (ULONG64_PTR)Stack + sizeof(Stack),
	.flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
	.tags = (ULONG64_PTR)&FramebufferHdrTag};

VOID KiSystemStartup(struct stivale2_struct *Stivale2Struct) {
	HalGDTInit();
	HalIDTInit();

	struct stivale2_struct_tag_framebuffer *FrameBuffer =
		KiGetStivaleTag(Stivale2Struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	struct stivale2_struct_tag_memmap *MemoryMap =
		KiGetStivaleTag(Stivale2Struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);

	KdInitSerial();
	MmPhysInit(MemoryMap);
	HalVidInit(FrameBuffer);

	KdPrintFormat("\nAyo, framebuffer address: 0x%p\n", FrameBuffer->framebuffer_addr);
	KdPrintFormat("Ayo, video framebuffer size: %dx%d\n", FrameBuffer->framebuffer_width, FrameBuffer->framebuffer_height);

	for (;;)
		;
}
