#include <iostream>

#include "params.h"
#include <helib/helib.h>
#include <helib/EncryptedArray.h>
#include "../../../BLAKE3/c/blake3.h"

using namespace helib;
using namespace std;
using namespace NTL;

//X^17 with HElib
void SLayer(Ctxt **ctxt_list){
    Ctxt buf(*ctxt_list[0]);
    for (int i = 0; i < BLOCKSIZE; i ++){
        buf = Ctxt(*ctxt_list[i]);
        buf.frobeniusAutomorph(THETA);
        buf *= *ctxt_list[i];
        *ctxt_list[i] = buf;
    }
}

//Linear Layer with HElib
void LinearLayer(Ctxt **ctxt_list, EncryptedArrayDerived<PA_GF2> ea2){
    Ctxt *result[BLOCKSIZE];

    //ptxt_const : encoded alpha
    vector<GF2X> consts(ea2.size(), GF2X(GF2::zero()));
    for(int i = 0; i < ea2.size(); i++){
        SetX(consts[i]);
    }
    ZZX ptxt_consts;
    ea2.encode(ptxt_consts, consts);

    //MixColumn
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

    //MixRows
    Ctxt *buf2[16];
    for (int i = 0; i < BLOCKSIZE; i++){
        buf2[i] = new Ctxt(*result[i]);
        (*buf2[i]).multByConstant(ptxt_consts);
    }

    for (int row = 0; row < 4; row++){
        for (int col = 0; col < 4; col++){
            *ctxt_list[(row * 4) + col] = *buf2[(row * 4) + col];
            *ctxt_list[(row * 4) + col] += *buf2[(row * 4) + ((col + 1) % 4)];
            *ctxt_list[(row * 4) + col] += *result[(row * 4) + ((col + 1) % 4)];
            *ctxt_list[(row * 4) + col] += *result[(row * 4) + ((col + 2) % 4)];
            *ctxt_list[(row * 4) + col] += *result[(row * 4) + ((col + 3) % 4)];
        }
    }
}

//Key schedule with HElib
void KeySchedule(Ctxt** round_keys, Ctxt** master_key, uint64_t nonce, uint64_t counter, EncryptedArrayDerived<PA_GF2> ea2){
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    uint64_t mask = (1 << DEG) - 1;

    unsigned char buf[8];
    memcpy(buf, &nonce, 4);
    memcpy(buf + 4, &counter, 4);
    blake3_hasher_update(&hasher, buf, 8);
    uint8_t output[XOF_BYTE_COUNT];
    blake3_hasher_finalize(&hasher, output, XOF_BYTE_COUNT);
    uint64_t* rand_vectors;
    for (int i = 0; i < XOF_ELEMENT_COUNT; i++)
    {
        rand_vectors[i] = (uint64_t) *(uint32_t *) (output + 4 * i);
        rand_vectors[i] = rand_vectors[i] & mask;
    }

    for (int i = 0; i < XOF_ELEMENT_COUNT; i++)
    {
        uint8_t *rand_vector_poly = (uint8_t*) &rand_vectors[i];
        GF2X const_poly = GF2XFromBytes(rand_vector_poly, 8);
        vector<GF2X> consts(ea2.size(), GF2X(GF2X::zero()));
        for(int j = 0; j < ea2.size(); j++){
            consts[i] = const_poly;
        }
        ZZX ptxt_consts;
        ea2.encode(ptxt_consts, consts);
        *round_keys[i] = Ctxt(*master_key[i % BLOCKSIZE]);
        (*round_keys[i]).multByConstant(ptxt_consts);
    }
    return;
}

int main(int argc, char* argv[]){

    unsigned long p = 2;
    unsigned long m = 42799;
    unsigned long r = 1;
    unsigned long bits = 500;
    unsigned long c = 2;

    cout << "Initialising context object..." << endl;

    Context context = ContextBuilder<BGV>()
                                    .m(m)
                                    .p(p)
                                    .r(r)
                                    .bits(bits)
                                    .c(c)
                                    .build();

    context.printout();
    cout << endl;

    cout << "Creating secret key..." << endl;
    SecKey secret_key(context);
    secret_key.GenSecKey();

    cout << "Generating key-switching matrices..." << endl;
    addSome1DMatrices(secret_key);
    addFrbMatrices(secret_key);
    const PubKey& public_key = secret_key;
    cout << "Building Encrypted Array..." << endl;
    PAlgebraMod pAlgMod = context.getAlMod();
    PAlgebraModDerived<PA_GF2> pAlgModDer(pAlgMod.getZMStar(), 1);
    // PA_GF2::RX factor = pAlgModDer.getFactors()[0];
    uint64_t GInt = 2097157;
    uint8_t* GPolyBytes = (uint8_t*) &GInt;
    PA_GF2::RX factor = GF2XFromBytes(GPolyBytes, 3);
    const EncryptedArrayDerived<PA_GF2> ea2(context, factor, context.getAlMod());

    long nslots = ea2.size();
    std::cout << "Number of slots: " << nslots << std::endl;

    long d = ea2.getDegree();
    std::cout << "Degree of Modulus G: " << d << std::endl;

    std::vector<NTL::GF2X> data(nslots, NTL::GF2X(11, NTL::GF2::zero()));

    NTL::ZZX ptxt_11;

    std::cout<<"Creating Ctxt vector..."<<std::endl;
    helib::Ctxt *ctxt_list[16];
    for(int i = 0; i < 16; i++){
        for (int j = 0; j < nslots; j++)
            data[j] = NTL::GF2::zero();
        ctxt_list[i] = new helib::Ctxt(public_key);
        for (int j = 0; j < nslots; j++)
            SetCoeff(data[j], (i%5));
        std::cout << "res[" << i << "]: " << data[0] << std::endl;
        ea2.encode(ptxt_11,data);
        public_key.Encrypt(*ctxt_list[i], ptxt_11);
    }


    return 0;
}