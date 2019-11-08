echo $PWD


echo "========================================\nCompiling files..."

# Compile files so we can link
g++ -std=c++11 -c Node.cpp SBM.cpp helpers.cpp Weighted_Sampler.cpp

# g++ -std=c++11 -c Node.cpp helpers.cpp


echo "========================================\nTesting Node.cpp"
g++ -std=c++11 \
  -isystem /home/nick/cpp_dev/googletest/googletest/include/ \
  -pthread cpp_tests/test_node.cpp Node.o helpers.o\
  /home/nick/cpp_dev/gtest_build/lib/libgtest.a -o my_googletest
./my_googletest


echo "========================================\nTesting SBM.cpp"
g++ -std=c++11 \
  -isystem /home/nick/cpp_dev/googletest/googletest/include/ \
  -pthread cpp_tests/test_sbm.cpp Node.o SBM.o helpers.o Weighted_Sampler.o\
  /home/nick/cpp_dev/gtest_build/lib/libgtest.a \
  -o my_googletest
./my_googletest


echo "========================================\nTesting Weighted_Sampler.cpp"
g++ -std=c++11 \
  -isystem /home/nick/cpp_dev/googletest/googletest/include/ \
  -pthread cpp_tests/test_weighted_sampler.cpp Weighted_Sampler.o helpers.o\
  /home/nick/cpp_dev/gtest_build/lib/libgtest.a -o my_googletest
./my_googletest


# Cleanup the compiled files so RCpp doesn't get mad
rm *.o