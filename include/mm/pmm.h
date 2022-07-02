#pragma once

#include <nos_types.h>
#include <limine.h>

#define PAGE_SIZE 0x1000
#define PHYS_MEM_OFFSET 0xffff800000000000

VOID MmFreePages(VOID *Address, ULONG64 Count);
VOID *MmAllocatePhysical(ULONG64 Count);
VOID *MmAllocatePhysicalZero(ULONG64 Count);
VOID MmPhysInit(struct limine_memmap_entry **MemoryMap, SIZE_T EntryCount);
