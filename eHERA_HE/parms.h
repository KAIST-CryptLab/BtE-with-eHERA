#ifndef __PARMS_H__
#define __PARMS_H__

#define TRUE 1
#define FALSE 0



// Parameter Preset
#define PARM_PRESET 1
#define BLOCKSIZE 16
#define DEG 21 // Degree of extension field
#define MOD 5 // Modulus polynomial
#define THETA 4 // power 2^theta+1, x -> x^17



// Par-I
#if PARM_PRESET == 1
    #define ROUNDS 4

#elif PARM_PRESET == 2
    #define ROUNDS 6

#elif PARM_PRESET == 3
    #define ROUNDS 7

#else
    #define ROUNDS 8

#endif

#define XOF_ELEMENT_COUNT ((ROUNDS + 1) * BLOCKSIZE)
#define XOF_BYTE_COUNT (4 * XOF_ELEMENT_COUNT)
#endif
