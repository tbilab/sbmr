#pragma once

#include "network.h"
#include "Block_Consensus.h"

struct MCMC_Sweeps {
    std::vector<double> sweep_entropy_delta;
    std::vector<int> sweep_num_nodes_moved;
    Block_Consensus block_consensus;
    std::vector<string> nodes_moved;
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
MCMC_Sweeps mcmc_sweep(SBM_Network& net,
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

  // // Initialize pair tracking map if needed
  // if (track_pairs) {
  //   results.block_consensus.initialize(get_level(level));
  // }

  // // Check if we have any blocks ready in the network...
  // const bool no_blocks_present = get_level(block_level)->size() == 0;

  // if (no_blocks_present) {
  //   initialize_blocks(level);

  //   if (verbose) {
  //     WARN_ABOUT("No blocks present. Initializing one block per node.");
  //   }
  // }

  // if (verbose) {
  //   OUT_MSG << "sweep_num,"
  //           << "node,"
  //           << "current_block,"
  //           << "proposed_block,"
  //           << "entropy_delta,"
  //           << "prob_of_accept,"
  //           << "move_accepted" << std::endl;
  // }

  // // Initialize a vector of nodes that will be passed through for a sweep.
  // // Grab level map
  // const LevelPtr node_map = get_level(level);
  // NodeVec        nodes_to_sweep;
  // nodes_to_sweep.reserve(node_map->size());
  // for (const auto& node : *node_map) {
  //   nodes_to_sweep.push_back(node.second);
  // }

  // for (int i = 0; i < num_sweeps; i++) {
  //   // Book keeper variables for this sweeps stats
  //   int    num_nodes_moved = 0;
  //   double entropy_delta   = 0;

  //   // Shuffle order order of nodes to be run through for sweep
  //   std::shuffle(nodes_to_sweep.begin(), nodes_to_sweep.end(), sampler.generator);

  //   // Setup container to track what pairs need to be updated for sweep
  //   std::set<std::string> pair_moves;

  //   int steps_taken = 0;
  //   // Loop through each node
  //   for (const NodePtr& curr_node : nodes_to_sweep) {
  //     // Check if we're running sweep with variable block numbers. If we are, we
  //     // need to make sure we don't have any extra unoccupied blocks floating around,
  //     // then we need to add a new block as a potential for the node to enter
  //     if (variable_num_blocks) {
  //       clean_empty_blocks();
  //       create_block_node(curr_node->type, block_level);
  //     }

  //     // Get a move proposal
  //     const NodePtr proposed_new_block = propose_move(*curr_node, eps);

  //     // If the proposed block is the nodes current block, we don't need to waste
  //     // time checking because decision will always result in same state.
  //     if (curr_node->parent == proposed_new_block) {
  //       continue;
  //     }

  //     if (verbose) {
  //       OUT_MSG << i
  //               << "," << curr_node->id
  //               << "," << (curr_node->parent)->id
  //               << "," << proposed_new_block->id
  //               << ",";
  //     }
  //     // Calculate acceptance probability based on posterior changes
  //     Proposal_Res proposal_results = make_proposal_decision(*curr_node, *proposed_new_block, eps);

  //     // Make movement decision
  //     const bool move_accepted = proposal_results.prob_of_accept > sampler.draw_unif();

  //     if (verbose) {
  //       OUT_MSG << proposal_results.entropy_delta << "," << proposal_results.prob_of_accept << ","
  //               << move_accepted << std::endl;
  //     }

  //     // Is the move accepted?
  //     if (move_accepted) {
  //       const NodePtr old_block = curr_node->parent;

  //       // Move the node
  //       curr_node->set_parent(proposed_new_block);

  //       // Update results
  //       results.nodes_moved.push_back(curr_node->id);
  //       num_nodes_moved++;
  //       entropy_delta += proposal_results.entropy_delta;

  //       if (track_pairs) {
  //         Block_Consensus::update_changed_pairs(curr_node->id,
  //                                               old_block->children,
  //                                               proposed_new_block->children,
  //                                               pair_moves);
  //       }
  //     } // End accepted if statement

  //     // Check for user breakout every 100 iterations.
  //     steps_taken = (steps_taken + 1) % 100;
  //     if (steps_taken == 0) {
  //       ALLOW_USER_BREAKOUT;
  //     }
  //   } // End current sweep

  //   // Update results for this sweep
  //   results.sweep_num_nodes_moved.push_back(num_nodes_moved);
  //   results.sweep_entropy_delta.push_back(entropy_delta);

  //   // Update the concensus pairs map with results if needed.
  //   if (track_pairs) {
  //     results.block_consensus.update_pair_tracking_map(pair_moves);
  //   }
  //   ALLOW_USER_BREAKOUT; // Let R used break out of loop if need be
  // }                      // End multi-sweep loop

  return results;
}