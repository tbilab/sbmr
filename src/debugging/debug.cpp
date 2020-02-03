// This script serves as a portal into debugging. I use it by pasting in any
// tests that are failing and then stepping through to see what's going on.

#include "../SBM.h"
#include "../cpp_tests/network_builders.cpp"

#include <iostream>

int main(int argc, char** argv)
{
  // Setup simple SBM model
  SBM my_SBM = build_simple_SBM();

  int    num_initial_blocks = my_SBM.get_level(1)->size();
  double initial_entropy    = my_SBM.get_entropy(0);

  // Run greedy aglomerative merge with best single merge done
  Merge_Step single_merge = my_SBM.agglomerative_merge(1, 1);

  // Make sure that we now have one less block than before for each type
  int new_block_num    = my_SBM.get_level(1)->size();
  int change_in_blocks = num_initial_blocks - new_block_num;
  // REQUIRE(change_in_blocks == 1);

  // Make sure entropy has gone up as we would expect
  // REQUIRE(single_merge.entropy_delta > 0);

  // Run again but this time merging the best 2
  SBM new_SBM = build_simple_SBM();

  // Run greedy aglomerative merge with best single merge done
  Merge_Step double_merge = new_SBM.agglomerative_merge(1, 2);

  return 0;
}
