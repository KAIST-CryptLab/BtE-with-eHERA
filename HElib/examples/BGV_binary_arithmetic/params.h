#ifndef __PARMS_H__
#define __PARMS_H__

#define TRUE 1
#define FALSE 0



// Parameter Preset
#define PARM_PRESET 1
#define BLOCKSIZE 16

// Par-I
#if PARM_PRESET == 1
    #define POLY_MOD_DEG 32768
    #define ROUNDS 4
    #define BLOCKSIZE 16
    #define DEG 21 // Degree of extension field
    #define MOD 5 // Modulus polynomial
    #define THETA 4 // power 2^theta+1, x -> x^17
    #define DEBUG 0

#endif

#define XOF_ELEMENT_COUNT ((ROUNDS + 1) * BLOCKSIZE)
#define XOF_BYTE_COUNT (4 * XOF_ELEMENT_COUNT)
#endif
