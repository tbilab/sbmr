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
                              true); //verbose

 
  // consensus.initialize(my_sbm.get_nodes_at_level(0));

  // // There should be nt(nt-1)/2 pairs for each type t with nt nodes. 
  // REQUIRE(consensus.size() == 12);
}

// TEST_CASE("Basic mcmc sweep - Simple Unipartite", "[SBM]")
// {
//   auto my_sbm = simple_unipartite();

//   auto consensus = Block_Consensus();

//   consensus.initialize(my_sbm.get_nodes_at_level(0));

//   // There should be nt(nt-1)/2 pairs for each type t with nt nodes. 
//   // 6*5/2
//   REQUIRE(consensus.size() == 15);
// }