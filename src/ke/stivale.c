#include <ke/ke.h>

VOID *KiGetStivaleTag(struct stivale2_struct *Stivale2Struct, ULONG64 Id) {
	struct stivale2_tag *CurrentTag = (VOID *)Stivale2Struct->tags;
	for (;;) {
		if (CurrentTag == NULL)
			return NULL;
		if (CurrentTag->identifier == Id)
			return CurrentTag;
		CurrentTag = (VOID *)CurrentTag->next;
	}
}
