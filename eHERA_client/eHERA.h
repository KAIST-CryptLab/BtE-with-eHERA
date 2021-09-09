#ifndef __eHERA_H__
#define __eHERA_H__

#include <cstring>
#include <vector>
#include "parms.h"

using namespace std;

typedef uint64_t* block_t;

block_t block_init(size_t sz);

uint64_t clmul(uint64_t a, uint64_t b);
uint64_t reduction(uint64_t a);
void linear(block_t state);


/*
Provides functionality of cipher eHERA. It takes nonce and counter as
inputs, and the nonce is assumed not to be reused.
 */
class eHera
{
    public:
        /*
         Create a eHERA instance initialized with secret key
         @param[in] key The secret key
         */
        eHera(uint64_t *key)
        {
            key_ = block_init(BLOCKSIZE);
            for (int i = 0; i < BLOCKSIZE; i++)
            {
                key_[i] = key[i];
            }
            rand_vectors_ = block_init(XOF_ELEMENT_COUNT + 8);
            round_keys_ = block_init(XOF_ELEMENT_COUNT);
        }

        // Destruct an eHERA instance
        ~eHera()
        {
            free(key_);
            free(rand_vectors_);
        }

        // Re-keying function
        void set_key(uint64_t *key);

        /*
        The init function computes round keys from
        extendable output function.
        @param[in] nonce Distinct nonce
        @param[in] counter Counter, but may be used as an integrated nonce
         */
        void init(uint64_t nonce, uint64_t counter);

        /*
        Both crypt and crypt_naive compute a block of HERA. crypt
        function is an AVX2 implementation while crypt_naive function
        is a reference code of HERA.
        @param[out] out Keystream of HERA
         */
        void crypt(block_t out);
        void crypt_naive(block_t out);

        // Copying outputs of XOF
        void get_rand_vectors(uint64_t *output);
        // Copying round keys
        void get_round_keys(uint64_t *output);

        void add_round_key(block_t out, int round);

        uint64_t nonce_;
        uint64_t counter_;

    private:
        // Secret key
        block_t key_;

        // Round constants
        block_t rand_vectors_;

        // Key multiplied by round constant
        block_t round_keys_;

        // The inner key schedule function in init and update
        void keyschedule();
};

#endif
