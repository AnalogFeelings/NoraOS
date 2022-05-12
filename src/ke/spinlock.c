#include <ke/ke.h>

VOID KeAcquireSpinlock(PKSPIN_LOCK Lock) {
    while ( !__sync_bool_compare_and_swap(Lock, 0, 1))
		 while (*Lock)
			 KiPauseProcessor();
}

VOID KeDropSpinlock(PKSPIN_LOCK Lock) {
    __sync_lock_release(Lock);
}
