// This script serves as a portal into debugging. I use it by pasting in any
// tests that are failing and then stepping through to see what's going on.
#include "../collapse_blocks.h"
#include "../cpp_tests/build_testing_networks.h"
#include "../network.h"

#include <iostream>

int main(int argc, char** argv)
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

  return 0;
}
