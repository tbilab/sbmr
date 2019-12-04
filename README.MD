# SBMR

## Running Tests

The tests are implemented using a precompiled header test library so testing 
is as simple as cloning this repo and then running the shell script `run_tests.sh`.

E.g. 

```bash
sh cpp_tests/run_tests.sh
```

The results should be output with timings etc. 

## Profiling code

Code profiling is done in much the same way, just using the script `profiling/run_profiling.sh`. 

The script outputs `profiling/results.json` which can then be loaded into 
`chrome://tracing` to view the flamegraph of the code execution for a agglomerative
merging. 