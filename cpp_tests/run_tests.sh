echo $PWD

echo "=============================================================================\nCompiling files..."
echo "=============================================================================\n"
# # Compile files so we can link
# g++ -std=c++11 -c Node.cpp SBM.cpp helpers.cpp Sampler.cpp

# g++ -std=c++11 cpp_tests/tests-main.cpp -c -o cpp_tests/tests-main.o

g++ -std=c++11 -c Node.cpp SBM.cpp helpers.cpp Sampler.cpp

g++ -std=c++11 cpp_tests/tests-main.o Node.o helpers.o Sampler.o cpp_tests/tests-node.cpp cpp_tests/tests-sampler.cpp -o cpp_tests/run_tests.o 

./cpp_tests/run_tests.o

# Cleanup the compiled classes so RCpp doesn't get mad
rm *.o

# echo "=============================================================================\nTesting Sampler.cpp"
# echo "=============================================================================\n"
# g++ -std=c++11 \
#   -isystem ${ISYSTEM} \
#   -pthread cpp_tests/test_sampler.cpp Sampler.o helpers.o Node.o\
#   ${LIBGTEST} -o my_googletest
# ./my_googletest


# echo "=============================================================================\nTesting Node.cpp"
# echo "=============================================================================\n"
# g++ -std=c++11 \
#   -isystem ${ISYSTEM} \
#   -pthread cpp_tests/test_node.cpp Node.o helpers.o\
#   ${LIBGTEST} -o my_googletest
# ./my_googletest


# echo "=============================================================================\nTesting SBM.cpp"
# echo "=============================================================================\n"
# g++ -std=c++11 \
#   -isystem ${ISYSTEM} \
#   -pthread cpp_tests/test_sbm.cpp Node.o SBM.o helpers.o Sampler.o\
#   ${LIBGTEST} -o my_googletest
# ./my_googletest


# g++ -std=c++11 \
#   -isystem ${ISYSTEM} \
#   -pthread cpp_tests/test_sbm.cpp Node.o SBM.o helpers.o Sampler.o\
#   ${LIBGTEST} -o my_googletest

# Cleanup the compiled files so RCpp doesn't get mad
# rm *.o
# rm my_googletest
