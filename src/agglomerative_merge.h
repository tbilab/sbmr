#pragma once

#include "Ordered_Pair.h"
#include "mcmc_sweep.h"

#include "../get_move_results.h"

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

using Node_Set        = std::unordered_set<Node*>;
using Node_Pair       = Ordered_Pair<Node*>;
using Best_Move_Queue = std::priority_queue<std::pair<double, Node_Pair>>;

// =============================================================================
// Runs efficient MCMC sweep algorithm on desired node level
// =============================================================================
Merge_Step agglomerative_merge(SBM_Network& net,
                               const int block_level,
                               const int num_merges_to_make,
                               const int num_checks_per_block,
                               const double& eps)
{

  // Strip away any previous meta-block level if it existed
  net.remove_higher_levels(block_level);
  // Build a new level of meta-blocks with one metablock per block
  net.initialize_blocks();

  // Set to keep track of the attepted merge pairs
  auto checked_pairs = Ordered_Pair_Set<Node*>();

  // Priority queue to keep track of best moves
  Best_Move_Queue best_merges;

  for (int type = 0; type < net.num_types(); type++) {

    const auto& blocks_of_type     = net.get_nodes_of_type(type, block_level);
    const int n_neighbors_for_type = net.num_possible_neighbors(type, block_level);

    // I think that there should be an easy way to calculate the minimum number of checks
    // needed to fully capture all pairs and it will be less than n_t
    const int m = std::min(num_checks_per_block, blocks_of_type.size());
    if (m < 2)
      LOGIC_ERROR("To few blocks to perform merge.");

    // Loop through each block and propose m moves using move proposal function
    for (const auto& block : blocks_of_type) {
      const Node* metablock = block->parent();

      for (int i = 0; i < m; i++) {
        const Node* proposed_metablock = net.propose_move(block.get(), eps);
        const auto merge_pair          = Node_Pair(block, proposed_metablock->get_only_child());

        // Ignore if proposed metablock is just current metablock
        if (metablock == proposed_metablock)
          continue;

        // See if this combo of groups has already been looked at
        const bool pair_already_checked = !checked_pairs.insert(merge_pair).second;
        if (pair_already_checked)
          continue;

        const auto move_delta = get_move_results(block, proposed_metablock, n_neighbors_for_type, eps).entropy_delta;

        // Calculate entropy delta for merge and place into results queue.
        best_merges.push(std::make_pair(-move_delta, merge_pair));
      } // End of m merge checks
    }   // End of loop over nodes of a type
  }     // End of loop over types in level

  // Now we find the top merges...
  // Start by initializing a merge result struct
  auto results = Merge_Step();

  // A set to keep track of what mergers have happened so as to not double up for a block
  Node_Set merged_blocks;
  for (int i = 0; i < num_merges_to_make; i++) {
    if (best_merges.size() == 0)
      LOGIC_ERROR("Ran out of merges to run after " + as_str(i));

    // Extract best remaining merge
    const auto best_merge = best_moves_q.top();
    const auto block_pair = best_merge.second;

    // Make sure we haven't already merged the culled block
    // Also make sure that we haven't removed the block we're trying to merge into
    const bool first_block_unmerged  = merged_blocks.insert(block_pair.first()).second;
    const bool second_block_unmerged = merged_blocks.insert(block_pair.second()).second;

    if (first_block_unmerged && second_block_unmerged)
      net.merge_blocks(block_pair.second, block_pair.first);

    // Update the results with entropy delta caused by this merge
    results.entropy_delta += best_merge.first;
  }

  return results;
}