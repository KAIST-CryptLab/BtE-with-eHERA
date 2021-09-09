# BtE Transciphering Framework with eHERA
This is an implementation of the BtE transciphering framework with the eHERA cipher using `HElib` library.
It includes the client-side encryption [eHERA_client](./eHERA_client) by eHERA, the server-side transciphering [eHERA_HE](./eHERA_HE), 
and required libraries as submodules.

## Install and Benchmark

To run the BtE framework with eHERA, C version of [BLAKE3](https://github.com/BLAKE3-team/BLAKE3.git) and [HElib](https://github.com/homenc/HElib.git) are required to be built.
The C version of `BLAKE3` should be built to support AVX2 intrinsics in order to benchmark properly.
Then you can make execution file(s) in both [eHERA_client](./eHERA_client) and [eHERA_HE](./eHERA_HE) directory by
```batch
make
```
The BtE transciphering with eHERA can be benchmarked by running the execution files.
