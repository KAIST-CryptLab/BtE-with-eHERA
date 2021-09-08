#include <iostream>

#include "params.h"
#include <helib/helib.h>
#include <helib/EncryptedArray.h>
#include "BLAKE3/c/blake3.h"
#include <time.h>

using namespace helib;
using namespace std;
using namespace NTL;


// Making vector of encoded C(Matrices with all zeros except single 1) for extracting
void EncodeC(vector<ZZX>* encC_list, EncryptedArrayDerived<PA_GF2> ea2){
    long d = ea2.getDegree();
    unsigned char cc[d];
    for(int extract = 0; extract < d; extract++){
        for(int i = 0; i < d; i ++)
            cc[i] = 0;
        cc[extract] = 1;

        vector<GF2X> scratch(d);
        for(int j = 0; j < d; j++)
            GF2XFromBytes(scratch[j], &cc[j], 1);

        vector<GF2X> C;
        ea2.buildLinPolyCoeffs(C, scratch);

        vector<ZZX> encC;
        encC.resize(d);
        scratch.resize(ea2.size());
        for(int i = 0; i < d; i++){
            for(int j = 0; j < ea2.size(); j++)
                scratch[j] = C[i];
            ea2.encode(encC[i], scratch);
        }

        encC_list[extract] = encC;
    }
}

// Extract one ciphertext from 16 ciphertexts
void Extract_one(Ctxt** result_list, Ctxt ctxt, vector<ZZX>* encC_list, EncryptedArrayDerived<PA_GF2> ea2){
    int degree = ea2.getDegree();
    unsigned char cc[degree];
    Ctxt* frob_list[degree];

    frob_list[0] = new Ctxt(ctxt);
    for(int i = 1; i < degree; i ++){
        Ctxt tmp(ctxt);
        tmp.frobeniusAutomorph(i);
        frob_list[i] = new Ctxt(tmp);
    }


    vector<ZZX> encC = encC_list[0];

    result_list[0] = new Ctxt(ctxt);

    (*result_list[0]).multByConstant(encC[0]);
    for(int i = 1; i < degree; i++){
        (*frob_list[i]).multByConstant(encC[i]);
        *result_list[0] += *frob_list[i];
    }

    return;
}

// Extract 16 ciphertexts
void Extract(Ctxt** result_list, Ctxt ctxt, vector<ZZX>* encC_list, EncryptedArrayDerived<PA_GF2> ea2){
    int degree = ea2.getDegree();
    unsigned char cc[degree];
    Ctxt* frob_list[degree];

    frob_list[0] = new Ctxt(ctxt);
    for(int i = 1; i < degree; i ++){
        Ctxt tmp(ctxt);
        tmp.frobeniusAutomorph(i);
        frob_list[i] = new Ctxt(tmp);
    }

    for(int extract = 0; extract < degree; extract++){

        vector<ZZX> encC = encC_list[extract];

        result_list[extract] = new Ctxt(ctxt);

        (*result_list[extract]).multByConstant(encC[0]);
        for(int i = 1; i < degree; i++){
            (*frob_list[i]).multByConstant(encC[i]);
            *result_list[extract] += *frob_list[i];
        }
    }

    return;
}

// X^17 with HElib
void SLayer(Ctxt **ctxt_list){
    for (int i = 0; i < BLOCKSIZE; i ++){
        Ctxt buf = Ctxt(*ctxt_list[i]);
        buf.frobeniusAutomorph(THETA);
        buf.multiplyBy(*ctxt_list[i]);
        *ctxt_list[i] = buf;
    }
}

// Linear Layer with HElib
void LinearLayer(Ctxt **ctxt_list, EncryptedArrayDerived<PA_GF2> ea2){
    Ctxt *result[BLOCKSIZE];

    // ptxt_const : encoded alpha
    vector<GF2X> consts(ea2.size(), GF2X(GF2::zero()));
    for(int i = 0; i < ea2.size(); i++){
        SetX(consts[i]);
    }
    ZZX ptxt_consts;
    ea2.encode(ptxt_consts, consts);

    // MixColumn
    Ctxt *buf[BLOCKSIZE];
    for(int i = 0; i < BLOCKSIZE; i++){
        buf[i] = new Ctxt(*ctxt_list[i]);
        (*buf[i]).multByConstant(ptxt_consts);
    }

    for(int col = 0; col < 4; col++){
        for(int row = 0; row < 4; row++){
            result[(row*4) + col] = new Ctxt(*buf[(row * 4) + col]);
            *result[(row*4) + col] += *buf[(((row + 1) % 4) * 4) + col];
            *result[(row * 4) + col] += *ctxt_list[(((row + 1) % 4) * 4) + col];
            *result[(row * 4) + col] += *ctxt_list[(((row + 2) % 4) * 4) + col];
            *result[(row * 4) + col] += *ctxt_list[(((row + 3) % 4) * 4) + col];
        }
    }

    // MixRows
    for (int i = 0; i < BLOCKSIZE; i++){
        (*buf[i]).clear();
        buf[i] = new Ctxt(*result[i]);
        (*buf[i]).multByConstant(ptxt_consts);
    }

    for (int row = 0; row < 4; row++){
        for (int col = 0; col < 4; col++){
            *ctxt_list[(row * 4) + col] = *buf[(row * 4) + col];
            *ctxt_list[(row * 4) + col] += *buf[(row * 4) + ((col + 1) % 4)];
            *ctxt_list[(row * 4) + col] += *result[(row * 4) + ((col + 1) % 4)];
            *ctxt_list[(row * 4) + col] += *result[(row * 4) + ((col + 2) % 4)];
            *ctxt_list[(row * 4) + col] += *result[(row * 4) + ((col + 3) % 4)];
        }
    }
}


// Making Random Vectors
void GetRandVecPoly(uint64_t* rand_vectors, uint64_t nonce, uint64_t counter){
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    uint64_t mask = (1 << DEG) - 1;

    unsigned char buf[16];
    memcpy(buf, &nonce, 8);
    memcpy(buf + 8, &counter, 8);
    blake3_hasher_update(&hasher, buf, 8);
    uint8_t output[XOF_BYTE_COUNT + 4];
    blake3_hasher_finalize(&hasher, output, XOF_BYTE_COUNT + 4);
    int bad_cnt = 0;
    for (int i = 0; i < XOF_ELEMENT_COUNT; i++)
    {
        uint32_t tmp = *(uint32_t *) (output + 4 * i);
        if (tmp != 0)
        {
            rand_vectors[i - bad_cnt] = (uint64_t) tmp;
            rand_vectors[i - bad_cnt] = rand_vectors[i - bad_cnt] & mask;
        }
        else
            bad_cnt++;
    }

    return;
}


// Key schedule with HElib
void KeySchedule(Ctxt** round_keys, Ctxt** master_key, uint64_t* rand_vectors, int round, EncryptedArrayDerived<PA_GF2> ea2){
    for (int i = 0; i < BLOCKSIZE; i++)
    {
        uint8_t rand_vector_poly[(DEG/8 + 1)];
        for(int j = 0; j < (DEG/8 + 1); j++){
            uint64_t tmp = rand_vectors[(BLOCKSIZE * round) + i] >> (8 * j);
            rand_vector_poly[j] = (uint8_t) (tmp & 0xff);
        }
        
        GF2X const_poly = GF2XFromBytes(rand_vector_poly, (DEG/8 + 1));
        vector<GF2X> consts(ea2.size(), GF2X(GF2::zero()));
        for(int j = 0; j < ea2.size(); j++){
            consts[j] = const_poly;
        }
        ZZX ptxt_consts;
        ea2.encode(ptxt_consts, consts);
        
        round_keys[i] = new Ctxt(*master_key[i]);
        (*round_keys[i]).multByConstant(ptxt_consts);
    }

    return;
}

// eHERA
void eHERA(Ctxt** ctxt_list, Ctxt** round_keys, Ctxt** key_list, uint64_t* rand_vectors, const EncryptedArrayDerived<PA_GF2> ea2){
    for(int i = 0; i < ROUNDS; i++){
        KeySchedule(round_keys, key_list, rand_vectors, i, ea2);
        for(int j = 0; j < BLOCKSIZE; j++){
            *ctxt_list[j] += *round_keys[j];
        }
        LinearLayer(ctxt_list, ea2);
        SLayer(ctxt_list);
        
    }
    LinearLayer(ctxt_list, ea2);
    KeySchedule(round_keys, key_list, rand_vectors, ROUNDS, ea2);
    for(int j = 0; j < BLOCKSIZE; j++)
        *ctxt_list[j] += *round_keys[j];
}