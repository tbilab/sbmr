#include "SBM.h"

#include "sbm_helpers.h"

// =============================================================================
// Propose a potential block move for a node.
// =============================================================================
NodePtr SBM::propose_move(const NodePtr node, const double eps)
{
  PROFILE_FUNCTION();

  const int block_level = node->level + 1;
  const int node_type = node->type;

  // Grab a list of all the blocks that the node could join
  const NodeVec potential_blocks = get_nodes_of_type_at_level(node_type, block_level);

  // Sample a random neighbor of node
  const NodePtr rand_neighbor = sampler.sample(node->edges)->get_parent_at_level(node->level);

  // Get number total number edges for neighbor's block
  const int neighbor_block_degree = rand_neighbor->parent->degree;

  // Decide if we are going to choose a random block for our node
  const double ergo_amnt            = eps * potential_blocks.size();
  const double prob_of_random_block = ergo_amnt / (neighbor_block_degree + ergo_amnt);

  // Decide where we will get new block from and draw from potential candidates
  return sampler.draw_unif() < prob_of_random_block ? sampler.sample(potential_blocks)
                                                    : sampler.sample(rand_neighbor->get_edges_to_level(block_level, node_type));
}

// =============================================================================
// Make a decision on the proposed new block for node
// =============================================================================
Proposal_Res SBM::make_proposal_decision(const NodePtr node,
                                         const NodePtr new_block,
                                         const double  eps)
{
  PROFILE_FUNCTION();

  const NodePtr old_block = node->parent; // Reference to old block that would be swapped for new_block
  // Make sure we're actually doing something
  if (old_block == new_block) {
    return Proposal_Res(0.0, 0.0);
  }

  const double node_degree = node->degree;
  const double block_level = new_block->level; // The level that this proposal is taking place on

  // Setup a struct to hold all the info we need about a given pair
  struct Node_Move_Cons {
    int old_to_neighbor  = 0;
    int new_to_neighbor  = 0;
    int node_to_neighbor = 0;
  };
  std::unordered_map<NodePtr, Node_Move_Cons> move_edge_counts;

  // Gather the node to edge counts together to one main map
  int node_to_old_block = 0;
  int node_to_new_block = 0;

  for (const auto& edge : old_block->edges) {
    move_edge_counts[edge->get_parent_at_level(block_level)].old_to_neighbor++;
  }

  for (const auto& edge : new_block->edges) {
    move_edge_counts[edge->get_parent_at_level(block_level)].new_to_neighbor++;
  }

  for (const auto& edge : node->edges) {
    const NodePtr edge_block = edge->get_parent_at_level(block_level);
    
    if (edge_block == old_block) {
      node_to_old_block++;
    }
    else if (edge_block == new_block) {
      node_to_new_block++;
    }

    move_edge_counts[edge_block].node_to_neighbor++;
  }

  // How many possible neighbor blocks are there?
  // Loop over all the possible neighbor node types for this node and add up.
  int n_possible_neighbors = 0;

  const auto possible_neighbor_types = edge_type_pairs.at(node->type);
  for (const auto& neighbor_type : possible_neighbor_types) {
    n_possible_neighbors += node_type_counts[neighbor_type][block_level];
  }

  // These are constants for edge connections that are used in entropy calc
  const int node_to_old_new_delta = node_to_old_block - node_to_new_block;
  const int pre_old_degree        = old_block->degree;
  const int post_old_degree       = pre_old_degree - node_degree;
  const int pre_new_degree        = new_block->degree;
  const int post_new_degree       = pre_new_degree + node_degree;

  // These will get summed into as we loop over all the neighbor blocks
  double entropy_delta  = 0;
  double pre_move_prob  = 0;
  double post_move_prob = 0;

  for (const auto& move_edges : move_edge_counts) {
    const NodePtr&        neighbor = move_edges.first;
    const Node_Move_Cons& pre      = move_edges.second;

    // Degree of neighbor group before move
    const int pre_neighbor_degree = neighbor->degree;

    // Initialize variables that will get changed depending on what the neighbor group is
    int    post_old_to_neighbor = pre.old_to_neighbor;
    int    post_new_to_neighbor = pre.new_to_neighbor;
    double scalar               = 1; // If we are double counting this pair we will need to downweight it

    // This will stay the same unless the neighbor is one of the old or new blocks
    int post_neighbor_degree = pre_neighbor_degree;

    const bool neighbor_is_old = neighbor == old_block;
    const bool neighbor_is_new = neighbor == new_block;

    if (neighbor_is_old) {
      post_old_to_neighbor -= 2 * (node_to_old_block);
      post_new_to_neighbor += node_to_old_new_delta;
      post_neighbor_degree = post_old_degree;
      scalar               = 2;
    }
    else if (neighbor_is_new) {
      post_old_to_neighbor += node_to_old_new_delta;
      post_new_to_neighbor += 2 * node_to_new_block;
      post_neighbor_degree = post_new_degree;
      scalar               = 2;
    }
    else {
      post_old_to_neighbor -= pre.node_to_neighbor;
      post_new_to_neighbor += pre.node_to_neighbor;
    }

    // First calculate old group's entropy contributions pre and post move
    const double pre_old_entropy  = partial_entropy(pre.old_to_neighbor, pre_neighbor_degree, pre_old_degree);
    const double post_old_entropy = partial_entropy(post_old_to_neighbor, post_neighbor_degree, post_old_degree);

    // Then do the same for the new group
    const double pre_new_entropy  = partial_entropy(pre.new_to_neighbor, pre_neighbor_degree, pre_new_degree);
    const double post_new_entropy = partial_entropy(post_new_to_neighbor, post_neighbor_degree, post_new_degree);

    // Add this neighbors contribution to the overall delta
    entropy_delta += (pre_old_entropy + pre_new_entropy - post_old_entropy - post_new_entropy) / scalar;

    // Before moving calculating probability ratio components for neighbor we
    // first check if node being moved has any connections to this neighbor
    if (pre.node_to_neighbor != 0) {
      const double prop_edges_to_neighbor = pre.node_to_neighbor / node_degree;
      const double eps_B                  = eps * n_possible_neighbors;

      pre_move_prob += prop_edges_to_neighbor * (pre.new_to_neighbor + eps) / (pre_neighbor_degree + eps_B);
      post_move_prob += prop_edges_to_neighbor * (post_old_to_neighbor + eps) / (post_neighbor_degree + eps_B);
    }
  } // End main neighbor loop

  // Multiply both exponential of entropy delta and prob ratio to get the acceptance probability
  return Proposal_Res(entropy_delta, exp(-entropy_delta) * (post_move_prob / pre_move_prob));
}

// =============================================================================
// Runs efficient MCMC sweep algorithm on desired node level
// =============================================================================
MCMC_Sweeps SBM::mcmc_sweep(const int    level,
                            const int    num_sweeps,
                            const double eps,
                            const bool   variable_num_blocks,
                            const bool   track_pairs,
                            const bool   verbose)
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

  if (verbose) {
    std::cout << "sweep_num,"
              << "node,"
              << "current_block,"
              << "proposed_block,"
              << "entropy_delta,"
              << "prob_of_accept,"
              << "move_accepted" << std::endl;
  }

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
      // need to make sure we don't have any extra unoccupied blocks floating around,
      // then we need to add a new block as a potential for the node to enter
      if (variable_num_blocks) {
        clean_empty_blocks();
        create_block_node(curr_node->type, block_level);
      }

      // Get a move proposal
      const NodePtr proposed_new_block = propose_move(curr_node, eps);

      // If the proposed block is the nodes current block, we don't need to waste
      // time checking because decision will always result in same state.
      if ((curr_node->parent)->id == proposed_new_block->id) {
        continue;
      }

      if (verbose) {
        std::cout << i
                  << "," << curr_node->id
                  << "," << (curr_node->parent)->id
                  << "," << proposed_new_block->id
                  << ",";
      }
      // Calculate acceptance probability based on posterior changes
      Proposal_Res proposal_results = make_proposal_decision(curr_node, proposed_new_block, eps);

      // Make movement decision
      const bool move_accepted = proposal_results.prob_of_accept > sampler.draw_unif();

      if (verbose) {
        std::cout << proposal_results.entropy_delta << "," << proposal_results.prob_of_accept << ","
                  << move_accepted << std::endl;
      }

      // Is the move accepted?
      if (move_accepted) {
        const NodePtr old_block = curr_node->parent;

        // Move the node
        curr_node->set_parent(proposed_new_block);

        // Update results
        results.nodes_moved.push_back(curr_node->id);
        num_nodes_moved++;
        entropy_delta += proposal_results.entropy_delta;

        if (track_pairs) {
          Block_Consensus::update_changed_pairs(curr_node, old_block->children, proposed_new_block->children,
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
double SBM::get_entropy(const int level)
{
  PROFILE_FUNCTION();
  //============================================================================
  // First, calc the number of total edges and build a degree->num nodes map

  // Build map of number of nodes with given degree
  std::map<int, int> n_nodes_w_degree;

  // Keep track of total number of edges as well
  double n_total_edges = 0;

  // Grab pointer to current level and start loop
  const LevelPtr node_level = get_level(level);
  for (const auto& node : *node_level) {
    const int node_degree = node.second->degree;
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
  for (const auto& degree_count : n_nodes_w_degree) {
    // Using std's built in lgamma here: lgamma(x + 1) = log(x!)
    degree_summation += degree_count.second * lgamma(degree_count.first + 1);
  }

  //============================================================================
  // Last, we calculate the summation of e_rs*ln(e_rs/e_r*e_s)/2 where e_rs is
  // number of edges between blocks r and s and e_r is the total number of
  // edges for block r.

  // Grab all block nodes
  const LevelPtr block_level = get_level(level + 1);

  if (block_level->size() == 0)
  {
     throw "Can't compute entropy for network with no block structure.";
  }

  // Now calculate the edge entropy betweeen nodes.
  double edge_entropy = 0.0;

  // Gather block-to-block edge counts
  const std::map<Edge, int> block_edges = gather_block_counts_at_level(level + 1);
  for (const auto& block_edge : block_edges) {
    const NodePtr block_r = block_edge.first.node_a;
    const NodePtr block_s = block_edge.first.node_b;

    if (block_r == block_s) {
      // Double self-edge counts (we're looking at half-edges)
      // and downweight contribution by half as they get seen the proper
      // number of times as compared to the others which are getting seen
      // half as much as we would expect due to non-duplicating pairs
      edge_entropy += partial_entropy(block_edge.second * 2,
                                      block_r->degree,
                                      block_s->degree)
          / 2;
    }
    else {
      edge_entropy += partial_entropy(block_edge.second,
                                      block_r->degree,
                                      block_s->degree);
    }
  }

  // Add three components together to return
  return -1 * (n_total_edges + degree_summation + edge_entropy);
}

// =============================================================================
// Merge two blocks, placing all nodes that were under block_b under block_a and
// deleting block_a from model.
// =============================================================================
void SBM::merge_blocks(NodePtr absorbing_block, NodePtr absorbed_block)
{
  PROFILE_FUNCTION();
  // Place all the members of block b under block a
  const ChildSet children_to_move = absorbed_block->children;
  for (const NodePtr& child_node : children_to_move) {
    child_node->set_parent(absorbing_block);
  }

  // Remove node and all its parents from their respective level blocks
  NodePtr current_node = absorbed_block;
  while (current_node) {
    // Delete the now absorbed block from level map
    get_level(current_node->level)->erase(current_node->id);

    // Remove nodes contribution to node counts map
    node_type_counts[current_node->type][current_node->level]--;

    current_node = current_node->parent;
  }
}

// =============================================================================
// Merge blocks at a given level based on the best probability of doing so
// =============================================================================
Merge_Step SBM::agglomerative_merge(const int    block_level,
                                    const int    num_merges_to_make,
                                    const int    num_checks_per_block,
                                    const double eps)
{
  PROFILE_FUNCTION();
  // Quick check to make sure reasonable request
  if (num_merges_to_make <= 0) {
    throw "Zero merges requested.";
  }

  // Level that the block metablocks will sit at
  const int meta_level = block_level + 1;

  if (nodes.count(meta_level) < 1) {
    // Build a single meta-block for each block if they don't exist already
    give_every_node_at_level_own_block(block_level);
  }

  // Grab all the blocks we're looking to merge
  const LevelPtr all_blocks = get_level(block_level);

  // Priority queue to find best moves
  std::priority_queue<std::pair<double, std::pair<NodePtr, NodePtr>>> best_moves_q;

  // Set to keep track of what pairs of nodes we have checked already so we dont double check
  std::unordered_set<std::string> checked_pairs;

  // Make sure doing a merge makes sense by checking we have enough blocks of every type
  for (int i = 0; i < node_type_counts.size(); i++) {
    if (node_type_counts[i][block_level] < 2) {
      throw "To few blocks to perform merge.";
    }
  }

  // Loop over each block and find best merge option
  for (const auto& block : *all_blocks) {

    NodeVec metablocks_to_search;

    // No point in running M checks if there are < M blocks left.
    const bool less_blocks_than_checks = node_type_counts[block.second->type][meta_level] <= num_checks_per_block;
    if (less_blocks_than_checks) {
      // Get a list of all the potential metablocks for block
      metablocks_to_search = get_nodes_of_type_at_level(block.second->type, meta_level);
    }
    else {
      metablocks_to_search.reserve(num_checks_per_block);
      // Otherwise, we should sample a given number of blocks to check
      for (int i = 0; i < num_checks_per_block; i++) {
        // Sample a metablock from potentials
        metablocks_to_search.push_back(propose_move(block.second, eps));
      }
    }

    // Now that we have gathered all the merges to check, we can loop
    // through them and check entropy changes
    for (const NodePtr& metablock : metablocks_to_search) {
      // Get block that the metablock belongs to
      const NodePtr merge_block = *((metablock->children).begin());

      // Skip block if it is the current block for this node
      if (merge_block->id == block.second->id) {
        continue;
      }

      // See if this combo of groups has already been looked at
      const bool unchecked_pair = checked_pairs.insert(make_pair_key(merge_block, block.second)).second;

      if (unchecked_pair) {

        const NodePtr& block_a = merge_block;
        const NodePtr& block_b = block.second;

        // Build a map of neighbor to pair of both groups connections to that neighbor.
        std::unordered_map<NodePtr, std::pair<int, int>> pair_counts_to_neighbor;

        int        e_ab_ab           = 0;
        int        times_merged_seen = 0;
        const auto block_a_counts    = block_a->gather_edges_to_level(block_level);
        for (const auto& block_a_count : block_a_counts) {
          pair_counts_to_neighbor[block_a_count.first].first = block_a_count.second;
          if (block_a_count.first == block_a | block_a_count.first == block_b) {
            e_ab_ab += block_a_count.second;
            times_merged_seen++;
          }
        }

        const auto block_b_counts = block_b->gather_edges_to_level(block_level);
        for (const auto& block_b_count : block_b_counts) {
          pair_counts_to_neighbor[block_b_count.first].second = block_b_count.second;
          if (block_b_count.first == block_a | block_b_count.first == block_b) {
            e_ab_ab += block_b_count.second;
            times_merged_seen++;
          }
        }

        const double e_a  = block_a->degree; // Degree of a before merge
        const double e_b  = block_b->degree; // Degree of b before merge
        const double e_ab = e_a + e_b;       // Degree of merged group

        double entropy_delta = 0;
        for (const auto& edge_counts : pair_counts_to_neighbor) {
          const NodePtr& block_s = edge_counts.first;

          const double e_a_s = edge_counts.second.first;
          const double e_b_s = edge_counts.second.second;
          const double e_s   = block_s->degree;

          entropy_delta += partial_entropy(e_a_s, e_a, e_s) + partial_entropy(e_b_s, e_b, e_s);

          const bool   is_merged = (block_s == block_b) | (block_s == block_a);
          const double e_ab_s    = is_merged ? e_ab_ab : e_a_s + e_b_s;
          const double e_s_post  = is_merged ? e_ab : e_s;

          // If we have multiples instances of merged group in neighbors and this is
          // the block_b (arbitrary) we dont want to count its entropy contribution
          // because we would be double counting
          const bool count_post_merge = !((block_s == block_b) & (times_merged_seen > 1));
          if (count_post_merge) {
            entropy_delta -= partial_entropy(e_ab_s, e_ab, e_s_post);
          }
        }

        // Calculate entropy delta for move and place this move's results in the queue.
        best_moves_q.push(std::make_pair(
            -entropy_delta,
            std::make_pair(block.second, merge_block)));
      }
    }
  }

  // Now we find the top merges...
  // Start by initializing a merge result struct
  Merge_Step results;

  // A set of the blocks that have been merged already this step and thus are off limits
  std::unordered_set<NodePtr> merged_blocks;
  int                         num_merges_made = 0;

  // Start working our way through the queue of best moves and making merges
  while ((num_merges_made < num_merges_to_make) & (best_moves_q.size() != 0)) {
    // Extract best remaining merge
    const auto best_merge = best_moves_q.top().second;

    // Make sure we haven't already merged the culled block
    // Also make sure that we haven't removed the block we're trying to merge into
    const bool pair_unmerged = merged_blocks.insert(best_merge.first).second & merged_blocks.insert(best_merge.second).second;

    if (pair_unmerged) {
      const double merge_entropy_delta = -best_moves_q.top().first; // we stored the negative entropy delta so we need to subtract

      // Merge the best block pair
      merge_blocks(best_merge.second, best_merge.first);

      // Record pair for results
      results.entropy_delta += merge_entropy_delta;
      results.from_node.push_back(best_merge.first->id);
      results.to_node.push_back(best_merge.second->id);
      num_merges_made++;
    }

    // Remove the last index from our queue and go again
    best_moves_q.pop();
  }

  return results;
}

// =============================================================================
// Run mcmc chain initialization by finding best organization
// of B' blocks for all B from B = N to B = 1.
// =============================================================================
std::vector<Merge_Step> SBM::collapse_blocks(const int    node_level,
                                             const int    num_mcmc_steps,
                                             const int    desired_num_blocks,
                                             const int    num_checks_per_block,
                                             const double sigma,
                                             const double eps,
                                             const bool   report_all_steps)
{
  PROFILE_FUNCTION();
  const int block_level = node_level + 1;

  // Start by giving every node at the desired level its own block and every
  // one of those blocks its own metablock
  give_every_node_at_level_own_block(node_level);
  give_every_node_at_level_own_block(block_level);

  // Calculate initial entropy for model before merging is done
  const double initial_entropy = get_entropy(node_level);

  // Grab reference to the block nodes container
  const LevelPtr block_level_ptr = get_level(block_level);

  // Get the current number of blocks we have (gets updated in while loop so not const)
  int curr_num_blocks = block_level_ptr->size();

  // A conservative estimate of how many steps collapsing will take as
  // anytime we're not doing an exhaustive search we will use less than
  // B_start - B_end moves.
  const int num_steps = curr_num_blocks - desired_num_blocks;

  // Setup vector to hold all merge step results.
  std::vector<Merge_Step> step_results;
  step_results.reserve(report_all_steps ? num_steps : 1);

  // Counter to calculate the total entropy delta of this collapse run. Only used when not reporting all results
  double total_entropy_delta = 0;

  while (curr_num_blocks > desired_num_blocks) {
    // Decide how many merges we should do. Make sure we don't overstep the goal
    // number of blocks and we need to remove at least 1 block
    const int num_merges = std::max(
        std::min(
            curr_num_blocks - desired_num_blocks,
            int(curr_num_blocks - (curr_num_blocks / sigma))),
        1);

    Merge_Step merge_results;

    // Attempt merge step
    try {
      // Perform next best merge and record results
      merge_results = agglomerative_merge(block_level, num_merges, num_checks_per_block, eps);
    }
    catch (...) {
      std::cerr << "Collapsibility limit of network reached so we break early\n"
                << "There are currently " << curr_num_blocks << " blocks left.\n";

      // We reached the collapsibility limit of our network so we break early
      break;
    }

    if (num_mcmc_steps != 0) {
      // Let model equilibriate with new block layout...
      const std::vector<double> sweep_entropy_deltas     = mcmc_sweep(node_level, num_mcmc_steps, eps, false, false).sweep_entropy_delta;
      double                    mcmc_sweep_delta_changes = 0;
      // Update the step entropy delta with the entropy delta from the sweeps
      for (const double& sweep_delta : sweep_entropy_deltas) {
        mcmc_sweep_delta_changes += sweep_delta;
      }

      merge_results.entropy_delta += mcmc_sweep_delta_changes;
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
    else {
      // If were just reporting the end result we need to update our entropy delta
      total_entropy_delta += merge_results.entropy_delta;
    }
  } // End main while loop

  // Gather results if we're only reporting final result
  if (!report_all_steps) {
    // Gather info for return
    step_results.push_back(Merge_Step(total_entropy_delta, get_state(), curr_num_blocks));
    step_results[0].entropy = initial_entropy + total_entropy_delta;
  } else {
    // Update the results entropy values with the true entropy rather than the delta provided. 
    double current_entropy = initial_entropy;
    for (auto& step_result : step_results) {
      step_result.entropy = current_entropy + step_result.entropy_delta;
      current_entropy = step_result.entropy;
    }
  }

  return step_results;
}