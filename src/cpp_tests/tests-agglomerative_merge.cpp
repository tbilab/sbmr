#include "../network.h"
#include "../collapse_blocks.h"
#include "build_testing_networks.h"
#include "catch.hpp"

TEST_CASE("Agglomerative merge step - Simple Bipartite", "[SBM]")
{
  auto my_sbm = simple_bipartite();

  const int num_initial_blocks = my_sbm.num_nodes_at_level(1);

  REQUIRE(num_initial_blocks == 6);

  // Run aglomerative merge with best single merge done
  const auto single_merge = agglomerative_merge(my_sbm,
                                                1,    // block_level,
                                                1,    // num_merges_to_make,
                                                5,    // num_checks_per_block,
                                                0.1); // eps

  // Make sure that we now have one less block than before for each type
  const int new_block_num    = my_sbm.num_nodes_at_level(1);
  const int change_in_blocks = num_initial_blocks - new_block_num;
  REQUIRE(change_in_blocks == 1);

  // Make sure entropy has gone up as we would expect
  REQUIRE(single_merge.entropy_delta > 0);

  // Run again but this time merging the best 2
  auto new_sbm = simple_bipartite();

  const auto double_merge = agglomerative_merge(new_sbm,
                                                1,    // block_level,
                                                2,    // num_merges_to_make,
                                                5,    // num_checks_per_block,
                                                0.1); // eps

  // Make sure that we now have two fewer blocks per type than before
  REQUIRE(2 == num_initial_blocks - new_sbm.num_nodes_at_level(1));

  // Entropy should up even more with two merges
  REQUIRE(single_merge.entropy_delta < double_merge.entropy_delta);
}

TEST_CASE("Agglomerative merge step - Simple Unipartite", "[SBM]")
{
  const int num_initial_blocks = 4;

  auto my_sbm = simple_unipartite();
  // We need to erase the original block stucture and give every node its own block to have enough blocks for this
  my_sbm.remove_block_levels_above(0);
  my_sbm.initialize_blocks(num_initial_blocks);

  // Run aglomerative merge with best single merge done
  const auto single_merge = agglomerative_merge(my_sbm,
                                                1,    // block_level,
                                                1,    // num_merges_to_make,
                                                5,    // num_checks_per_block,
                                                0.1); // eps

  // Make sure that we now have one less block than before for each type
  const int new_block_num    = my_sbm.num_nodes_at_level(1);
  const int change_in_blocks = num_initial_blocks - new_block_num;
  REQUIRE(change_in_blocks == 1);

  // Make sure entropy has gone up as we would expect
  REQUIRE(single_merge.entropy_delta > 0);

  // Run again but this time merging the best 2
  auto new_sbm = simple_unipartite();
  new_sbm.remove_block_levels_above(0);
  new_sbm.initialize_blocks(num_initial_blocks);

  const auto double_merge = agglomerative_merge(new_sbm,
                                                1,    // block_level,
                                                2,    // num_merges_to_make,
                                                5,    // num_checks_per_block,
                                                0.1); // eps

  // Make sure that we now have two fewer blocks per type than before
  REQUIRE(2 == num_initial_blocks - new_sbm.num_nodes_at_level(1));

  // Entropy should up even more with two merges
  REQUIRE(single_merge.entropy_delta < double_merge.entropy_delta);
}


TEST_CASE("Collapse Blocks (no MCMC) - Simple Bipartite", "[SBM]")
{
  auto my_sbm = simple_bipartite();

  auto collapse_to_2_res = collapse_blocks(my_sbm,
                                           0,     // node_level,
                                           2,     // B_end,
                                           5,     // n_checks_per_block,
                                           0,     // n_mcmc_sweeps,
                                           1.1,   // sigma,
                                           0.01,  // eps,
                                           true,  // report all steps,
                                           true); // Allow exhaustive
}