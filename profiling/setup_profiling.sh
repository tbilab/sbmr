# Compile everything 
g++ -std=c++11 profiling/profile.cpp Node.cpp Network.cpp SBM.cpp helpers.cpp Sampler.cpp -o profiling/profile.out

# Run profiled code
./profiling/profile.out

# Remove binariesls
rm profiling/*.out

