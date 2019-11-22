// This script serves as a portal into debugging. I use it by pasting in any
// tests that are failing and then stepping through to see what's going on.

#include <iostream>
#include "../helpers.h"
#include "../SBM.h"
#include "../cpp_tests/network_builders.cpp"

// Print a state dump for debugging purposes
void print_state_dump(State_Dump state)
{
  int n = state.id.size();

  for (int i = 0; i < n; i++)
  {
    std::cout << state.id[i] << ", "
              << state.parent[i] << ", " 
              << state.level[i] << ", " 
              << std::endl;
  }
}

int main(int argc, char **argv)
{
  // Setup simple SBM model
  SBM my_SBM = build_simulated_SBM();

  int desired_num_groups = 4;

  // Run full agglomerative merging algorithm till we have just 3 groups left
  auto run_results = my_SBM.agglomerative_run(
      0,     // Level
      desired_num_groups);
      
  return 0;
}
