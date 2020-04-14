# SBMR

<!-- badges: start -->
[![R build status](https://github.com/nstrayer/sbmR/workflows/R-CMD-check/badge.svg)](https://github.com/nstrayer/sbmR/actions)
<!-- badges: end -->

An R package for fitting and exploring the results of Stochastic Block Models for network data. 

## What are SBMs?

SBM Stands for "Stochastic Block Model." These models are used to describe the generating process for graph data where nodes are arranged into 'blocks' (or 'clusters' or 'groups'). 

### "Block"
A given block of nodes can be characterized by the propensity for a node within it to be connected to all the other blocks in the network. For instance, in a network of three blocks - a, b, and c - any node from block a may have an average number of connections to other nodes in block a of 5, an average of 10 connections to nodes in block b, and 1 to nodes in block c.

### "Stochastic"
Stochastic (a fancy word for "with randomness") just means that while that node from block a may have on average 5, 10, and 1 connections to blocks a, b, and c respectively, the actual observed values may be different, perhaps 4, 8, and 3. In this standard case the random model that is used to 'draw' these random counts is the poisson model. Other distributions can be used (such as bernouli for binary connection or no connection edges), but the package currently only supports _inference_ with the poisson model. 

### Fitting a "Model"

Up to now the model has been described as one to generate data, but that's not particularly interesting. When you fit an SBM to some data what you are doing is trying to find the underlying model that is most likely to have generated your observed data. 

This package does that using Markov Chain Monte Carlo (MCMC) where the underlying model that may have generated your data is shuffled around to likely layouts and how well that layout fits your data is recorded. This results in many possible models where the frequency of a model layout occuring is proportional to how well supported that layout is by your data. 


## Are my data appropriate for an SBM?

Like any good statistical model many assumptions about your data and its generating process need to be made for the model results to have any meaning. In the case of the SBM these assumptions are relatively minimal. 

### Data can be represented as a graph with edges counts between nodes

The classic data that fits this is social network data. A node may represent an individual and edges represent the existance of a connection (friendship, interaction, etc) with an other node. Multipartite graph data is also supported. The work that this package was developed for is bipartite data (meaning two-node-types) of an individual and disease diagnoses, an "individual node" is connected to a "disease-node" if the person was diagnosed with that disease. 

Examples of data that are not appropriate are anything where the connections are continuous. Perhaps nodes are cities and the edges are number of miles between them (with a fractional miles). While you could round these continuous values to integers it is highly likely that it is innapropriate for the model and you are better served finding a more parsimonius model. 

### The underlying phenomena generating your data should result in sets of nodes that have similar connection patterns

The SBM attempts to partition the nodes in your data into groups that are, in the eyes of the model, identical in terms of there probability of connection the node partitioning. If this is unreasonable for the data, the model will not return valid results. For instance, in many data with people, socioeconomic information can be highly predictive of a large number of outcomes. In this case the SBM will be more likely do differntiate different socioeconomic classes rather than a more interesting separation. In addition, the socioeconomic confounding in this case is continuous and therefor breaks the assumption of clean groups of nodes. 


## Package Goals

This package has a limited scope it aims to do the following things well:

- Fit SBM models using Bayesian MCMC
- Heavy focus on model uncertainty
- Easily work with multi-partite (bi, tri, etc) graph data
- Produce meaningful and high-quality visualizations of model results 

For more powerful (but larger and harder-to-install) libraries for working with graph data check out `graph-tool` for Python (heavy SBM focus with many more features than `sbmr`) and `iGraph` (a giant in graph data manipulation and exploration.) 

If there is a feature that you feel would be very beneficial to the package that would save you needing to go to another package for a small graph-data/SBM related function, please let me know in [github issues](https://github.com/tbilab/sbmR/issues) or if you have the time/desire a PR (I am willing to help out implementation details for those not fully comfortable with PRs into open source yet). 


## Usage

Please see the [articles on package usage on the package website.]()

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



