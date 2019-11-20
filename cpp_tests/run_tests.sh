echo $PWD

echo "=============================================================================\nCompiling Classes..."
echo "=============================================================================\n"

# Compile the main classes
g++ -std=c++11 -c Node.cpp SBM.cpp helpers.cpp Sampler.cpp


echo "=============================================================================\nCompiling Tests..."
echo "=============================================================================\n"
# Compile the entryway to the tests. This only needs to happen once
# g++ -std=c++11 cpp_tests/tests-main.cpp -c -o cpp_tests/tests-main.o

# Compile all the tests
g++ -std=c++11 cpp_tests/tests-main.o \
  Node.o SBM.o helpers.o Sampler.o \
  cpp_tests/tests-node.cpp \
  cpp_tests/tests-sampler.cpp \
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

