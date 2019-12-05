OPTIMIZATION_LEVEL=-O2

# Compile everything and tell compiler to enable profiling
g++ -std=c++11 ${OPTIMIZATION_LEVEL} profiling/profile.cpp \
    -DPROFILING=1 \
    Node.cpp Network.cpp SBM.cpp helpers.cpp Sampler.cpp \
    profiling/Instrument.h 

# Run profiled code
./a.out

# Remove binaries
rm ./a.out

