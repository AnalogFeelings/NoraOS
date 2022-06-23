#pragma once

#include <nos_types.h>
#include <kdcom/kdcom.h>
#include <ke/ke.h>
#include <mm/pmm.h>

/** This is a boundary tag which is prepended to the
 * page or section of a page which we have allocated. It is
 * used to identify valid memory blocks that the
 * application is trying to free.
 */
struct BOUNDARY_TAG {
	UINT Magic;	   //< It's a kind of ...
	UINT Size;	   //< Requested size.
	UINT RealSize; //< Actual size.
	INT Index;	   //< Location in the page table.

	struct BOUNDARY_TAG *SplitLeft;	 //< Linked-list info for broken pages.
	struct BOUNDARY_TAG *SplitRight; //< The same.

	struct BOUNDARY_TAG *Next;	   //< Linked list info.
	struct BOUNDARY_TAG *Previous; //< Linked list info.
};

VOID *malloc(SIZE_T);		   //< The standard function.
VOID *realloc(VOID *, SIZE_T); //< The standard function.
VOID *calloc(SIZE_T, SIZE_T);  //< The standard function.
VOID free(VOID *);			   //< The standard function.
