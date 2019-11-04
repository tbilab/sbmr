echo $PWD

echo "========================================\nTesting Node.cpp"

g++ -std=c++11 \
  -isystem /home/nick/cpp_dev/googletest/googletest/include/ \
  -pthread cpp_tests/test_node.cpp\
  /home/nick/cpp_dev/gtest_build/lib/libgtest.a -o my_googletest
./my_googletest


echo "========================================\nTesting SBM.cpp"

g++ -std=c++11 \
  -isystem /home/nick/cpp_dev/googletest/googletest/include/ \
  -pthread cpp_tests/test_sbm.cpp\
  /home/nick/cpp_dev/gtest_build/lib/libgtest.a -o my_googletest
./my_googletest
