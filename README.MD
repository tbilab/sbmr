# SBMR

An R package for fitting and exploring the results of Stochastic Block Models for network data. 

## Development

This package is currently under active development. Commits to the master branch pass all included R and C++ tests but are not guarenteed to be bug free. If you discover a bug in use please report using the [issue tracker for the github repo.](https://github.com/tbilab/sbmR/issues)


## Running Tests 

Tests for the package fall under two categories: tests for the underlying c++ code and the R package code. 

### C++ tests

Tests for c++ code are implemented using a precompiled header test library so testing 
is as simple as cloning this repo and then running the shell script `src/cpp_tests/run_tests.sh`.

E.g. 

```bash
sh src/cpp_tests/run_tests.sh
```

The results should be output with timings etc. 

#### Profiling

As the C++ code is should be as fast as possible, a profiling workflow for detecting slow areas of code is also included. 
Like with testing the script `src/profiling/run_profiling.sh` will compile a `profile.cpp` script, run it, and return the output to json file for investigation in your favorite flame-graph viewer such as `chrome://tracing`. 


### R tests

Tests for the R package code that wraps the underlying c++ heavy lifting are done using the standard testthat workflow. To run them either use the built in build pane in RStudio or run `devtools::test()`. 



