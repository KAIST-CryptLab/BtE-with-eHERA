# BtE Transciphering Framework with eHERA
This is an implementation of the server-side BtE transciphering framework with the eHERA cipher using `HElib` library.

## Install

You need [BLAKE3](https://github.com/BLAKE3-team/BLAKE3.git) and [HElib](https://github.com/homenc/HElib.git) to make.
After building and installing `BLAKE3` and `HElib`, you should change library path in 1, 5-th line of [Makefile](./eHERA_HE/Makefile).
```batch
BLAKEPATH = <BLAKE3 directory>/c
HELIBPATH = <HElib directory>
```
Then you can make in [eHERA_HE](./eHERA_HE) directory.
```batch
make
```


## Benchmark

You can run the benchmark of server-side BtE transciphering with eHERA using [benchmark.cpp](./eHERA_HE/benchmark.cpp).
The benchmark parameters are given in [params.h](./eHERA_HE/params.h).
To benchmark all the parameters, run the following command in [eHERA_HE](./eHERA_HE) directory.

```batch
./benchmark
```