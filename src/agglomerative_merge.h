#pragma once

#include <queue>

#include "Ordered_Pair.h"
#include "model_helpers.h"

struct Block_Mergers {
  double entropy_delta = 0.0;
  std::vector<string> merge_from;
  std::vector<string> merge_into;
  Block_Mergers(const int n)
  {
    merge_from.reserve(n);
    merge_into.reserve(n);
  }
};

using Node_Set        = std::unordered_set<Node*>;
using Node_Pair       = Ordered_Pair<Node*>;
using Best_Move_Queue = std::priority_queue<std::pair<double, Node_Pair>>;

inline double merge_entropy_delta(const Node_Pair& merge_pair)
{
  // Block b will be absorbed into block a
  const Node* block_a      = merge_pair.first();
  const Node* block_b      = merge_pair.second();
  const int block_a_degree = block_a->degree();
  const int block_b_degree = block_b->degree();
  const int merged_degree  = block_a_degree + block_b_degree;

  Edge_Count_Map block_a_neighbor_counts = block_a->gather_neighbors_at_level(block_a->level());
  Edge_Count_Map block_b_neighbor_counts = block_b->gather_neighbors_at_level(block_a->level());

  // Entropy accumulation will happen into this variable.
  double merge_entropy_delta = 0.0;

  // First we will add in entropy from before merger
  for (const auto& a_to_t : block_a_neighbor_counts) {
    const Node* block_t = a_to_t.first;
    const double scalar = block_t == block_a ? 2 : 1;
    merge_entropy_delta += ent(a_to_t.second, block_a_degree, block_t->degree()) / scalar;
  }
  for (const auto& b_to_t : block_b_neighbor_counts) {
    const Node* block_t = b_to_t.first;
    // Merge edge counts into absorbing blocks counts
    block_a_neighbor_counts[block_t] += b_to_t.second;

    if (block_t == block_a) continue; // Avoid double counting the a-b pair
    const double scalar = block_t == block_b ? 2 : 1;
    merge_entropy_delta += ent(b_to_t.second, block_b_degree, block_t->degree()) / scalar;
  }

  // Now merge the counts to absorbed block into the absorbing block and get rid of absorbed entry
  // if the pair exists. This only happens if we're in a unipartite network
  const auto a_to_b_it = block_a_neighbor_counts.find(block_b);
  if (a_to_b_it != block_a_neighbor_counts.end()) {
    block_a_neighbor_counts[block_a] += a_to_b_it->second;
    block_a_neighbor_counts.erase(a_to_b_it);
  }

  for (const auto& a_to_t : block_a_neighbor_counts) {
    const Node* block_t     = a_to_t.first;
    const bool is_absorbing = block_t == block_a;
    const int t_degree      = is_absorbing ? merged_degree : block_t->degree();
    const double scalar     = is_absorbing ? 2 : 1;
    merge_entropy_delta -= ent(a_to_t.second, merged_degree, t_degree) / scalar;
  }

  return merge_entropy_delta;
}

// =============================================================================
// Runs efficient MCMC sweep algorithm on desired node level
// =============================================================================
template <typename Network>
inline Block_Mergers agglomerative_merge(Network& net,
                                         const int block_level,
                                         const int num_merges_to_make,
                                         const int num_checks_per_block,
                                         const double& eps,
                                         const bool allow_exhaustive = true)
{
  // Set to keep track of the attepted merge pairs
  auto checked_pairs = Ordered_Pair_Set<Node*>();

  // Priority queue to keep track of best moves
  Best_Move_Queue best_merges;

  for (int type = 0; type < net.num_types(); type++) {
    const auto& blocks_of_type = net.get_nodes_of_type(type, block_level);
    const int n_blocks_of_type = blocks_of_type.size();

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
          const auto merge_pair = Node_Pair(block_i, blocks_of_type.at(j).get());

          best_merges.push(std::make_pair(-merge_entropy_delta(merge_pair), merge_pair));
        }
      }
    } else {
      // Loop through each block and propose m moves using move proposal function
      for (const auto& block : blocks_of_type) {
        const auto block_i = block.get();

        for (int i = 0; i < num_checks_per_block; i++) {
          Node* block_j = net.propose_merge(block_i, eps);

          // Ignore if proposal if it's just the block itself
          if (block_i == block_j) continue;

          const auto merge_pair = Node_Pair(block_i, block_j);

          // See if this combo of groups has already been looked at
          const bool pair_already_checked = !checked_pairs.insert(merge_pair).second;
          if (pair_already_checked) continue;

          // Calculate entropy delta for merge and place into results queue.
          best_merges.push(std::make_pair(-merge_entropy_delta(merge_pair), merge_pair));
        } // End of m merge checks
      }   // End of loop over nodes of a type
    }
  } // End of loop over types in level

  // Now we find the top merges...
  // Start by initializing a merge result struct
  auto results = Block_Mergers(num_merges_to_make);

  std::vector<Node_Pair> merges_to_make;
  merges_to_make.reserve(num_merges_to_make);

  // A set to keep track of what mergers have happened so as to not double up for a block
  Node_Set merged_blocks;

  while (merges_to_make.size() < num_merges_to_make) {
    if (best_merges.size() == 0) LOGIC_ERROR("Ran out of merges to use.");

    // Extract best remaining merge and remove from queue
    const auto best_merge = best_merges.top();
    best_merges.pop();

    const auto block_pair = best_merge.second;

    // Make sure we haven't already merged either of the two blocks
    const bool first_block_free  = merged_blocks.find(block_pair.first()) == merged_blocks.end();
    const bool second_block_free = merged_blocks.find(block_pair.second()) == merged_blocks.end();

    if (first_block_free && second_block_free) {
      merges_to_make.push_back(block_pair);

      // Insert these blocks into merged blocks so they wont be used again
      merged_blocks.insert(block_pair.first());
      merged_blocks.insert(block_pair.second());

      // Insert blocks into results for mergers
      results.merge_from.push_back(block_pair.first()->id());
      results.merge_into.push_back(block_pair.second()->id());
    }

    // Update the results with entropy delta caused by this merge. We subtract
    // here because we negated the entropy delta when inserting into the queue
    results.entropy_delta -= best_merge.first;
  }

  // Finally, go through and make all requested merges
  for (const auto& merge_pair : merges_to_make) {
    net.merge_blocks(merge_pair.first(), merge_pair.second());
  }

  return results;
}