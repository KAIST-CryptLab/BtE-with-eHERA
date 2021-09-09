#include "util.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

static unsigned char rxbuf[256];
static size_t rxcnt = 0;

uint64_t rdtsc()
{
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

uint64_t RDTSC_START()
{
    unsigned cyc_high, cyc_low;
    __asm volatile("" ::: /* pretend to clobber */ "memory");             \
    __asm volatile(
            "cpuid\n\t"
            "rdtsc\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            : "=r"(cyc_high), "=r"(cyc_low)::"%rax", "%rbx", "%rcx", "%rdx");
    return ((uint64_t)cyc_high << 32) | cyc_low;
}

uint64_t RDTSC_END()
{
    unsigned cyc_high, cyc_low;
    __asm volatile(
            "rdtscp\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            "cpuid\n\t"
            : "=r"(cyc_high), "=r"(cyc_low)::"%rax", "%rbx", "%rcx", "%rdx");
    return ((uint64_t)cyc_high << 32) | cyc_low;
}

void rand_bytes(void* buf, size_t sz)
{
    size_t idx = 0;
    size_t cnt = 0;
    while (sz > 0)
    {
        if (rxcnt == 0)
        {
            if (getrandom(rxbuf, 256, 0) != 256)
            {
                puts("Invalid random");
                exit(-1);
            }

            rxcnt = 256;
        }

        cnt = sz < rxcnt ? sz : rxcnt;

        memcpy((uint8_t *)buf + idx, rxbuf + (256 - rxcnt), cnt);
        sz -= cnt;
        rxcnt -= cnt;
        idx += cnt;
    }
}


