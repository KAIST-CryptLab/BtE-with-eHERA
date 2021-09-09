#ifndef UTIL_H
#define UTIL_H

#include <sys/random.h>
#include <stdint.h>

static uint64_t NCLOCKS_START;
static uint64_t NCLOCKS_END;
static uint64_t NCLOCKS;

#if 0
#define CLOCK_START()                                                         \
    do {                                                                      \
        register unsigned cyc_high, cyc_low;                                  \
        __asm volatile("" ::: /* pretend to clobber */ "memory");             \
        __asm volatile(                                                       \
            "cpuid\n\t"                                                       \
            "rdtsc\n\t"                                                       \
            "mov %%edx, %0\n\t"                                               \
            "mov %%eax, %1\n\t"                                               \
            : "=r"(cyc_high), "=r"(cyc_low)::"%rax", "%rbx", "%rcx", "%rdx"); \
        (NCLOCKS_START) = ((uint64_t)cyc_high << 32) | cyc_low;               \
    } while (0)

#define CLOCK_END()                                                           \
    do {                                                                      \
        register unsigned cyc_high, cyc_low;                                  \
        __asm volatile(                                                       \
            "rdtscp\n\t"                                                      \
            "mov %%edx, %0\n\t"                                               \
            "mov %%eax, %1\n\t"                                               \
            "cpuid\n\t"                                                       \
            : "=r"(cyc_high), "=r"(cyc_low)::"%rax", "%rbx", "%rcx", "%rdx"); \
        (NCLOCKS_END) = ((uint64_t)cyc_high << 32) | cyc_low;                 \
        (NCLOCKS) = (NCLOCKS_END) - (NCLOCKS_START);                          \
    } while (0)

#else
#define CLOCK_START() {NCLOCKS = RDTSC_START();}
#define CLOCK_END() {NCLOCKS = RDTSC_END() - NCLOCKS;}
#endif

uint64_t RDTSC_START();
uint64_t RDTSC_END();
void rand_bytes(void*, size_t);

#endif /* UTIL_H */
