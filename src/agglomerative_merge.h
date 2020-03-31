#pragma once

#include <queue>

#include "Ordered_Pair.h"
#include "get_move_results.h"
#include "mcmc_sweep.h"
#include "network.h"

struct Merge_Step {
  double entropy_delta;
  double entropy;
  // State_Dump state;
  int num_blocks;
  Merge_Step()
      : entropy_delta(0)
  {
  }
  Merge_Step(const double e, const State_Dump s, const int n)
      : entropy_delta(e)
      // , state(s)
      , num_blocks(n)
  {
  }
};

using Node_Set        = std::unordered_set<Node*>;
using Node_Pair       = Ordered_Pair<Node*>;
using Best_Move_Queue = std::priority_queue<std::pair<double, Node_Pair>>;

inline double merge_entropy_delta(const Node* absorbing_block, const Node* absorbed_block){

  const int block_level            = absorbing_block->level();
  const int absorbing_block_degree = absorbing_block->degree();
  const int absorbed_block_degree  = absorbed_block->degree();
  const int merged_degree          = absorbing_block_degree + absorbed_block_degree;

  Edge_Count_Map absorbing_block_neighbor_counts = absorbing_block->gather_neighbors_at_level(block_level);
  Edge_Count_Map absorbed_block_neighbor_counts  = absorbed_block->gather_neighbors_at_level(block_level);

  // First get partial entropy from both blocks pre-merger
  double pre_merge_entropy = 0.0;
  for (const auto& a_to_t : absorbing_block_neighbor_counts) {
    const Node* block_t = a_to_t.first;
    const double scalar = block_t == absorbing_block ? 2 : 1;
    pre_merge_entropy += ent(a_to_t.second, absorbing_block_degree, block_t->degree()) / scalar;
  }
  for (const auto& b_to_t : absorbed_block_neighbor_counts) {
    const Node* block_t = b_to_t.first;
    // Merge edge counts into absorbing blocks counts
    absorbing_block_neighbor_counts[block_t] += b_to_t.second;

    if (block_t == absorbing_block) continue; // Avoid double counting the a-b pair
    const double scalar = block_t == absorbed_block ? 2 : 1;
    pre_merge_entropy += ent(b_to_t.second, absorbed_block_degree, block_t->degree()) / scalar;
  }

  // Now merge the counts to absorbed block into the absorbing block and get rid of absorbed entry
  // if the pair exists 
  const auto absorbing_to_absorbed_it = absorbing_block_neighbor_counts.find(absorbed_block);
  if(absorbing_to_absorbed_it != absorbing_block_neighbor_counts.end()){
    absorbing_block_neighbor_counts[absorbing_block] += absorbing_block_neighbor_counts[absorbed_block];
    absorbing_block_neighbor_counts.erase(absorbed_block);
  }

  double post_merge_entropy = 0.0;
  for (const auto& a_to_t : absorbing_block_neighbor_counts) {
    const Node* block_t     = a_to_t.first;
    const bool is_absorbing = block_t == absorbing_block;
    const int t_degree      = is_absorbing ? merged_degree : block_t->degree();
    const double scalar     = is_absorbing ? 2 : 1;
    post_merge_entropy += ent(a_to_t.second, merged_degree, t_degree) / scalar;
  }

  return pre_merge_entropy - post_merge_entropy;
}


// =============================================================================
// Runs efficient MCMC sweep algorithm on desired node level
// =============================================================================
inline Merge_Step agglomerative_merge(SBM_Network& net,
                                      const int block_level,
                                      const int num_merges_to_make,
                                      const int num_checks_per_block,
                                      const double& eps,
                                      const bool allow_exhaustive = true)
{
  // Strip away any previous meta-block level if it existed and
  // build a new level of meta-blocks with one metablock per block
  net.remove_higher_levels(block_level);
  net.initialize_blocks();

  // Set to keep track of the attepted merge pairs
  auto checked_pairs = Ordered_Pair_Set<Node*>();

  // Priority queue to keep track of best moves
  Best_Move_Queue best_merges;

  for (int type = 0; type < net.num_types(); type++) {

    const auto& blocks_of_type     = net.get_nodes_of_type(type, block_level);
    const int n_blocks_of_type     = blocks_of_type.size();
    const int n_neighbors_for_type = net.num_possible_neighbors(type, block_level);
    // Compare how many checks will be done for merges. If this number is greater than just an exhaustive
    // search of all possible pairs, and the user allows it, just perform an exhaustive search of mergers.
    // The exhaustive search also benefits from not having to do the move proposal step.
    // We're comparing n*m vs n*(n-1)/2 moves
    const bool exhaustive_is_cheaper = num_checks_per_block >= (n_blocks_of_type - 1) / 2;

    if (allow_exhaustive && exhaustive_is_cheaper) {

      // Loop through all our nodes of this type in a pairwise fashion to get every possible pair
      for (int i = 0; i < n_blocks_of_type; i++) {
        const auto block_i = blocks_of_type.at(i).get();
        for (int j = i + 1; j < n_blocks_of_type; j++) {
          const auto block_j = blocks_of_type.at(j).get();

          const auto merge_delta = merge_entropy_delta(block_i, block_j);

          best_merges.push(std::make_pair(-merge_delta, Node_Pair(block_i, block_j)));
        }
      }
    } else {
      // Loop through each block and propose m moves using move proposal function
      for (const auto& block : blocks_of_type) {
        const Node* metablock = block->parent();

        for (int i = 0; i < num_checks_per_block; i++) {
          const Node* proposed_metablock = net.propose_move(block.get(), eps);
          const auto merge_pair          = Node_Pair(block.get(), proposed_metablock->get_only_child());

          // Ignore if proposed metablock is just current metablock
          if (metablock == proposed_metablock) continue;

          // See if this combo of groups has already been looked at
          const bool pair_already_checked = !checked_pairs.insert(merge_pair).second;
          if (pair_already_checked) continue;
          
          const auto merge_delta = merge_entropy_delta(block.get(), proposed_metablock->get_only_child());

          // Calculate entropy delta for merge and place into results queue.
          best_merges.push(std::make_pair(-merge_delta, merge_pair));
        } // End of m merge checks
      }   // End of loop over nodes of a type
    }

  } // End of loop over types in level

  // Now we find the top merges...
  // Start by initializing a merge result struct
  auto results = Merge_Step();

  std::vector<Node_Pair> top_distinct_merges;
  top_distinct_merges.reserve(num_merges_to_make);

  // A set to keep track of what mergers have happened so as to not double up for a block
  Node_Set merged_blocks;

  while (top_distinct_merges.size() < num_merges_to_make) {
    if (best_merges.size() == 0) LOGIC_ERROR("Ran out of merges to use.");

    // Extract best remaining merge and remove from queue
    const auto best_merge = best_merges.top();
    best_merges.pop();

    const auto block_pair = best_merge.second;

    // Make sure we haven't already merged the culled block
    // Also make sure that we haven't removed the block we're trying to merge into
    const bool first_block_unmerged  = merged_blocks.insert(block_pair.first()).second;
    const bool second_block_unmerged = merged_blocks.insert(block_pair.second()).second;

    if (first_block_unmerged && second_block_unmerged) {
      top_distinct_merges.push_back(block_pair);
    }

    // Update the results with entropy delta caused by this merge. We subtract
    // here because we negated the entropy delta when inserting into the queue
    results.entropy_delta -= best_merge.first;
  }

  // Dump the highest level of blocks before making merges
  net.remove_last_level();

  for (const auto& merge_pair : top_distinct_merges) {
    net.merge_blocks(merge_pair.first(), merge_pair.second());
  }

  return results;
}