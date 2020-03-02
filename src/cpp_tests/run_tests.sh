echo $PWD

cd src/
echo "=============================================================================\nCompiling Classes..."
echo "=============================================================================\n"

OPTIMIZATION_LEVEL=""
# OPTIMIZATION_LEVEL=-O2

# Compile the main classes
g++ -std=c++17 ${OPTIMIZATION_LEVEL} -c \
  -DNO_RCPP=1 \
  Node.cpp Block_Consensus.cpp SBM.cpp Sampler.cpp 


echo "=============================================================================\nCompiling Tests..."
echo "=============================================================================\n"

# Compile the entryway to the tests. This only needs to happen once
if [ ! -f cpp_tests/tests-main.o ]; then
  echo "Building test entryway..."
  g++ -std=c++17 ${OPTIMIZATION_LEVEL} cpp_tests/tests-main.cpp -c -o cpp_tests/tests-main.o
fi


# Compile all the tests
g++ -std=c++17 ${OPTIMIZATION_LEVEL} -DNO_RCPP=1\
  cpp_tests/tests-main.o \
  Node.o SBM.o Sampler.o Block_Consensus.o \
  cpp_tests/tests-node.cpp \
  cpp_tests/tests-edge.cpp \
  cpp_tests/tests-sampler.cpp \
  cpp_tests/tests-network.cpp \
  cpp_tests/tests-sbm.cpp \
  -o cpp_tests/run_tests.o 



echo "=============================================================================\nRunning Tests..."
echo "=============================================================================\n"

# Run tests
./cpp_tests/run_tests.o -d yes



echo "=============================================================================\nCleaning Up..."
echo "=============================================================================\n"
# Cleanup the compiled classes so RCpp doesn't get mad
rm *.o

