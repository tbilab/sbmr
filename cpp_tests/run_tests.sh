echo $PWD

# Set location of the google test library
GTEST_LIB=/Users/nick/cpp_code/googletest/googletest/include/
LIBGTEST=/Users/nick/cpp_code/googletest/install/lib/libgtest.a

echo "=============================================================================\nCompiling files..."
echo "=============================================================================\n"
# Compile files so we can link
g++ -std=c++11 -c Node.cpp SBM.cpp helpers.cpp Sampler.cpp


echo "=============================================================================\nTesting Sampler.cpp"
echo "=============================================================================\n"
g++ -std=c++11 \
  -isystem ${GTEST_LIB} \
  -pthread cpp_tests/test_sampler.cpp Sampler.o helpers.o Node.o\
  ${LIBGTEST} -o my_googletest
./my_googletest


echo "=============================================================================\nTesting Node.cpp"
echo "=============================================================================\n"
g++ -std=c++11 \
  -isystem ${GTEST_LIB} \
  -pthread cpp_tests/test_node.cpp Node.o helpers.o\
  ${LIBGTEST} -o my_googletest
./my_googletest


echo "=============================================================================\nTesting SBM.cpp"
echo "=============================================================================\n"
g++ -std=c++11 \
  -isystem ${GTEST_LIB} \
  -pthread cpp_tests/test_sbm.cpp Node.o SBM.o helpers.o Sampler.o\
  ${LIBGTEST} -o my_googletest
./my_googletest


# Cleanup the compiled files so RCpp doesn't get mad
rm *.o
