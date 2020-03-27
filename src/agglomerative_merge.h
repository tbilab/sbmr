#pragma once

#include "mcmc_sweep.h"
#include "network.h"

struct Merge_Step {
  double entropy_delta;
  double entropy;
  State_Dump state;
  int num_blocks;
  Merge_Step()
      : entropy_delta(0)
  {
  }
  Merge_Step(const double e, const State_Dump s, const int n)
      : entropy_delta(e)
      , state(s)
      , num_blocks(n)
  {
  }
};

// =============================================================================
// Runs efficient MCMC sweep algorithm on desired node level
// =============================================================================
Merge_Step agglomerative_merge(SBM_Network& net,
                               const int block_level,
                               onst int num_merges_to_make,
                               const int num_checks_per_block,
                               const double& eps)
{
  auto results = Merge_Step();

  return results;
}