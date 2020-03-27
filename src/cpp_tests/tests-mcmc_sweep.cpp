#include "build_testing_networks.h"
#include "../mcmc_sweep.h"
#include "catch.hpp"


TEST_CASE("Basic mcmc sweep - Simple Bipartite", "[SBM]")
{
  auto my_sbm = simple_bipartite();

  auto sweep_res = mcmc_sweep(my_sbm,
                              2,
                              0.2,   // eps
                              true, //variable num blocks
                              false, //track pairs
                              0,     // level
                              false); //verbose

}

TEST_CASE("Basic mcmc sweep - Simple Unipartite", "[SBM]")
{
  auto my_sbm = simple_unipartite();

  
  auto sweep_res = mcmc_sweep(my_sbm,
                              2,
                              0.2,   // eps
                              true, //variable num blocks
                              false, //track pairs
                              0,     // level
                              false); //verbose
}