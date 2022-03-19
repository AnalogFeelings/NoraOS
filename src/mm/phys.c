#include <mm/mm.h>
#include <rtl/mem.h>

// Bitmap functions

#define ALIGN_UP(__number) (((__number) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define ALIGN_DOWN(__number) ((__number) & ~(PAGE_SIZE - 1))

#define BIT_SET(__bit) (MiPhysBitmap[(__bit) / 8] |= (1 << ((__bit) % 8)))
#define BIT_CLEAR(__bit) (MiPhysBitmap[(__bit) / 8] &= ~(1 << ((__bit) % 8)))
#define BIT_TEST(__bit) ((MiPhysBitmap[(__bit) / 8] >> ((__bit) % 8)) & 1)

static UCHAR *MiPhysBitmap = 0;
ULONG64 HighestPage = 0;

VOID MiFreePage(VOID *Address) {
	BIT_CLEAR((ULONG64)Address / PAGE_SIZE);
}

VOID MiAllocPage(VOID *Address) {
	BIT_SET((ULONG64)Address / PAGE_SIZE);
}

VOID MmFreePages(VOID *Address, ULONG64 Count) {
	for (ULONG64 i = 0; i < Count; i++)
		MiFreePage((VOID *)((ULONG64)Address + (i * PAGE_SIZE)));
}

VOID MiAllocPages(VOID *Address, ULONG64 Count) {
	for (ULONG64 i = 0; i < Count; i++)
		MiAllocPage((VOID *)((ULONG64)Address + (i * PAGE_SIZE)));
}

VOID *MmAllocatePhysical(ULONG64 Count) {
	for (ULONG64 i = 0; i < HighestPage / PAGE_SIZE; i++) {
		for (ULONG64 j = 0; j < Count; j++) {
			if (BIT_TEST(i))
				break;
			else if (j == Count - 1) {
				MiAllocPages((VOID *)(i * PAGE_SIZE), Count);
				return (VOID *)(i * PAGE_SIZE);
			}
		}
	}
	return NULL;
}

VOID *MmAllocatePhysicalZero(ULONG64 Count) {
	VOID *Temp = MmAllocatePhysical(Count);
	RtlZeroMemory(Temp, Count * PAGE_SIZE);
	return Temp;
}

VOID MmPhysInit(struct stivale2_struct_tag_memmap *MemoryMap) {
	ULONG64 Top = 0;
	// Find the top most page
	for (ULONG64 i = 0; i < MemoryMap->entries; i++) {
		if (MemoryMap->memmap[i].type != STIVALE2_MMAP_USABLE ||
			MemoryMap->memmap[i].type != STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE)
			continue;
		Top = MemoryMap->memmap[i].base + MemoryMap->memmap[i].length;
		if (Top > HighestPage)
			HighestPage = Top;
	}
	// Calculate the bitmap size and get the bitmap address
	ULONG64 BitmapSize = ALIGN_UP(ALIGN_DOWN(HighestPage) / PAGE_SIZE / 8);
	for (ULONG64 i = 0; i < MemoryMap->entries; i++) {
		if (MemoryMap->memmap[i].type == STIVALE2_MMAP_USABLE &&
			MemoryMap->memmap[i].base >= BitmapSize) {
			MiPhysBitmap =
				(UCHAR *)(MemoryMap->memmap[i].base + PHYS_MEM_OFFSET);
			break;
		}
	}
	RtlSetMemory(MiPhysBitmap, 0xff, BitmapSize);
	for (ULONG64 i = 0; i < MemoryMap->entries; i++) {
		if (MemoryMap->memmap[i].type == STIVALE2_MMAP_USABLE)
			MmFreePages((VOID *)MemoryMap->memmap[i].base,
						MemoryMap->memmap[i].length / PAGE_SIZE);
	}
}
