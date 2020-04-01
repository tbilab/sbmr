echo $PWD

cd src/

OPTIMIZATION_LEVEL=""
# OPTIMIZATION_LEVEL=-O2


echo "=============================================================================\nCompiling Tests..."
echo "=============================================================================\n"

# Compile the entryway to the tests. This only needs to happen once
if [ ! -f cpp_tests/tests-main.o ]; then
  echo "Building test entryway..."
  g++ -std=c++11 ${OPTIMIZATION_LEVEL} cpp_tests/tests-main.cpp -c -o cpp_tests/tests-main.o
fi


# Compile all the tests
g++ -std=c++11 ${OPTIMIZATION_LEVEL} -DNO_RCPP=1\
  cpp_tests/tests-main.o \
  cpp_tests/tests-node.cpp \
  cpp_tests/tests-sampler.cpp \
  cpp_tests/tests-sbm_network.cpp \
  cpp_tests/tests-Block_Concensus.cpp \
  cpp_tests/tests-mcmc_sweep.cpp \
  cpp_tests/tests-sbm_network_algorithms.cpp \
  cpp_tests/tests-agglomerative_merge.cpp \
  -o cpp_tests/run_tests.o 


echo "=============================================================================\nRunning Tests..."
echo "=============================================================================\n"

# Run tests
./cpp_tests/run_tests.o -d yes
