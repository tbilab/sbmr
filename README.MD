# SBMR

## Running Tests

To install the c++ testing framework Google Test:

First, make sure `cmake` is installed and then running the following lines: 

```bash
git clone git@github.com:google/googletest.git
cd googletest/
mkdir install/
cd install/
cmake -DCMAKE_CXX_COMPILER="/usr/bin/g++" -DCMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++" ../
make #compiles the code  
sudo make install #installs the code 
```

Then modify the `cpp_tests/run_tests.sh` script to point to the newly installed library...

```bash
# Set location of the google test library
GTEST_LIB=/Users/nick/cpp_code/googletest/googletest/include/
LIBGTEST=/Users/nick/cpp_code/googletest/install/lib/libgtest.a
```
