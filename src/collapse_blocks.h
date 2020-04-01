#pragma once

#include "agglomerative_merge.h"
#include "mcmc_sweep.h"
#include "network.h"

struct Collapse_Results {
  double entropy_delta = 0; // Will keep track of the overall entropy change from this collapse
  int num_blocks;
  std::vector<Block_Mergers> merge_steps; // Will keep track of results at each step of the merger
  std::vector<State_Dump> states;
  Collapse_Results(const int n)
      : num_blocks(n)
  {
  }
};

inline int calc_num_merges(const int B, const int B_end, const double& sigma)
{
  const int target_num = std::floor(double(B) / sigma);

  return std::max(std::min(target_num, B - B_end), 1);
}

inline Collapse_Results collapse_blocks(SBM_Network& net,
                                        const int node_level,
                                        const int B_end,
                                        const int n_checks_per_block,
                                        const int n_mcmc_sweeps,
                                        const double& sigma,
                                        const double& eps,
                                        const bool report_all_steps = true,
                                        const bool allow_exhaustive = true)
{
  const int block_level = node_level + 1;
  const bool using_mcmc = n_mcmc_sweeps > 0;

  // Initialize struct to hold results of collapse
  auto results = Collapse_Results(B_end);

  // Remove any existing block level(s)
  net.remove_block_levels_above(node_level);

  // Initialize one-block-per-node
  net.initialize_blocks();

  // Setup variable to track the current number of blocks in the model
  int B_cur = net.num_nodes_at_level(block_level);

  // Lambda to calculate how many merges a step needs
  auto calc_num_merges = [&B_end, &sigma](const int B) {
    const int target_num      = std::floor(double(B) / sigma);
    const int merges_till_end = B - B_end;
    return std::max(std::min(target_num, merges_till_end), 1);
  };

  // Keep doing merges until we've reached the desired number of blocks
  while (B_cur > B_end) {
    const int n_merges_to_make = calc_num_merges(B_cur);

    // Perform merges
    auto merge_result = agglomerative_merge(net,
                                            block_level,
                                            n_merges_to_make,
                                            n_checks_per_block,
                                            eps,
                                            allow_exhaustive);

    if (using_mcmc) {
      // Update the merge results entropy delta with the changes caused by MCMC sweep
      merge_result.entropy_delta += mcmc_sweep(net,
                                               n_mcmc_sweeps,
                                               eps,        // eps
                                               false,      // variable num blocks
                                               false,      // track pairs
                                               node_level, // level
                                               false)      // verbose
                                        .entropy_delta;
    }

    // Update results stuct
    results.entropy_delta += merge_result.entropy_delta;

    if (report_all_steps) {
      results.merge_steps.push_back(merge_result);
      results.states.push_back(net.state());
    }

    // Update B_cur
    B_cur -= n_merges_to_make;
  }
}
