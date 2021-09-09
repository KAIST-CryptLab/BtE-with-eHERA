BLAKEPATH = ../BLAKE3/c/
BLAKEHEADER = $(BLAKEPATH)/blake3.h
LIBBLAKE = -L$(BLAKEPATH) -lblake3






all: cycle_naive cycle_simd

cycle_naive: cycle_count.cpp eHERA.cpp parms.h eHERA.h util.c util.h
	g++ -O3 -o cycle_naive cycle_count.cpp eHERA.cpp util.c -std=c++17 -march=native -mno-avx -mno-sse2 -mno-sse4 $(LIBBLAKE)

cycle_simd: cycle_simd.cpp eHERA_simd.cpp parms.h eHERA.h util.c util.h
	g++ -O3 -o cycle_simd cycle_simd.cpp eHERA_simd.cpp util.c -std=c++17 -march=native -mno-avx512f $(LIBBLAKE)


clean:
	rm -f cycle_naive
	rm -f cycle_simd
