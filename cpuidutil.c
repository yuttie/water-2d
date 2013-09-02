/*
 *******************************************************************************
 *
 *  cpuidutil.c - *
 *
 *  Copyright (C) 2004-2004 Yuta Taniguchi
 *******************************************************************************
 */


/* Include Files*/
#include <stdbool.h>
#include "cpuidutil.h"

#define IS_64BIT_CPU defined(__x86_64__) || defined(_M_AMD64) || defined (_M_X64)


inline bool CheckFeature(unsigned int i);


/* CPUID命令の有無確認関数 */
bool CheckCPUID() {
    bool b;
    asm volatile (
#if IS_64BIT_CPU
        "pushfq\n\t"
        "pushfq\n\t"
        "popq %%rax\n\t"
        "movq %%rax, %%rcx\n\t"
        "xorq $0x200000, %%rax\n\t"
        "pushq %%rax\n\t"
        "popfq\n\t"
        "pushfq\n\t"
        "popq %%rax\n\t"
        "popfq\n\t"
        "cmpq %%rcx, %%rax\n\t"
#else
        "pushfl\n\t"
        "pushfl\n\t"
        "popl %%eax\n\t"
        "movl %%eax, %%ecx\n\t"
        "xorl $0x200000, %%eax\n\t"
        "pushl %%eax\n\t"
        "popfl\n\t"
        "pushfl\n\t"
        "popl %%eax\n\t"
        "popfl\n\t"
        "cmpl %%ecx, %%eax\n\t"
#endif
        "jz false\n\t"
    "true:\n\t"
        "movb $0x1, %[result]\n\t"
        "jmp exit\n\t"
    "false:\n\t"
        "movb $0x0, %[result]\n\t"
    "exit:\n\t"
        : [result] "=r"(b)
    );
    return b;
}

/* CPUID命令で得られるEDXレジスタを返す関数 */
unsigned int GetEDX() {
    unsigned int x = 1;
    asm volatile (
        "cpuid\n\t"
        : "=d" (x)
        : "a" (x)
        : "%ebx", "%ecx"
    );
    return x;
}

/* featureの有無確認関数 */
bool CheckFeature(unsigned int i) {
    bool x = 1;
    asm volatile (
        "cpuid\n\t"
        "btl %[i], %%edx\n\t"
        "setc %[result]\n\t"
        : [result] "=r"(x)
        : "a"(x), [i] "r"(i)
        : "%ebx", "%ecx", "%edx"
    );
    return x;
}

/* MMX命令の有無確認関数 */
bool CheckMMX() { return CheckFeature(23); }

/* SSE命令の有無確認関数 */
bool CheckSSE() { return CheckFeature(25); }

/* SSE2命令の有無確認関数 */
bool CheckSSE2() { return CheckFeature(26); }
