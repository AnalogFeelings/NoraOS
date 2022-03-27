#include <rtl.h>

VOID RtlReverseString(PWSTR String) {
    ULONG i, j;
    WCHAR Temp;

    i = 0;
    j = (ULONG)RtlStringLength(String) - 1;

    for (; i < j; i++, j--) {
        Temp = String[i];
        String[i] = String[j];
        String[j] = Temp;
    }
}

VOID RtlIntToString(LONG Value, PWSTR Buffer, ULONG Base) {
    ULONG Temp;
    ULONG Index;
    BOOLEAN Negative;

    Index = 0;
    Negative = FALSE;
    if (Value == 0) {
        Buffer[Index++] = '0';
        Buffer[Index] = 0;
        return;
    }

    if (Base == 10 && Value < 0) {
        Negative = TRUE;
        Value = -Value;
    }

    while (Value != 0) {
        Temp = Value % Base;
        Buffer[Index++] = (Temp > 9) ? (WCHAR)(Temp - 10) + 'a' : (WCHAR)(Temp + '0');
        Value /= Base;
    }

    if (Negative) {
        Buffer[Index++] = '-';
    }

    Buffer[Index] = 0;

    RtlReverseString(Buffer);
}

LONG RtlStringToInt(PWCHAR Buffer) {
    LONG Temp;
    ULONG Index;
    ULONG Base;
    BOOLEAN Negative;

    Base = 10;
    Index = 0;
    Temp = 0;
    Negative = FALSE;
    if (Buffer[Index] == '-') {
        Negative = TRUE;
        Index++;
    }

    if (Buffer[Index] == '0' && Buffer[Index + 1] == 'x') {
        Base = 16;
        Index += 2;
    }

    for (;
        Buffer[Index] != 0 &&
        ((Buffer[Index] >= '0' && Buffer[Index] <= '9') ||
            ((Base > 10 && RtlLowerChar(Buffer[Index]) >= 'a' && RtlLowerChar(Buffer[Index] <= 'f')))); Index++) {//'a' + base - 10
        if (Buffer[Index] >= '0' && Buffer[Index] <= '9') {
            Temp = Temp * Base + Buffer[Index] - '0';
        }
        else {
            Temp = Temp * Base + Buffer[Index] - 'a' + 10;
        }
    }

    if (Negative) {
        Temp *= -1;
    }

    return Temp;
}

VOID RtlFormatBufferFromArgumentList(PWSTR Buffer, PWSTR Format, VA_LIST List) {
    ULONG Index;
    BOOLEAN CodeHash;
    BOOLEAN CodePad;
    BOOLEAN CodeUpper;

    ULONG64 ArgULL64;
    ULONG32 Lo32;
    ULONG32 Hi32;
    WCHAR LoBuf[16];
    WCHAR HiBuf[16];
    LONG32 LoPad;
    LONG32 HiPad;
    LONG32 ZeroPad;
    PWCHAR ArgWS;
    WCHAR ArgWC;
    PCHAR ArgAS;
    CHAR ArgAC;

    Index = 0;

    while (*Format) {
        if (*Format != '%') {
            Buffer[Index] = *Format++;
            Index++;
            continue;
        }
        else {
            Format++;
        }

        CodeHash = FALSE;
        CodePad = FALSE;
        CodeUpper = FALSE;
        LoPad = 0;
        HiPad = 0;
        ZeroPad = 0;

        if (*Format == '#') {
            CodeHash = TRUE;
            Format++;
        }

        if (*Format == '.') {
            Format++;
            ZeroPad = RtlStringToInt(Format);
            while (*Format >= '0' && *Format <= '9') {
                Format++;
            }
        }

        switch (*Format) {
        case '%':;
            Buffer[Index] = *Format++;
            Index++;
            break;
        case 'w':;
            Format++;
        case 's':;
        case 'c':;

            if (*Format == 's') {
                ArgWS = __crt_va_arg(List, PWCHAR);

                if (ArgWS == NULL) {
                    ArgWS = L"(null)";
                }
                else if (*ArgWS == 0) {
                    ArgWS = L"(zero)";
                }

                for (ULONG32 i = 0; ArgWS[i]; i++, Index++) {
                    Buffer[Index] = ArgWS[i];
                }

                Format++;
            }
            else if (*Format == 'c') {
                ArgWC = __crt_va_arg(List, WCHAR);

                if (ArgWC == 0) {
                    ArgWS = L"(zero)";
                    for (ULONG32 i = 0; ArgWS[i]; i++, Index++) {
                        Buffer[Index] = ArgWS[i];
                    }
                }
                else {
                    Buffer[Index++] = ArgWC;
                }
                Format++;
            }

            break;
        case 'a':;
            Format++;

            if (*Format == 's') {
                ArgAS = __crt_va_arg(List, PCHAR);

                if (ArgAS == NULL) {
                    ArgAS = "(null)";
                }
                else if (*ArgAS == 0) {
                    ArgAS = "(zero)";
                }

                for (ULONG32 i = 0; ArgAS[i]; i++, Index++) {
                    Buffer[Index] = (WCHAR)ArgAS[i];
                }

                Format++;
            }
            else if (*Format == 'c') {
                ArgAC = __crt_va_arg(List, CHAR);

                if (ArgAC == 0) {
                    ArgAS = "(zero)";
                    for (ULONG32 i = 0; ArgAS[i]; i++, Index++) {
                        Buffer[Index] = ArgAS[i];
                    }
                }
                else {
                    Buffer[Index++] = ArgAC;
                }
                Format++;
            }

            break;
        case 'x':;
        case 'X':;
            Format++;

            if ((*Format == 'l' &&
                *(Format + 1) == 'l') ||
                (*Format == 'L' &&
                    *(Format + 1) == 'L')) {
                Format++;

                ArgULL64 = __crt_va_arg(List, ULONG64);
                Lo32 = (ULONG32)(ArgULL64);
                Hi32 = (ULONG32)(ArgULL64 >> 32);

                CodeUpper = *Format == 'L';

                if (CodeHash) {
                    Buffer[Index++] = '0';
                    Buffer[Index++] = 'x';
                }

                RtlIntToString(Lo32, LoBuf, 16);
                RtlIntToString(Hi32, HiBuf, 16);
                LoPad = 8 - (ULONG)RtlStringLength(LoBuf);
                if (Hi32 == 0) {
                    LoPad = 0;
                }

                HiPad = ZeroPad - ((ULONG32)RtlStringLength(LoBuf) + LoPad);
                if (Hi32 != 0) {
                    HiPad -= (ULONG)RtlStringLength(HiBuf);
                }

                for (unsigned char i = 0; i < HiPad; i++, Index++) {
                    Buffer[Index] = '0';
                }

                for (unsigned char i = 0; HiBuf[i] && Hi32 != 0; i++, Index++) {
                    Buffer[Index] = CodeUpper ? RtlUpperChar(HiBuf[i]) : HiBuf[i];
                }

                for (unsigned char i = 0; i < LoPad; i++, Index++) {
                    Buffer[Index] = '0';
                }

                for (unsigned char i = 0; LoBuf[i]; i++, Index++) {
                    Buffer[Index] = CodeUpper ? RtlUpperChar(LoBuf[i]) : LoBuf[i];
                }

                Format++;
            }
            else if (*Format == 'l' ||
                *Format == 'L') {
                Lo32 = __crt_va_arg(List, ULONG32);

                CodeUpper = *Format == 'L';

                if (CodeHash) {
                    Buffer[Index++] = '0';
                    Buffer[Index++] = 'x';
                }

                RtlIntToString(Lo32, LoBuf, 16);
                LoPad = ZeroPad - (LONG32)RtlStringLength(LoBuf);

                for (int i = 0; LoPad > 0 && i < LoPad; i++, Index++) {
                    Buffer[Index] = '0';
                }

                for (int i = 0; LoBuf[i]; i++, Index++) {
                    Buffer[Index] = CodeUpper ? RtlUpperChar(LoBuf[i]) : LoBuf[i];
                }

                Format++;
            }

            break;
        case 'd':
            Lo32 = __crt_va_arg(List, ULONG32);

            CodeUpper = *Format == 'd';

            if (CodeHash) {
                Buffer[Index++] = '0';
                Buffer[Index++] = 'n';
            }

            RtlIntToString(Lo32, LoBuf, 10);
            LoPad = ZeroPad - (LONG32)RtlStringLength(LoBuf);

            for (unsigned char i = 0; i < LoPad; i++, Index++) {
                Buffer[Index] = '0';
            }

            for (unsigned char i = 0; LoBuf[i]; i++, Index++) {
                Buffer[Index] = CodeUpper ? RtlUpperChar(LoBuf[i]) : LoBuf[i];
            }

            Format++;
            break;
        default:;
            break;
        }

    }

    Buffer[Index] = 0;
}

VOID RtlFormatBuffer(PWSTR Buffer, PWSTR Format, ...) {
    VA_LIST ArgumentList;
    __crt_va_start(ArgumentList, Format);
    RtlFormatBufferFromArgumentList(Buffer, Format, ArgumentList);
    __crt_va_end(ArgumentList);
}

LONG RtlCompareString(PCWSTR String1, PCWSTR String2, BOOLEAN CaseInsensitive) {
    if (!CaseInsensitive) {
        while (*String1 && *String2 && *String1 == *String2) {

            String1++, String2++;
        }
        return *String1 - *String2;
    }
    else {
        while (*String1 && *String2 && RtlUpperChar(*String1) == RtlUpperChar(*String2)) {

            String1++, String2++;
        }
        return RtlUpperChar(*String1) - RtlUpperChar(*String2);
    }
}

LONG RtlCompareStringLength(PCWSTR  String1, PCWSTR  String2, BOOLEAN CaseInsensitive, ULONG64 Length) {
    if (!CaseInsensitive) {
        while (Length-- && *String1 && *String2 && *String1 == *String2) {

            String1++, String2++;
        }
        return *--String1 - *--String2;
    }
    else {
        while (Length-- && *String1 && *String2 && RtlUpperChar(*String1) == RtlUpperChar(*String2)) {

            String1++, String2++;
        }
        return RtlUpperChar(*--String1) - RtlUpperChar(*--String2);
    }
}

LONG RtlCompareAnsiString(PCSTR String1, PCSTR String2, BOOLEAN CaseInsensitive) {
    if (!CaseInsensitive) {
        while (*String1 && *String2 && *String1 == *String2) {

            String1++, String2++;
        }
        return *String1 - *String2;
    }
    else {
        while (*String1 && *String2 && RtlUpperChar(*String1) == RtlUpperChar(*String2)) {

            String1++, String2++;
        }
        return RtlUpperChar(*String1) - RtlUpperChar(*String2);
    }
}

LONG RtlCompareAnsiStringLength(PCSTR String1, PCSTR String2, BOOLEAN CaseInsensitive, ULONG64 Length) {
    if (!CaseInsensitive) {
        while (--Length && *String1 && *String2 && *String1 == *String2) {

            String1++, String2++;
        }
        return *String1 - *String2;
    }
    else {
        while (--Length && *String1 && *String2 && RtlUpperChar(*String1) == RtlUpperChar(*String2)) {

            String1++, String2++;
        }
        return RtlUpperChar(*String1) - RtlUpperChar(*String2);
    }
}

ULONG lstrlenW(PCWSTR String) {
    ULONG Length = 0;

    while (String[Length++] != 0);
    return Length - 1; // Return the length excluding a null terminator.
}

LONG lstrcmpW(PCWSTR String1, PCWSTR String2) {
    while (*String1++ == *String2++)
        if (*String1 == 0) return 0;      // Return  0 if strings are the same.

    if (*--String1 > *--String2) return 1;    // Return  1 if String1 is bigger.
    else return -1;                         // Return -1 if String2 is bigger.
}

LONG lstrcmpiW(PCWSTR String1, PCWSTR String2) {
    while (RtlUpperChar(*String1) == RtlUpperChar(*String2))
        if (*String1++ == 0 || *String2++ == 0) return 0; // Return  0 if strings are the same.

    if (RtlUpperChar(*String1) > RtlUpperChar(*String2)) return 1;    // Return  1 if String1 is bigger.
    else return -1;                                         // Return -1 if String2 is bigger.
}
LONG lstrncmpW(PCWSTR String1, PCWSTR String2, ULONG Characters) {
    while (*String1++ == *String2++ && --Characters)
        if (*String1 == 0) return 0; // Return 0 if characters are the same.

    return Characters; // Return the amount of different characters.
}

LONG lstrncmpiW(PCWSTR String1, PCWSTR String2, ULONG Characters) {
    while (RtlUpperChar(*String1) == RtlUpperChar(*String2) && --Characters)
        if (*String1++ == 0 || *String2++ == 0) return 0; // Return 0 if characters are the same.

    return Characters; // Return the amount of different characters.
}

VOID lstrcpyW(PWSTR DestinationString, PWSTR SourceString) {
    while (*SourceString != 0)
        *DestinationString++ = *SourceString++;
    *DestinationString = 0;
}

VOID lstrncpyW(PWSTR DestinationString, PWSTR SourceString, ULONG Characters) {
    while (*SourceString != 0 && Characters--)
        *DestinationString++ = *SourceString++;

    *DestinationString = 0;
}

VOID lstrcatW(PWSTR  DestinationString, PCWSTR SourceString) {
    while (*DestinationString != 0) DestinationString++;
    while (*SourceString != 0) *DestinationString++ = *SourceString++;

    *DestinationString = 0;
}

VOID lstrncatW(PWSTR  DestinationString, PCWSTR SourceString, ULONG  Characters) {
    while (*++DestinationString != 0);
    while (*SourceString != 0 && Characters--) *DestinationString++ = *SourceString++;

    *DestinationString = 0;
}

PWSTR lstrchrW(PWSTR String, WCHAR Character) {
    while (*String++ != 0)
        if (*String == Character) return String; // Return pointer to the first occurence.

    return NULL; // Return NULL pointer if no matches.
}

PWSTR lstrchriW(PWSTR String, WCHAR Character) {
    while (*String++ != 0)
        if (RtlUpperChar(*String) == RtlUpperChar(Character)) return String; // Return pointer to the first occurence.

    return NULL; // Return NULL pointer if no matches.
}

PWSTR lstrstrW(PWSTR String, PWSTR Substring) {
    do {
        PWSTR SubstringAddress = Substring;

        while (*String == *SubstringAddress)
            if (String++, *++SubstringAddress == 0) return String - (SubstringAddress - Substring); // Return pointer to the first occurence.
    } while (*String++ != 0);

    return NULL; // Return NULL pointer if no matches.
}

PWSTR lstrstriW(PWSTR String, PWSTR Substring) {
    do {
        PWSTR SubstringAddress = Substring;

        while (RtlUpperChar(*String) == RtlUpperChar(*SubstringAddress)) {
            if (*SubstringAddress++ == 0) return String - SubstringAddress + Substring; // Return pointer to the first occurence.
            String++;
        }
    } while (*String != 0);

    return NULL; // Return NULL pointer if no matches.
}

wchar_t* wcsncpy(wchar_t* destination, const wchar_t* source, size_t num) {
    wchar_t* o;

    o = destination;

    __stosw(o, source, num);

    return o;
}