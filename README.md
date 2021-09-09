# BtE Transciphering Framework with eHERA
This is an implementation of the BtE transciphering framework with the eHERA cipher.
It includes the client-side encryption [eHERA_client](./eHERA_client) by eHERA, the server-side transciphering [eHERA_HE](./eHERA_HE), 
and required libraries as submodules.

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


## Install and Benchmark

To run the BtE framework with eHERA, C version of [BLAKE3](https://github.com/BLAKE3-team/BLAKE3.git) and [HElib](https://github.com/homenc/HElib.git) are required to be built.
The C version of `BLAKE3` should be built to support AVX2 intrinsics in order to benchmark properly.
Then you can make execution file(s) in both [eHERA_client](./eHERA_client) and [eHERA_HE](./eHERA_HE) directory by
```batch
make
```
The BtE transciphering with eHERA can be benchmarked by running the execution files.
