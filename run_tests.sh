g++ -std=c++11 \
  -isystem /home/nick/cpp_dev/googletest/googletest/include/ \
  -pthread test_node.cpp \
  /home/nick/cpp_dev/gtest_build/lib/libgtest.a -o my_googletest
./my_googletest