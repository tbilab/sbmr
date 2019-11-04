echo $PWD


# Compile files so we can link
g++ -std=c++11 -c Node.cpp SBM.cpp


echo "========================================\nTesting Node.cpp"

g++ -std=c++11 \
  -isystem /home/nick/cpp_dev/googletest/googletest/include/ \
  -pthread cpp_tests/test_node.cpp Node.o\
  /home/nick/cpp_dev/gtest_build/lib/libgtest.a -o my_googletest
./my_googletest


echo "========================================\nTesting SBM.cpp"


g++ -std=c++11 \
  -isystem /home/nick/cpp_dev/googletest/googletest/include/ \
  -pthread cpp_tests/test_sbm.cpp Node.o SBM.o\
  /home/nick/cpp_dev/gtest_build/lib/libgtest.a \
  -o my_googletest 
./my_googletest


# Cleanup the compiled files so RCpp doesn't get mad
rm *.o