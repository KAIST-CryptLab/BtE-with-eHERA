#include <iostream>

#include "params.h"
#include <helib/helib.h>
#include <helib/EncryptedArray.h>
#include "BLAKE3/c/blake3.h"
#include <time.h>

using namespace helib;
using namespace std;
using namespace NTL;

void EncodeC(vector<ZZX>* encC_list, EncryptedArrayDerived<PA_GF2> ea2);
void Extract_one(Ctxt** result_list, Ctxt ctxt, vector<ZZX>* encC_list, EncryptedArrayDerived<PA_GF2> ea2);
void Extract(Ctxt** result_list, Ctxt ctxt, vector<ZZX>* encC_list, EncryptedArrayDerived<PA_GF2> ea2);
void SLayer(Ctxt **ctxt_list);
void LinearLayer(Ctxt **ctxt_list, EncryptedArrayDerived<PA_GF2> ea2);
void GetRandVecPoly(uint64_t* rand_vectors, uint64_t nonce, uint64_t counter);
void KeySchedule(Ctxt** round_keys, Ctxt** master_key, uint64_t* rand_vectors, int round, EncryptedArrayDerived<PA_GF2> ea2);
void eHERA(Ctxt** ctxt_list, Ctxt** round_keys, Ctxt** key_list, uint64_t* rand_vectors, const EncryptedArrayDerived<PA_GF2> ea2);