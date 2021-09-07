BLAKEPATH = BLAKE3/c/
BLAKEHEADER = $(BLAKEPATH)blake3.h
LIBBLAKE = -L$(BLAKEPATH) -lblake3

HELIBPATH = helib_pack/
HELIBHEADERS = $(HELIBPATH)include
LIBHELIB = -L$(HELIBPATH)lib/ -lhelib

SOURCE = eHERA_HElib.cpp benchmark.cpp
HEADER = eHERA_HElib.h params.h

LIBPATH = /usr/local/lib

CXXOPTS = -O3 -I$(HELIBHEADERS) -std=c++17
LDOPTS = $(LIBBLAKE) $(LIBHELIB) -L$(LIBPATH) -lgmp -lntl -lpthread


all: benchmark

benchmark: benchmark.cpp eHERA_HElib.cpp eHERA_HElib.h params.h
		$(CXX) $(CXXOPTS) benchmark.cpp eHERA_HElib.cpp $(LDOPTS) -o benchmark -march=native -mavx2

clean: 
	rm benchmark