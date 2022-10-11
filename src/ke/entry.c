#include <hal/gdt.h>
#include <hal/idt.h>
#include <hal/vid.h>
#include <kdcom/kdcom.h>
#include <ke/ke.h>
#include <mm/pmm.h>
#include <rtl/debug.h>
#include <rtl/mem.h>
#include <limine.h>

STATIC VOLATILE struct limine_memmap_request MemoryMapRequest = {
	.id = LIMINE_MEMMAP_REQUEST, .revision = 0
};

STATIC VOLATILE struct limine_framebuffer_request FramebufferRequest = {
	.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0
};

VOID KiSystemStartup(VOID) {
	HalGDTInit();
	HalIDTInit();

	struct limine_framebuffer *Framebuffer = FramebufferRequest.response->framebuffers[0];
	struct limine_memmap_entry **MemoryMap = MemoryMapRequest.response->entries;
    SIZE_T MemoryMapEntryCount = MemoryMapRequest.response->entry_count;

	KdInitSerial();
	MmPhysInit(MemoryMap, MemoryMapEntryCount);
	HalVidInit(Framebuffer);

	HalVidPrint("Greetings from NoraOS!");

	KdPrintFormat("\nAyo, framebuffer address: 0x%p\n", Framebuffer->address);
	KdPrintFormat("Ayo, video framebuffer size: %dx%d\n", Framebuffer->width, Framebuffer->height);

	asm volatile("int3"); //Test IDT

	for (;;)
		;
}
