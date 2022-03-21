#include <hal/vid.h>
#include <rtl/debug.h>

static VOID RtlPrintf(PCSTR fmt, va_list args) {
	while (*fmt) {
		if (*fmt == '%') {
			fmt++;
			if (*fmt == '%') {
				HalVidPrintC('%');
			}
			if (*fmt == 's') {
				HalVidPrint(va_arg(args, PCSTR));
			}
			if (*fmt == 'S') {
				PCSTR str = va_arg(args, PCSTR);
				SIZE_T len = va_arg(args, size_t);
				for (SIZE_T i = 0; i < len; i++) {
					HalVidPrintC(str[i]);
				}
			}
			if (*fmt == 'x' || *fmt == 'p') {
				CHAR string[17] = {0};
				ULONG64 number = va_arg(args, ULONG64);
				for (INT i = 16; i > 0; number >>= 4) {
					string[--i] = "0123456789abcdef"[number & 0x0f];
				}
				HalVidPrint(string);
			}
			if (*fmt == 'd') {
				CHAR string[21] = {0};
				ULONG64 number = va_arg(args, ULONG64);
				for (INT i = 20; i > 0;) {
					string[--i] = number % 10 + '0';
					number /= 10;
				}
				SIZE_T counter = 0;
				while (string[counter] == '0' && counter < 19) {
					counter++;
				}
				HalVidPrint(&string[counter]);
			}
		} else {
			HalVidPrintC(*fmt);
		}
		fmt++;
	}
}

VOID RtlDebugPrint(PCSTR Fmt, ...) {
	va_list Args;
	va_start(Args, Fmt);
	RtlPrintf(Fmt, Args);
	va_end(Args);
}
