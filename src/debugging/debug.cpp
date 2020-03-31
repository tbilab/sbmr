// This script serves as a portal into debugging. I use it by pasting in any
// tests that are failing and then stepping through to see what's going on.
#include "../agglomerative_merge.h"
#include "../cpp_tests/build_testing_networks.h"
#include "../network.h"

#include <iostream>

int main(int argc, char** argv)
{
  // Setup simple SBM model
  auto my_sbm = simple_unipartite();

  const int num_initial_blocks = my_sbm.num_nodes_at_level(1);

  // Run aglomerative merge with best single merge done
  const auto single_merge = agglomerative_merge(my_sbm,
                                                1,    // block_level,
                                                1,    // num_merges_to_make,
                                                5,    // num_checks_per_block,
                                                0.1); // eps

  return 0;
}
