#include <iostream>
#include <iomanip>
#include "eHERA.h"
#include "util.h"

int main()
{
    int elem_length = 6;
    uint64_t mask = (1 << DEG) - 1;

    block_t key = block_init(BLOCKSIZE);

    // Random key generation
    for (int i = 0; i < 16; i++)
    {
        rand_bytes(&key[i], 8);
        key[i] &= mask;
    }

    // Print key vector
    cout << "Key: " << flush;
    for (int i = 0; i < BLOCKSIZE; i++)
    {
        cout << hex << setfill('0') << setw(elem_length) << key[i] << " ";
    }
    cout << dec << endl;

    uint64_t nonce = 0x123456789abcdef;
    uint64_t counter = 0;
    int repeat = 100000;

    // Initialization of eHERA
    eHera cipher(key);
    cout << "eHera init" << endl;
    cipher.init(nonce, counter);

    block_t temp = block_init(BLOCKSIZE);

    // Benchmark of key schedule
    CLOCK_START();
    for (int i = 0; i < repeat; i++)
    {
        cipher.init(nonce, counter);
    }
    CLOCK_END();
    int xof_time = NCLOCKS / repeat;
    cout << "XOF: " << xof_time << endl;

    block_t keystream = block_init(BLOCKSIZE);

    // Benchmark of encryption
    CLOCK_START();
    for (int i = 0; i < repeat; i++)
    {
        cipher.init(nonce, counter);
        cipher.crypt(keystream);
    }
    CLOCK_END();
    int latency = NCLOCKS / repeat;
    cout << "Latency: " << latency << endl;

    return 0;
}

