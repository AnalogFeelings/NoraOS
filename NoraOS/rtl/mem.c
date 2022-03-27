#include <rtl.h>

VOID* __memset(VOID* m1, UCHAR v, __int64 n) {
	__stosb(m1, v, n);
	return m1;
}

VOID* __memcpy(VOID* dst, VOID* src, int n) {
	__movsb(dst, src, n);
	return dst;
}

LONG __memcmp(VOID* m1, VOID* m2, int n) {
	PUCHAR u1, u2;
	ULONG len;
	u1 = m1;
	u2 = m2;
	len = n;
	while (n) {
		if (*u1 != *u2) {
			return *u1 - *u2;
		}
		u1++, u2++;
		n--;
	}
	return 0;
}