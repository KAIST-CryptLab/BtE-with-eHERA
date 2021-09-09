#ifndef UTIL_H
#define UTIL_H

#include <sys/random.h>
#include <stdint.h>

static uint64_t NCLOCKS_START;
static uint64_t NCLOCKS_END;
static uint64_t NCLOCKS;


#define CLOCK_START() {NCLOCKS = RDTSC_START();}
#define CLOCK_END() {NCLOCKS = RDTSC_END() - NCLOCKS;}


uint64_t RDTSC_START();
uint64_t RDTSC_END();
void rand_bytes(void*, size_t);

#endif /* UTIL_H */
