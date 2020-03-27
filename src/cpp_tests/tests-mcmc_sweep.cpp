#include "build_testing_networks.h"
#include "../mcmc_sweep.h"
#include "catch.hpp"


TEST_CASE("Basic mcmc sweep - Simple Bipartite", "[SBM]")
{
  auto my_sbm = simple_bipartite();

  const int n_sweeps = 2;

  auto sweep_res = mcmc_sweep(my_sbm,
                              n_sweeps,
                              0.2,   // eps
                              true, //variable num blocks
                              false, //track pairs
                              0,     // level
                              false); //verbose

  REQUIRE(sweep_res.sweep_entropy_delta.size() == n_sweeps);
  REQUIRE(sweep_res.sweep_num_nodes_moved.size() == n_sweeps);
}

TEST_CASE("Basic mcmc sweep - Simple Unipartite", "[SBM]")
{
  const int n_sweeps = 2;

  auto my_sbm = simple_unipartite();

  
  auto sweep_res = mcmc_sweep(my_sbm,
                              n_sweeps,
                              0.2,   // eps
                              true, //variable num blocks
                              false, //track pairs
                              0,     // level
                              false); //verbose

  REQUIRE(sweep_res.sweep_entropy_delta.size() == n_sweeps);
  REQUIRE(sweep_res.sweep_num_nodes_moved.size() == n_sweeps);
}