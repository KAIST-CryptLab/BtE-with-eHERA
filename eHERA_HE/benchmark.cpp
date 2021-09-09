#include <iostream>

#include "parms.h"
#include <helib/helib.h>
#include <helib/EncryptedArray.h>
#include "eHERA_HElib.h"
#include <time.h>

using namespace helib;
using namespace std;
using namespace NTL;


int main(int argc, char* argv[]){

    clock_t start, end;
    unsigned long p = 2;
    unsigned long m = 42799;
    unsigned long r = 1;
    unsigned long bits = 244;
    switch (ROUNDS)
    {
    case 6:
        bits = 300;
        break;
    case 7:
        bits = 344;
        break;
    case 8:
        bits = 384;
        break;
    default:
        break;
    }
    unsigned long c = 2;
    uint64_t nonce = 0x123456789abcdef;
    uint64_t counter = 0;

    // Initialising context object...

    Context context = ContextBuilder<BGV>()
                                    .m(m)
                                    .p(p)
                                    .r(r)
                                    .bits(bits)
                                    .c(c)
                                    .build();

    context.printout();
    cout << "Bit Size of Ctxt Primes = " << ceil(context.logOfProduct(context.getCtxtPrimes()) / log(2.0)) << endl;
    cout << endl;

    // Creating secret key...
    SecKey secret_key(context);
    secret_key.GenSecKey();

    // Generating key-switching matrices...
    addSome1DMatrices(secret_key);
    addFrbMatrices(secret_key);


    const PubKey& public_key = secret_key;
    // Building Encrypted Array...
    PAlgebraMod pAlgMod = context.getAlMod();
    PAlgebraModDerived<PA_GF2> pAlgModDer(pAlgMod.getZMStar(), 1);
    uint64_t GInt = 2097157;
    uint8_t GPolyBytes[8];
    for(int i = 0; i < 8; i++){
        uint64_t tmp = GInt >> (8 * i);
        GPolyBytes[i] = (uint8_t) (tmp & 0xff);
    }
    PA_GF2::RX factor = GF2XFromBytes(GPolyBytes, 8);
    const EncryptedArrayDerived<PA_GF2> ea2(context, factor, context.getAlMod());

    long nslots = ea2.size();

    long d = ea2.getDegree();

    vector<GF2X> data(nslots, GF2X(GF2::zero()));

    ZZX ptxt_11;

    // Creating key vector...
    Ctxt* key_list[BLOCKSIZE];
    for(int i = 0; i < BLOCKSIZE; i++){
        uint64_t buf = i;
        uint8_t buf_bytes[8];
        for (int j = 0; j < 8; j++)
            buf_bytes[j] = (uint8_t)((buf >> (8 * j)) & 0xff);
        for (int j = 0; j < nslots; j++)
            data[j] = GF2XFromBytes(buf_bytes, 8);
        key_list[i] = new Ctxt(public_key);
        ea2.encode(ptxt_11, data);
        public_key.Encrypt(*key_list[i], ptxt_11);
    }

    // Creating Ctxt vector...
    Ctxt* ctxt_list[BLOCKSIZE];
    for(int i = 0; i < BLOCKSIZE; i++){
        uint64_t buf = i + 1;
        uint8_t buf_bytes[8];
        for (int j = 0; j < 8; j++)
            buf_bytes[j] = (uint8_t)((buf >> (8 * j)) & 0xff);
        for (int j = 0; j < nslots; j++)
            data[j] = GF2XFromBytes(buf_bytes, 8);
        ctxt_list[i] = new Ctxt(public_key);
        ea2.encode(ptxt_11, data);
        public_key.Encrypt(*ctxt_list[i], ptxt_11);
    }

    // Creating Random vector...
    uint64_t* rand_vectors = static_cast<uint64_t*>
        (aligned_alloc(32, sizeof(uint64_t) * (XOF_ELEMENT_COUNT + 8)));
    memset(rand_vectors, 0, sizeof(uint64_t) * (XOF_ELEMENT_COUNT + 8));
    getRandVecPoly(rand_vectors, nonce, counter);


    ZZX dec_ff;
    vector<GF2X> res;
    Ctxt* round_keys[BLOCKSIZE];
    ZZ number;
    cout << " - noise budget: " << (*ctxt_list[0]).capacity() << endl;
    start = clock();
    eHERA(ctxt_list, round_keys, key_list, rand_vectors, ea2);
    end = clock();
    cout << "After Full eHERA, time: " << ((double)(end - start)) / CLOCKS_PER_SEC << "s." << endl;

    //Making 1 symmetric cipher ptxt
    vector<GF2X> consts_symmetry(ea2.size(), GF2X(GF2::zero()));
    for(int j = 0; j < ea2.size(); j++){
        SetX(consts_symmetry[j]);
    }

    ZZX ptxt_symmetry;
    start = clock();
    ea2.encode(ptxt_symmetry, consts_symmetry);
    (*ctxt_list[0]).addConstant(ptxt_symmetry);
    end = clock();

    cout << "Encoding & Adding 1 symmetric ptxt : " << ((double)(end - start)) / CLOCKS_PER_SEC << "s" << endl;

    for(int ctxt_index = 0; ctxt_index < BLOCKSIZE; ctxt_index++){
        ea2.encode(ptxt_symmetry, consts_symmetry);
        (*ctxt_list[ctxt_index]).addConstant(ptxt_symmetry);
    }
    end = clock();
    cout << "Encoding & Adding 16 symmetric ptxt :" << ((double)(end - start)) / CLOCKS_PER_SEC << "s" << endl;

    Ctxt* extract_result[d];
    vector<ZZX> encC_list[d];

    //Making vector of encoded C(Matrices with all zeros except single 1) for extracting
    buildLinPolyExtract(encC_list, ea2);

    //Extracting a bit from a ctxt
    start = clock();
    extractOne(extract_result, *ctxt_list[0], encC_list, ea2);
    end = clock();
    cout << "Extracting 1 ctxt's 1 bits :" << ((double)(end - start)) / CLOCKS_PER_SEC << "s" << endl;

    //Extracting 16 ctxt's 21 bits
    start = clock();
    for(int ctxt_index = 0; ctxt_index < 1; ctxt_index++){
        extract(extract_result, *ctxt_list[ctxt_index], encC_list, ea2);
    }
    end = clock();
    cout << "Extracting 16 ctxt's 21 bits :" << ((double)(end - start)) / CLOCKS_PER_SEC << "s" << endl;
    cout << "Final noise budget: " << (*extract_result[0]).capacity() << endl;

    return 0;
}