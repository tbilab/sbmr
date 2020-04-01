#pragma once

#include "Block_Consensus.h"
#include "get_move_results.h"
#include "network.h"

struct MCMC_Sweeps {
  std::vector<double> sweep_entropy_delta;
  std::vector<int> sweep_num_nodes_moved;
  Block_Consensus block_consensus;
  std::vector<string> nodes_moved;
  double entropy_delta = 0.0;
  MCMC_Sweeps(const int n)
  {
    // Preallocate the entropy change and num groups moved in sweep vectors and
    sweep_entropy_delta.reserve(n);
    sweep_num_nodes_moved.reserve(n);
  }
};

// =============================================================================
// Runs efficient MCMC sweep algorithm on desired node level
// =============================================================================
inline MCMC_Sweeps mcmc_sweep(SBM_Network& net,
                              const int num_sweeps,
                              const double& eps,
                              const bool variable_num_blocks,
                              const bool track_pairs,
                              const int level    = 0,
                              const bool verbose = false)
{
  const int block_level = level + 1;

  // Initialize structure that contains the returned values for this/these sweeps
  MCMC_Sweeps results(num_sweeps);

  // Initialize pair tracking map if needed
  if (track_pairs) {
    results.block_consensus.initialize(net.get_nodes_at_level(level));
  }

  // Check if we have any blocks ready in the network...
  const bool no_blocks_present = net.num_levels() > block_level + 1;

  if (no_blocks_present) {
    net.initialize_blocks();

    if (verbose) {
      WARN_ABOUT("No blocks present. Initializing one block per node.");
    }
  }

  // If we are allowing a variable number of blocks, initialize an empty block for each node type
  if (variable_num_blocks) {
    for (int type = 0; type < net.num_types(); type++) {
      net.add_block_node(type, block_level);
    }
  }

  if (verbose) {
    OUT_MSG << "sweep_num,"
            << "node,"
            << "current_block,"
            << "proposed_block,"
            << "entropy_delta,"
            << "prob_of_accept,"
            << "move_accepted" << std::endl;
  }

  // Initialize a vector of nodes that will be passed through for a sweep.
  auto nodes = net.get_flat_level(level);

  for (int i = 0; i < num_sweeps; i++) {
    // Book keeper variables for this sweeps stats
    int num_nodes_moved  = 0;
    double entropy_delta = 0;

    // Shuffle order of nodes to be run through for sweep
    net.sampler.shuffle(nodes);

    // Setup container to track what pairs of nodes need to have their consensus membership updated for this sweep
    Pair_Set pair_moves;

    int steps_taken = 0;
    // Loop through each node
    for (const auto& curr_node : nodes) {
      // Get a move proposal
      Node* proposed_new_block = net.propose_move(curr_node, eps);

      Node* old_block = curr_node->parent();

      // If the proposed block is the nodes current block, we don't need to waste
      // time checking because decision will always result in same state.
      if (old_block == proposed_new_block) {
        continue;
      }

      if (verbose) {
        OUT_MSG << i
                << "," << curr_node->id()
                << "," << curr_node->parent()->id()
                << "," << proposed_new_block->id()
                << ",";
      }
      // Calculate acceptance probability based on posterior changes
      auto proposal_results = get_move_results(curr_node,
                                               proposed_new_block,
                                               net.num_possible_neighbor_blocks(curr_node),
                                               eps);

      // Make movement decision
      const bool move_accepted = proposal_results.prob_of_accept > net.sampler.draw_unif();

      if (verbose) {
        OUT_MSG << proposal_results.entropy_delta << "," << proposal_results.prob_of_accept << ","
                << move_accepted << std::endl;
      }

      // Is the move accepted?
      if (move_accepted) {

        bool remove_empty_block = variable_num_blocks;
        if (variable_num_blocks) {
          // If the old block will still have children after the move and
          // the new block is empty block, this move will cause there to be no
          // empty blocks for this type
          const bool old_wont_be_empty = old_block->num_children() > 1;
          const bool new_is_empty      = proposed_new_block->num_children() == 0;

          if (new_is_empty) {
            if (old_wont_be_empty) {
              // Need to add a new block node as we wont have any empty ones left
              net.add_block_node(curr_node->type(), block_level);
            } else {
              // In this case the old block will become to the empty block so we
              // don't want it to be removed by the swap_blocks function
              remove_empty_block = false;
            }
          }
        }

        net.swap_blocks(curr_node, proposed_new_block, remove_empty_block);

        // Update results
        results.nodes_moved.push_back(curr_node->id());
        num_nodes_moved++;
        entropy_delta += proposal_results.entropy_delta;

        if (track_pairs) {
          Block_Consensus::update_changed_pairs(curr_node->id(),
                                                old_block->children(),
                                                proposed_new_block->children(),
                                                pair_moves);
        }

      } // End accepted if statement

      // Check for user breakout every 100 iterations.
      steps_taken = (steps_taken + 1) % 100;
      if (steps_taken == 0) {
        ALLOW_USER_BREAKOUT;
      }
    } // End current sweep

    // Update results for this sweep
    results.sweep_num_nodes_moved.push_back(num_nodes_moved);
    results.sweep_entropy_delta.push_back(entropy_delta);
    results.entropy_delta += entropy_delta;

    // Update the concensus pairs map with results if needed.
    if (track_pairs) {
      results.block_consensus.update_pair_tracking_map(pair_moves);
    }
    ALLOW_USER_BREAKOUT; // Let R used break out of loop if need be
  }                      // End multi-sweep loop

  if (variable_num_blocks) {
    // Cleanup the single empty block for each type
    for (const auto& blocks_of_type : net.get_nodes_at_level(block_level)) {
      for (const auto& block : blocks_of_type) {
        if (block->num_children() == 0) {
          net.delete_node(block);
          break; // No need to continue so break out (also would cause problems because we just modified the vector we're looping over)
        }
      }
    }
  }
  return results;
}