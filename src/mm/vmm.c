#include <mm/vmm.h>

/**  Durand's Ridiculously Amazing Super Duper Memory functions.  */

#define DEBUG

#define LIBALLOC_MAGIC 0xc001c0de
#define MAXCOMPLETE 5
#define MAXEXP 32
#define MINEXP 8

#define MODE_BEST 0
#define MODE_INSTANT 1

#define MODE MODE_BEST

struct BOUNDARY_TAG *FreePages[MAXEXP]; //< Allowing for 2^MAXEXP blocks
INT CompletePages[MAXEXP];				//< Allowing for 2^MAXEXP blocks

#ifdef DEBUG
UINT Allocated = 0; //< The real amount of memory allocated.
UINT InUse = 0;		//< The amount of memory in use (malloc'ed).
#endif

STATIC BOOLEAN Initialized = 0; //< Flag to indicate initialization.
STATIC INT PageSize = 4096;		//< Individual page size
STATIC INT PageCount = 16;		//< Minimum number of pages to allocate.

STATIC KSPIN_LOCK Lock;

// ***********   HELPER FUNCTIONS  *******************************

/** This function is supposed to lock the memory data structures. It
 * could be as simple as disabling interrupts or acquiring a spinlock.
 * It's up to you to decide. 
 *
 * \return 0 if the lock was acquired successfully. Anything else is
 * failure.
 */
STATIC INT liballoc_lock() {
	KeAcquireSpinlock(&Lock);
	KiInterruptDisable;

	return 0;
}

/** This function unlocks what was previously locked by the liballoc_lock
 * function.  If it disabled interrupts, it enables interrupts. If it
 * had acquiried a spinlock, it releases the spinlock. etc.
 *
 * \return 0 if the lock was successfully released.
 */
STATIC INT liballoc_unlock() {
	KeDropSpinlock(&Lock);
	KiInterruptEnable;

	return 0;
}

/** This is the hook into the local system which allocates pages. It
 * accepts an integer parameter which is the number of pages
 * required.  The page size was set up in the liballoc_init function.
 *
 * \return NULL if the pages were not allocated.
 * \return A pointer to the allocated memory.
 */
STATIC VOID *liballoc_alloc(SIZE_T Size) {
	VOID *Target = MmAllocatePhysicalZero(Size);

	return Target;
}

/** This frees previously allocated memory. The void* parameter passed
 * to the function is the exact same value returned from a previous
 * liballoc_alloc call.
 *
 * The integer value is the number of pages to free.
 *
 * \return 0 if the memory was successfully freed.
 */
STATIC INT liballoc_free(VOID *Target, SIZE_T Size) {
	MmFreePages(Target, Size);

	return 0;
}

/** Returns the exponent required to manage 'size' amount of memory. 
 *
 *  Returns n where  2^n <= size < 2^(n+1)
 */
STATIC INLINE INT getexp(UINT Size) {
	if (Size < (1 << MINEXP)) {
#ifdef DEBUG
		KdPrintFormat("getexp returns -1 for %d less than MINEXP\n", Size);
#endif
		return -1; // Smaller than the quantum.
	}

	int shift = MINEXP;

	while (shift < MAXEXP) {
		if ((1 << shift) > Size)
			break;
		shift += 1;
	}

#ifdef DEBUG
	KdPrintFormat("getexp returns %d (%d bytes) for %d size\n", shift - 1, (1 << (shift - 1)), Size);
#endif

	return shift - 1;
}

STATIC VOID *liballoc_memset(VOID *Dest, INT Value, SIZE_T Size) {
	INT i;
	for (i = 0; i < Size; i++)
		((PCHAR)Dest)[i] = Value;

	return Dest;
}

STATIC VOID *liballoc_memcpy(VOID *Dest, CONST VOID *Source, SIZE_T Size) {
	PCHAR cdest;
	PCHAR csrc;
	PUINT ldest = (PUINT)Dest;
	PUINT lsrc = (PUINT)Source;

	while (Size >= sizeof(UINT)) {
		*ldest++ = *lsrc++;
		Size -= sizeof(UINT);
	}

	cdest = (PCHAR)ldest;
	csrc = (PCHAR)lsrc;

	while (Size > 0) {
		*cdest++ = *csrc++;
		Size -= 1;
	}

	return Dest;
}

#ifdef DEBUG
STATIC VOID dump_array() {
	INT i = 0;
	struct BOUNDARY_TAG *tag = NULL;

	KdPrintFormat("------ Free pages array ---------\n");
	KdPrintFormat("System memory allocated: %d\n", Allocated);
	KdPrintFormat("Memory in used (malloc'ed): %d\n", InUse);

	for (i = 0; i < MAXEXP; i++) {
		KdPrintFormat("%d(%d): ", i, CompletePages[i]);

		tag = FreePages[i];
		while (tag != NULL) {
			if (tag->SplitLeft != NULL)
				KdPrintFormat("*");
			KdPrintFormat("%d", tag->RealSize);
			if (tag->SplitRight != NULL)
				KdPrintFormat("*");

			KdPrintFormat(" ");
			tag = tag->Next;
		}
		KdPrintFormat("\n");
	}

	KdPrintFormat("'*' denotes a split to the left/right of a tag\n");
}
#endif

STATIC INLINE VOID insert_tag(struct BOUNDARY_TAG *Tag, int Index) {
	INT realIndex;

	if (Index < 0) {
		realIndex = getexp(Tag->RealSize - sizeof(struct BOUNDARY_TAG));
		if (realIndex < MINEXP)
			realIndex = MINEXP;
	} else
		realIndex = Index;

	Tag->Index = realIndex;

	if (FreePages[realIndex] != NULL) {
		FreePages[realIndex]->Previous = Tag;
		Tag->Next = FreePages[realIndex];
	}

	FreePages[realIndex] = Tag;
}

STATIC INLINE VOID remove_tag(struct BOUNDARY_TAG *Tag) {
	if (FreePages[Tag->Index] == Tag)
		FreePages[Tag->Index] = Tag->Next;

	if (Tag->Previous != NULL)
		Tag->Previous->Next = Tag->Next;
	if (Tag->Next != NULL)
		Tag->Next->Previous = Tag->Previous;

	Tag->Next = NULL;
	Tag->Previous = NULL;
	Tag->Index = -1;
}

STATIC INLINE struct BOUNDARY_TAG *melt_left(struct BOUNDARY_TAG *Tag) {
	struct BOUNDARY_TAG *left = Tag->SplitLeft;

	left->RealSize += Tag->RealSize;
	left->SplitRight = Tag->SplitRight;

	if (Tag->SplitRight != NULL)
		Tag->SplitRight->SplitLeft = left;

	return left;
}

STATIC INLINE struct BOUNDARY_TAG *absorb_right(struct BOUNDARY_TAG *Tag) {
	struct BOUNDARY_TAG *right = Tag->SplitRight;

	remove_tag(right); // Remove right from free pages.

	Tag->RealSize += right->RealSize;

	Tag->SplitRight = right->SplitRight;
	if (right->SplitRight != NULL)
		right->SplitRight->SplitLeft = Tag;

	return Tag;
}

STATIC INLINE struct BOUNDARY_TAG *split_tag(struct BOUNDARY_TAG *Tag) {
	UINT remainder = Tag->RealSize - sizeof(struct BOUNDARY_TAG) - Tag->Size;

	struct BOUNDARY_TAG *new_tag =
		(struct BOUNDARY_TAG *)((UINT)Tag + sizeof(struct BOUNDARY_TAG) + Tag->Size);

	new_tag->Magic = LIBALLOC_MAGIC;
	new_tag->RealSize = remainder;

	new_tag->Next = NULL;
	new_tag->Previous = NULL;

	new_tag->SplitLeft = Tag;
	new_tag->SplitRight = Tag->SplitRight;

	if (new_tag->SplitRight != NULL)
		new_tag->SplitRight->SplitLeft = new_tag;
	Tag->SplitRight = new_tag;

	Tag->RealSize -= new_tag->RealSize;

	insert_tag(new_tag, -1);

	return new_tag;
}

// ***************************************************************

STATIC struct BOUNDARY_TAG *allocate_new_tag(UINT Size) {
	UINT pages;
	UINT usage;
	struct BOUNDARY_TAG *tag;

	// This is how much space is required.
	usage = Size + sizeof(struct BOUNDARY_TAG);

	// Perfect amount of space
	pages = usage / PageSize;
	if ((usage % PageSize) != 0)
		pages += 1;

	// Make sure it's >= the minimum size.
	if (pages < PageCount)
		pages = PageCount;

	tag = (struct BOUNDARY_TAG *)liballoc_alloc(pages);

	if (tag == NULL)
		return NULL; // uh oh, we ran out of memory.

	tag->Magic = LIBALLOC_MAGIC;
	tag->Size = Size;
	tag->RealSize = pages * PageSize;
	tag->Index = -1;

	tag->Next = NULL;
	tag->Previous = NULL;
	tag->SplitLeft = NULL;
	tag->SplitRight = NULL;

#ifdef DEBUG
	KdPrintFormat("Resource allocated %p of %d pages (%d bytes) for %d size.\n", tag, pages, pages * PageSize, Size);

	Allocated += pages * PageSize;

	KdPrintFormat("Total memory usage = %d KB\n", (INT)((Allocated / (1024))));
#endif

	return tag;
}

VOID *malloc(SIZE_T Size) {
	INT index;
	VOID *ptr;
	struct BOUNDARY_TAG *tag = NULL;

	liballoc_lock();

	if (Initialized == FALSE) {
#ifdef DEBUG
		KdPrintFormat("%s\n", "liballoc initializing.");
#endif
		for (index = 0; index < MAXEXP; index++) {
			FreePages[index] = NULL;
			CompletePages[index] = 0;
		}
		Initialized = TRUE;
	}

	index = getexp(Size) + MODE;
	if (index < MINEXP)
		index = MINEXP;

	// Find one big enough.
	tag = FreePages[index]; // Start at the front of the list.
	while (tag != NULL) {
		// If there's enough space in this tag.
		if ((tag->RealSize - sizeof(struct BOUNDARY_TAG)) >= (Size + sizeof(struct BOUNDARY_TAG))) {
#ifdef DEBUG
			KdPrintFormat("Tag search found %d >= %d\n", (tag->RealSize - sizeof(struct BOUNDARY_TAG)), (Size + sizeof(struct BOUNDARY_TAG)));
#endif
			break;
		}

		tag = tag->Next;
	}

	// No page found. Make one.
	if (tag == NULL) {
		if ((tag = allocate_new_tag(Size)) == NULL) {
			liballoc_unlock();
			return NULL;
		}

		index = getexp(tag->RealSize - sizeof(struct BOUNDARY_TAG));
	} else {
		remove_tag(tag);

		if ((tag->SplitLeft == NULL) && (tag->SplitRight == NULL))
			CompletePages[index] -= 1;
	}

	// We have a free page.  Remove it from the free pages list.

	tag->Size = Size;

	// Removed... see if we can re-use the excess space.

#ifdef DEBUG
	KdPrintFormat("Found tag with %d bytes available (requested %d bytes, leaving %d), which has exponent: %d (%d bytes)\n", tag->RealSize - sizeof(struct BOUNDARY_TAG), Size, tag->RealSize - Size - sizeof(struct BOUNDARY_TAG), index, 1 << index);
#endif

	UINT remainder = tag->RealSize - Size - sizeof(struct BOUNDARY_TAG) * 2; // Support a new tag + remainder

	if (((INT)(remainder) > 0) /*&& ( (tag->real_size - remainder) >= (1<<MINEXP))*/) {
		INT childIndex = getexp(remainder);

		if (childIndex >= 0) {
#ifdef DEBUG
			KdPrintFormat("Seems to be splittable: %d >= 2^%d .. %d\n", remainder, childIndex, (1 << childIndex));
#endif

			struct BOUNDARY_TAG *new_tag = split_tag(tag);

			new_tag = new_tag; // Get around the compiler warning about unused variables.

#ifdef DEBUG
			KdPrintFormat("Old tag has become %d bytes, new tag is now %d bytes (%d exp)\n", tag->RealSize, new_tag->RealSize, new_tag->Index);
#endif
		}
	}

	ptr = (VOID *)((UINT)tag + sizeof(struct BOUNDARY_TAG));

#ifdef DEBUG
	InUse += Size;
	KdPrintFormat("malloc: %p, %d, %d\n", ptr, (INT)InUse / 1024, (INT)Allocated / 1024);
	dump_array();
#endif

	liballoc_unlock();
	return ptr;
}

VOID free(VOID *Target) {
	INT index;
	struct BOUNDARY_TAG *tag;

	if (Target == NULL)
		return;

	liballoc_lock();

	tag = (struct BOUNDARY_TAG *)((UINT)Target - sizeof(struct BOUNDARY_TAG));

	if (tag->Magic != LIBALLOC_MAGIC) {
		liballoc_unlock(); // release the lock
		return;
	}

#ifdef DEBUG
	InUse -= tag->Size;
	KdPrintFormat("free: %p, %d, %d\n", Target, (INT)InUse / 1024, (INT)Allocated / 1024);
#endif

	// MELT LEFT...
	while ((tag->SplitLeft != NULL) && (tag->SplitLeft->Index >= 0)) {
#ifdef DEBUG
		KdPrintFormat("Melting tag left into available memory. Left was %d, becomes %d (%d)\n", tag->SplitLeft->RealSize, tag->SplitLeft->RealSize + tag->RealSize, tag->SplitLeft->RealSize);
#endif
		tag = melt_left(tag);
		remove_tag(tag);
	}

	// MELT RIGHT...
	while ((tag->SplitRight != NULL) && (tag->SplitRight->Index >= 0)) {
#ifdef DEBUG
		KdPrintFormat("Melting tag right into available memory. This was was %d, becomes %d (%d)\n", tag->RealSize, tag->SplitRight->RealSize + tag->RealSize, tag->SplitRight->RealSize);
#endif
		tag = absorb_right(tag);
	}

	// Where is it going back to?
	index = getexp(tag->RealSize - sizeof(struct BOUNDARY_TAG));
	if (index < MINEXP)
		index = MINEXP;

	// A whole, empty block?
	if ((tag->SplitLeft == NULL) && (tag->SplitRight == NULL)) {

		if (CompletePages[index] == MAXCOMPLETE) {
			// Too many standing by to keep. Free this one.
			UINT pages = tag->RealSize / PageSize;

			if ((tag->RealSize % PageSize) != 0)
				pages += 1;
			if (pages < PageCount)
				pages = PageCount;

			liballoc_free(tag, pages);

#ifdef DEBUG
			Allocated -= pages * PageSize;
			KdPrintFormat("Resource freeing %p of %d pages\n", tag, pages);
			dump_array();
#endif

			liballoc_unlock();
			return;
		}

		CompletePages[index] += 1; // Increase the count of complete pages.
	}

	// ..........

	insert_tag(tag, index);

#ifdef DEBUG
	KdPrintFormat("Returning tag with %d bytes (requested %d bytes), which has exponent: %d\n", tag->RealSize, tag->Size, index);
	dump_array();
#endif

	liballoc_unlock();
}

VOID *calloc(SIZE_T Count, SIZE_T Size) {
	INT real_size;
	VOID *p;

	real_size = Count * Size;

	p = malloc(real_size);

	liballoc_memset(p, 0, real_size);

	return p;
}

VOID *realloc(VOID *Target, SIZE_T Size) {
	VOID *ptr;
	struct BOUNDARY_TAG *tag;
	INT real_size;

	if (Size == 0) {
		free(Target);
		return NULL;
	}
	if (Target == NULL)
		return malloc(Size);

	if (liballoc_lock != NULL)
		liballoc_lock(); // lockit
	tag = (struct BOUNDARY_TAG *)((UINT)Target - sizeof(struct BOUNDARY_TAG));
	real_size = tag->Size;
	if (liballoc_unlock != NULL)
		liballoc_unlock();

	if (real_size > Size)
		real_size = Size;

	ptr = malloc(Size);
	liballoc_memcpy(ptr, Target, real_size);
	free(Target);

	return ptr;
}
