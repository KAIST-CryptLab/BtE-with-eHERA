all: cycle_naive cycle_simd

cycle_naive: cycle_count.cpp eHERA.cpp parms.h eHERA.h util.c util.h
	g++ -O3 -o cycle_naive cycle_count.cpp eHERA.cpp util.c -std=c++17 -march=native -mno-avx -mno-sse2 -mno-sse4 -lblake3

cycle_simd: cycle_simd.cpp eHERA_simd.cpp parms.h eHERA.h util.c util.h
	g++ -O3 -o cycle_simd cycle_simd.cpp eHERA_simd.cpp util.c -std=c++17 -march=native -mno-avx512f -lblake3


clean:
	rm -f cycle_naive
	rm -f cycle_simd
