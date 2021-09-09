LDOPTS = -lgmp -lntl -lpthread -lhelib -lblake3

all: benchmark

benchmark: benchmark.cpp eHERA_HElib.cpp eHERA_HElib.h parms.h
		$(CXX) -O3 -std=c++17 benchmark.cpp eHERA_HElib.cpp $(LDOPTS) -o benchmark -march=native -mavx2

clean: 
	rm benchmark