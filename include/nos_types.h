#pragma once

#define NULL ((void *)0)
#define VOID void

#define EXTERN extern
#define STATIC static
#define VOLATILE volatile
#define DECLSPEC __declspec
#define CONST const
#define FORCEINLINE __forceinline
#define UNALIGNED __unaligned

typedef unsigned char UCHAR, *PUCHAR;
typedef unsigned short USHORT, *PUSHORT;
typedef unsigned int UINT, *PUINT;
typedef unsigned long ULONG, *PULONG;
typedef unsigned long ULONG32, *PULONG32;
typedef unsigned long long ULONG64, *PULONG64;

typedef signed char CHAR, *PCHAR;
typedef CONST char *PCSTR;
typedef signed short SHORT, *PSHORT;
typedef signed int INT, *PINT;
typedef signed long LONG, *PLONG;
typedef signed long LONG32, *PLONG32;
typedef signed long long LONG64, *PLONG64;

typedef unsigned long long UCHAR_PTR;
typedef unsigned long long USHORT_PTR;
typedef unsigned long long ULONG_PTR;
typedef unsigned long long ULONG32_PTR;
typedef unsigned long long ULONG64_PTR;

typedef unsigned char BOOLEAN, *PBOOLEAN;

#define TRUE 1
#define FALSE 0
