#include "build_testing_networks.h"
#include "../Block_Consensus.h"
#include "../swap_blocks.h"
#include "catch.hpp"


TEST_CASE("Building block concensus - Simple Bipartite", "[SBM]")
{

  auto my_sbm = simple_bipartite();

  auto consensus = Block_Consensus();

  consensus.initialize(my_sbm.get_nodes_at_level(0));

  // There should be nt(nt-1)/2 pairs for each type t with nt nodes. 
  REQUIRE(consensus.size() == 12);
}

TEST_CASE("Building block concensus - Simple Unipartite", "[SBM]")
{

  auto my_sbm = simple_unipartite();

  auto consensus = Block_Consensus();

  consensus.initialize(my_sbm.get_nodes_at_level(0));

  // There should be nt(nt-1)/2 pairs for each type t with nt nodes. 
  // 6*5/2
  REQUIRE(consensus.size() == 15);
}