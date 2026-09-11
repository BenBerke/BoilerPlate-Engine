//
// Created by berke on 9/11/2026.
//

#ifndef BPENGINE_TYPEDEFS_H
#define BPENGINE_TYPEDEFS_H

#ifndef BP_NO_TYPDEFS

typedef unsigned char bool;
#define true 1
#define false 0

#ifndef __cplusplus
#define null ((void *)0)
#else
#ifndef _WIN64
#define null 0
#else
#define null 0LL
#endif
#endif

#ifdef _WIN32
typedef signed char i8;
typedef unsigned char u8;
typedef signed short i16;
typedef unsigned short u16;
typedef signed int i32;
typedef unsigned int u32;
typedef signed long long i64;
typedef unsigned long long u64;

#ifdef _WIN64
typedef unsigned long long usize;
typedef signed long long isize;
#endif

#else
typedef unsigned int usize;
typedef signed int isize;
#endif

#endif

#endif //BPENGINE_TYPEDEFS_H