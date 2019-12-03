OPTIMIZATION_LEVEL=-O2

# Compile everything 
g++ -std=c++11 ${OPTIMIZATION_LEVEL} profiling/profile.cpp Node.cpp Network.cpp SBM.cpp helpers.cpp Sampler.cpp profiling/Instrument.h 

# Run profiled code
./a.out

# Remove binaries
rm ./a.out

