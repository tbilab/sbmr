#include "SBM.h"

#include "sbm_helpers.h"

// =============================================================================
// Propose a potential block move for a node.
// =============================================================================
NodePtr SBM::propose_move(const NodePtr node)
{
  PROFILE_FUNCTION();

  const int block_level = node->level + 1;

  // Grab a list of all the blocks that the node could join
  const NodeVec potential_blocks = get_nodes_of_type_at_level(node->type, block_level);

  // Sample a random neighbor of the current node
  const NodePtr rand_neighbor = sampler.sample(node->get_edges_to_level(node->level));

  // Get number total number edges for neighbor's block
  const int neighbor_block_degree = rand_neighbor->parent->degree;

  // Decide if we are going to choose a random block for our node
  const double ergo_amnt            = EPS * potential_blocks.size();
  const double prob_of_random_block = ergo_amnt / (neighbor_block_degree + ergo_amnt);

  // Decide where we will get new block from and draw from potential candidates
  return sampler.draw_unif() < prob_of_random_block
      ? sampler.sample(potential_blocks)
      : sampler.sample(rand_neighbor->get_edges_to_level(block_level));
}

// =============================================================================
// Make a decision on the proposed new block for node
// =============================================================================
Proposal_Res SBM::make_proposal_decision(const NodePtr node,
                                         const NodePtr new_block,
                                         const bool merge_testing = false)
{
  PROFILE_FUNCTION();
  // The level that this proposal is taking place on
  const int block_level = new_block->level;

  // Reference to old block that would be swapped for new_block
  const NodePtr old_block = node->parent;

  // Make sure we're actually doing something
  if (old_block == new_block) {
    return Proposal_Res(0.0, 0.0, false);
  }

  // What type of nodes does the current node connect to?
  const int connections_type = (node->edges).front()->type;

  // Get vector of all nodes of the desired type at the block level
  NodeVec potential_neighbors = get_nodes_of_type_at_level(connections_type, block_level);

  // Gather edge entropy values corresponding to the pre-move state from both
  // the new and old block
  const NodeEdgeMap old_block_edge_counts_pre = old_block->gather_edges_to_level(block_level);
  const NodeEdgeMap new_block_edge_counts_pre = new_block->gather_edges_to_level(block_level);

  // Calculate the entropy portion that will change from move for before the move
  const double partial_entropy_pre = compute_node_edge_entropy_partial(old_block_edge_counts_pre, old_block, new_block) + compute_node_edge_entropy_partial(new_block_edge_counts_pre, new_block, old_block);

  // Calculate the probability of doing the move to the new node
  double prob_move_to_new;
  if (!merge_testing) {
    // No need to do this if we're looking at merge results
    prob_move_to_new = calc_prob_of_move(new_block_edge_counts_pre, potential_neighbors, EPS);
  }

  // Move node to new block
  node->set_parent(new_block);

  // Gather the new entropy portions from the two blocks
  const NodeEdgeMap old_block_edge_counts_post = old_block->gather_edges_to_level(block_level);
  const NodeEdgeMap new_block_edge_counts_post = new_block->gather_edges_to_level(block_level);

  // Calculate the entropy portion that will change after doing the move
  const double partial_entropy_post = compute_node_edge_entropy_partial(old_block_edge_counts_post, old_block, new_block) + compute_node_edge_entropy_partial(new_block_edge_counts_post, new_block, old_block);

  double prob_move_back_to_old;
  if (!merge_testing) {
    // Calculate the probability of moving back to the old node
    prob_move_back_to_old = calc_prob_of_move(old_block_edge_counts_post, potential_neighbors, EPS);
  }
  // Get difference in entropy values from before and after move
  const double entropy_delta = partial_entropy_pre - partial_entropy_post;

  bool move_accepted = false;
  double acceptance_prob = 0;

  if (!merge_testing) {
    // Multiply both together to get the acceptance probability
    acceptance_prob = exp(entropy_delta) * (prob_move_to_new / prob_move_back_to_old);

    move_accepted = acceptance_prob > 1
        ? true
        : sampler.draw_unif() < acceptance_prob;
  }

  if (!move_accepted | merge_testing) {
    // Return the node to the old block if move wasn't accepted
    node->set_parent(old_block);
  }

  return Proposal_Res(entropy_delta, acceptance_prob, move_accepted);
}

// =============================================================================
// Runs efficient MCMC sweep algorithm on desired node level
// =============================================================================
MCMC_Sweeps SBM::mcmc_sweep(const int  level,
                            const int  num_sweeps,
                            const bool variable_num_blocks,
                            const bool track_pairs)
{
  PROFILE_FUNCTION();

  const int block_level = level + 1;

  // Initialize structure that contains the returned values for this/these sweeps
  MCMC_Sweeps results(num_sweeps);

  // Initialize pair tracking map if needed
  if (track_pairs) {
    results.block_consensus.initialize(get_level(level));
  }

  // Grab level map
  const LevelPtr node_map = get_level(level);

  // Initialize vector to hold nodes in order of pass through for a sweep.
  NodeVec node_vec;
  node_vec.reserve(node_map->size());

  for (int i = 0; i < num_sweeps; i++) {
    // Book keeper variables for this sweeps stats
    int    num_nodes_moved = 0;
    double entropy_delta   = 0;

    // Generate a random order of nodes to be run through for sweep
    Sampler::shuffle_nodes(node_vec, node_map, sampler.int_gen);

    // Setup container to track what pairs need to be updated for sweep
    std::unordered_set<std::string> pair_moves;

    // Loop through each node
    for (const NodePtr& curr_node : node_vec) {

      // Check if we're running sweep with variable block numbers. If we are, we
      // need to add a new block as a potential for the node to enter
      if (variable_num_blocks) {
        // Add a new block node for the current node type
        create_block_node(curr_node->type, block_level);
      }

      // Get a move proposal
      const NodePtr proposed_new_block = propose_move(curr_node);

      // If the propsosed block is the nodes current block, we don't need to waste
      // time checking because decision will always result in same state.
      if (curr_node->parent->id == proposed_new_block->id) {
        continue;
      }

      // Calculate acceptance probability based on posterior changes
      Proposal_Res proposal_results = make_proposal_decision(curr_node, proposed_new_block);

      // Is the move accepted?
      if (proposal_results.move_accepted) {
        const NodePtr old_block = curr_node->parent;

        // Move the node
        curr_node->set_parent(proposed_new_block);

        // Update results
        results.nodes_moved.push_back(curr_node->id);
        num_nodes_moved++;
        entropy_delta += proposal_results.entropy_delta;

        // If we are varying number of blocks and we made a move we should clean
        // up the now potentially empty blocks for the next node proposal.
        if (variable_num_blocks) {
          clean_empty_blocks();
        }

        if (track_pairs) {
          Block_Consensus::update_changed_pairs(curr_node,
                                                old_block->children,
                                                proposed_new_block->children,
                                                pair_moves);
        }
      } // End accepted if statement
    }   // End current sweep

    // Update results for this sweep
    results.sweep_num_nodes_moved.push_back(num_nodes_moved);
    results.sweep_entropy_delta.push_back(entropy_delta);

    // Update the concensus pairs map with results if needed.
    if (track_pairs) {
      results.block_consensus.update_pair_tracking_map(pair_moves);
    }
  } // End multi-sweep loop

  return results;
}

// =============================================================================
// Compute microcononical entropy of current model state
// Note that this is currently only the degree corrected entropy
// =============================================================================
double SBM::compute_entropy(const int level)
{
  PROFILE_FUNCTION();
  //============================================================================
  // First, calc the number of total edges and build a degree->num nodes map

  // Build map of number of nodes with given degree
  std::map<int, int> n_nodes_w_degree;

  // Keep track of total number of edges as well
  double n_total_edges = 0;

  // Grab pointer to current level and start loop
  LevelPtr node_level = get_level(level);
  for (auto const& node : *node_level) {
    int node_degree = node.second->degree;
    n_total_edges += node_degree;
    n_nodes_w_degree[node_degree]++;
  }
  // Divide by two because we double counted all edges
  n_total_edges /= 2.0;

  //==========================================================================
  // Next, we calculate the summation of N_k*ln(K!) where K is degree size and
  // N_k is the number of nodes of that degree

  // Calculate first component (sum of node degree counts portion)
  double degree_summation = 0.0;
  for (auto const& degree_count : n_nodes_w_degree) {
    // Using std's built in lgamma here: lgamma(x + 1) = log(x!)
    degree_summation += degree_count.second * lgamma(degree_count.first + 1);
  }

  //============================================================================
  // Last, we calculate the summation of e_rs*ln(e_rs/e_r*e_s)/2 where e_rs is
  // number of edges between blocks r and s and e_r is the total number of
  // edges for block r.

  // Grab all block nodes
  auto block_level = get_level(level + 1);

  // Turn level into a vector that can be iterated over
  NodeVec nodes_in_level;
  nodes_in_level.reserve(block_level->size());

  for (auto const& block : *block_level) {
    nodes_in_level.push_back(block.second);
  }

  // Now calculate the edge entropy betweeen nodes.
  double edge_entropy = 0.0;

  for (const auto& block_r : nodes_in_level) {
    edge_entropy += compute_node_edge_entropy(block_r);
  }

  // Add three components together to return
  return -1 * (n_total_edges + degree_summation + edge_entropy / 2);
}

// =============================================================================
// Merge two blocks, placing all nodes that were under block_b under block_a and
// deleting block_a from model.
// =============================================================================
void SBM::merge_blocks(NodePtr block_a, NodePtr block_b)
{
  PROFILE_FUNCTION();
  // Place all the members of block b under block a
  auto children_to_move = block_b->children;

  for (const NodePtr& member_node : children_to_move) {
    member_node->set_parent(block_a);
  }
}

// =============================================================================
// Merge blocks at a given level based on the best probability of doing so
// =============================================================================
Merge_Step SBM::agglomerative_merge(const int block_level,
                                    const int num_merges_to_make)
{
  PROFILE_FUNCTION();
  // Quick check to make sure reasonable request
  if (num_merges_to_make == 0) {
    throw "Zero merges requested.";
  }

  // Level that the block metablocks will sit at
  int meta_level = block_level + 1;

  // Build a single meta-block for each node at desired level
  give_every_node_at_level_own_block(block_level);

  // Grab all the blocks we're looking to merge
  LevelPtr all_blocks = get_level(block_level);

  // Build vectors for recording merges
  NodeVec             from_blocks;
  NodeVec             to_blocks;
  std::vector<double> move_delta;

  const int size_to_return = N_CHECKS_PER_block * all_blocks->size();
  from_blocks.reserve(size_to_return);
  to_blocks.reserve(size_to_return);
  move_delta.reserve(size_to_return);

  // Make sure doing a merge makes sense by checking we have enough blocks
  // of every type
  for (int i = 0; i < node_type_counts.size(); i++) {
    if (node_type_counts[i][block_level] < 2) {
      throw "To few blocks to perform merge.";
    }
  }

  // Loop over each block and find best merge option
  for (const auto& block : *all_blocks) {

    NodeVec metablocks_to_search;

    // If we're running algorithm in greedy mode we should just
    // add every possible block to the blocks-to-search list
    if (GREEDY) {
      // Get a list of all the potential merges for block
      metablocks_to_search = get_nodes_of_type_at_level(block.second->type, meta_level);
    }
    else {
      metablocks_to_search.reserve(N_CHECKS_PER_block);
      // Otherwise, we should sample a given number of blocks to check
      for (int i = 0; i < N_CHECKS_PER_block; i++) {
        // Sample a block from potential blocks
        metablocks_to_search.push_back(propose_move(block.second));
      }
    }

    // Now that we have gathered all the merges to check, we can loop
    // through them and check entropy changes
    for (const NodePtr& metablock : metablocks_to_search) {
      // Get block that the metablock belongs to
      NodePtr merge_block = *((metablock->children).begin());

      // Skip block if it is the current block for this node
      if (merge_block->id == block.second->id)
        continue;

      // Calculate entropy delta for move
      double entropy_delta = make_proposal_decision(block.second, metablock, true).entropy_delta;

      from_blocks.push_back(block.second);
      to_blocks.push_back(merge_block);
      move_delta.push_back(entropy_delta);
    }
  }

  // Now we find the top merges
  // Initialize a merge result holder struct
  Merge_Step results;

  // Priority queue to find best moves
  std::priority_queue<std::pair<double, int>> best_moves;

  for (int i = 0; i < move_delta.size(); ++i) {
    // Place this move's results in the queue.
    // The negative here means that when we pop the top value of
    // the queue we get the smallest value, not the largeset.
    best_moves.push(std::pair<double, int>(-move_delta[i], i));
  }

  // A set of the unique merges we've made
  std::unordered_set<string> merges_made;

  // Start working our way through the queue of best moves and making merges
  // if they are appropriate...
  bool more_merges_needed = true;
  bool queue_not_empty    = true;

  while (more_merges_needed & queue_not_empty) {
    // Extract index of best remaining merge
    int merge_index = best_moves.top().second;

    // Get blocks that are being merged (culled)
    NodePtr from_block = from_blocks[merge_index];
    NodePtr to_block   = to_blocks[merge_index];

    // Make sure we haven't already merged the culled block
    bool from_still_exists = merges_made.find(from_block->id) == merges_made.end();

    // Also make sure that we haven't removed the block we're trying to
    // merge into
    bool to_still_exists = merges_made.find(to_block->id) == merges_made.end();

    if (from_still_exists & to_still_exists) {
      // Insert new culled block into set
      merges_made.insert(from_block->id);

      // Merge the best block pair
      merge_blocks(to_block, from_block);

      // Record pair for results
      results.from_node.push_back(from_block->id);
      results.to_node.push_back(to_block->id);
    }

    // Remove the last index from our queue and go again
    best_moves.pop();

    // Update termination checking conditions
    more_merges_needed = merges_made.size() < num_merges_to_make;
    queue_not_empty    = best_moves.size() != 0;
  }

  // Erase the empty blocks and metablocks
  auto removed_blocks = clean_empty_blocks();

  // Return the entropy for new model and merges done
  results.entropy = compute_entropy(block_level - 1);

  return results;
}

// =============================================================================
// Run mcmc chain initialization by finding best organization
// of B' blocks for all B from B = N to B = 1.
// =============================================================================
std::vector<Merge_Step> SBM::collapse_blocks(const int  node_level,
                                             const int  num_mcmc_steps,
                                             const int  desired_num_blocks,
                                             const bool report_all_steps)
{
  PROFILE_FUNCTION();
  const int block_level = node_level + 1;

  // Start by giving every node at the desired level its own block
  give_every_node_at_level_own_block(node_level);

  // Grab reference to the block nodes container
  auto block_level_ptr = get_level(block_level);

  // A conservative estimate of how many steps collapsing will take as
  // anytime we're not doing an exhaustive search we will use less than
  // B_start - B_end moves.
  const int num_steps = block_level_ptr->size() - desired_num_blocks;

  // Setup vector to hold all merge step results.
  std::vector<Merge_Step> step_results;
  step_results.reserve(report_all_steps ? num_steps : 1);

  // Get the current number of blocks we have
  int curr_num_blocks = block_level_ptr->size();

  while (curr_num_blocks > desired_num_blocks) {
    // Decide how many merges we should do.
    int num_merges = int(curr_num_blocks - (curr_num_blocks / SIGMA));

    // Need to remove at least 1 block
    if (num_merges < 1) num_merges = 1;

    // Make sure we don't overstep the goal number of blocks
    const int num_blocks_after_merge = curr_num_blocks - num_merges;
    if (num_blocks_after_merge < desired_num_blocks) {
      num_merges = curr_num_blocks - desired_num_blocks;
    }

    Merge_Step merge_results;

    // Attempt merge step
    try {
      // Perform next best merge and record results
      merge_results = agglomerative_merge(
          block_level,
          num_merges);
    }
    catch (...) {
      std::cerr << "Collapsibility limit of network reached so we break early\n"
                << "There are currently " << curr_num_blocks << " blocks left.\n";

      // We reached the collapsibility limit of our network so we break early
      break;
    }

    if (num_mcmc_steps != 0) {
      // Let model equilibriate with new block layout...
      mcmc_sweep(node_level, num_mcmc_steps, false, false);

      clean_empty_blocks();
      // Update the step entropy results with new equilibriated model
      if (report_all_steps) merge_results.entropy = compute_entropy(node_level);
    }

    // Update current number of blocks
    curr_num_blocks = block_level_ptr->size();

    if (report_all_steps) {
      // Dump state into step results
      merge_results.state = get_state();

      // Record how many blocks we have after this step
      merge_results.num_blocks = curr_num_blocks;

      // Gather info for return
      step_results.push_back(merge_results);
    }
  } // End main while loop

  // Gather results if we're only reporting final result
  if (!report_all_steps) {
    // Gather info for return
    step_results.push_back(Merge_Step(
        compute_entropy(node_level),
        get_state(),
        curr_num_blocks));
  }

  return step_results;
}
