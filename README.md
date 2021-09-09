# BtE Transciphering Framework with eHERA
This is an implementation of the BtE transciphering framework with the eHERA cipher.
It includes the client-side encryption [eHERA_client](./eHERA_client) by eHERA, and the server-side transciphering [eHERA_HE](./eHERA_HE).

## Client-side

[eHERA_client](./eHERA_client) contains a client-side implementation of the BtE framework:
pure C++ implementation of the eHERA cipher, and
an AVX2-optimized C++ implementation of the eHERA cipher, whose XOF is instantiated by [BLAKE3](https://github.com/BLAKE3-team/BLAKE3.git).
Parameter presets are given in [parms.h](./eHERA_client/parms.h).

## Server-side

[eHERA_HE](./eHERA_HE) is a server-side implementation of the BtE framework.
The BtE framework is implemented using [HElib](https://github.com/homenc/HElib.git).
The server-side homomorphically evaluates eHERA keystream under BGV scheme to recover the original data, and extracts digits.
Parameter presets are given in [parms.h](./eHERA_HE/parms.h).


## Dependency

To run the BtE framework with eHERA, C version of [BLAKE3](https://github.com/BLAKE3-team/BLAKE3.git) version 1.0.0.
and [HElib](https://github.com/homenc/HElib.git) version 2.1.0. are required to be built.
The C version of `BLAKE3` should be built to support AVX2 intrinsics in order to benchmark properly.

