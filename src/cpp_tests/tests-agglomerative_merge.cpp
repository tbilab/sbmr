#include "../network.h"
#include "../agglomerative_merge.h"
#include "build_testing_networks.h"
#include "catch.hpp"


TEST_CASE("Agglomerative merge steps - Simple Bipartite", "[SBM]")
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


TEST_CASE("Agglomerative merge steps - Simple Unipartite", "[SBM]")
{

  auto my_sbm = simple_unipartite();

  const int num_initial_blocks = my_sbm.num_nodes_at_level(1);

  REQUIRE(num_initial_blocks == 3);

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

  // // Run again but this time merging the best 2
  // auto new_sbm = simple_bipartite();

  // const auto double_merge = agglomerative_merge(new_sbm,
  //                                               1,    // block_level,
  //                                               2,    // num_merges_to_make,
  //                                               5,    // num_checks_per_block,
  //                                               0.1); // eps

  // // Make sure that we now have two fewer blocks per type than before
  // REQUIRE(2 == num_initial_blocks - new_sbm.num_nodes_at_level(1));

  // // Entropy should up even more with two merges
  // REQUIRE(single_merge.entropy_delta < double_merge.entropy_delta);
}
