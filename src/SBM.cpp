#include "SBM.h"

// =============================================================================
// Propose a potential block move for a node.
// =============================================================================
NodePtr SBM::propose_move(const NodePtr node)
{
  PROFILE_FUNCTION();

  int block_level = node->level + 1;

  // Grab a list of all the blocks that the node could join
  std::vector<NodePtr> potential_blocks = get_nodes_of_type_at_level(
    node->type,
    block_level);

  // Sample a random neighbor of the current node
  NodePtr rand_neighbor = sampler.sample(
    node->get_edges_to_level(node->level)
  );

  // Get number total number edges for neighbor's block
  int neighbor_block_degree = rand_neighbor->parent->degree;

  // Decide if we are going to choose a random block for our node
  double ergo_amnt = EPS*potential_blocks.size();
  double prob_of_random_block = ergo_amnt/(neighbor_block_degree + ergo_amnt);

  // Decide where we will get new block from and draw from potential candidates
  return sampler.draw_unif() < prob_of_random_block ?
    sampler.sample(potential_blocks):
    sampler.sample(rand_neighbor->get_edges_to_level(block_level));
}


inline static void process_block_pair(
    const std::map<NodePtr, int>::iterator con_block_it,
    const double edges_from_node,
    const double moved_degree_pre,
    const double moved_degree_post,
    double * entropy_pre,
    double * entropy_post)
{

  const double neighbor_deg = con_block_it->first->degree;
  const double edge_count_pre = con_block_it->second;

  // The old and new edge counts we need to compute entropy
  // If we're looking at the neighbor blocks for the old block we need to
  // subtract the edges the node contributed, otherwise we need to add.
  double edge_count_post = edge_count_pre + edges_from_node;

  // Calculate entropy contribution pre move
  double entropy_pre_delta = edge_count_pre > 0 ? edge_count_pre *
                                                      log(edge_count_pre / (moved_degree_pre * neighbor_deg))
                                                : 0;

  // Calculate entropy contribution post move
  double entropy_post_delta = edge_count_post > 0 ? edge_count_post *
                                                        log(edge_count_post / (moved_degree_post * neighbor_deg))
                                                  : 0;

  (*entropy_pre) += entropy_pre_delta;
  (*entropy_post) += entropy_post_delta;
}

// =============================================================================
// Make a decision on the proposed new block for node
// =============================================================================
Proposal_Res SBM::make_proposal_decision(const NodePtr node,
                                         const NodePtr new_block)
{
  PROFILE_FUNCTION();
  // The level that this proposal is taking place on
  int block_level = node->level + 1;

  // Reference to old block that would be swapped for new_block
  NodePtr old_block = node->parent;

  // Grab degree of the node to move
  double node_degree = node->degree;

  // Get degrees of the two blocks pre-move
  double old_block_degree_pre = old_block->degree;
  double new_block_degree_pre = new_block->degree;

  // Get degrees of the two blocks post-move
  double old_block_degree_post = old_block_degree_pre - node_degree;
  double new_block_degree_post = new_block_degree_pre + node_degree;

  // Initialize the partial edge entropy sum holders
  double entropy_pre = 0;
  double entropy_post = 0;

  // Gather edge maps for the node and its moved blocks as these will have
  // changes in their entropy contribution
  std::map<NodePtr, int> node_edges = node->gather_edges_to_level(block_level);

  std::map<NodePtr, int> new_block_edges = new_block->gather_edges_to_level(block_level);

  std::map<NodePtr, int> old_block_edges = old_block->gather_edges_to_level(block_level);

  for (auto con_block_it = old_block_edges.begin();
       con_block_it != old_block_edges.end();
       con_block_it++)
  {
    process_block_pair(con_block_it,
                       -node_edges[con_block_it->first],
                       old_block_degree_pre,
                       old_block_degree_post,
                       &entropy_pre,
                       &entropy_post);
  }

  // Do the same for the new block - neighbor
  for (auto con_block_it = new_block_edges.begin();
       con_block_it != new_block_edges.end();
       con_block_it++)
  {
    process_block_pair(con_block_it,
                       node_edges[con_block_it->first],
                       new_block_degree_pre,
                       new_block_degree_post,
                       &entropy_pre,
                       &entropy_post);
  }

  // Now we move on to calculating the probability ratios for the node moving
  // from old->new and then new->old assuming node was already in new.
  double pre_move_prob = 0.0;
  double post_move_prob = 0.0;

  // Loop over all the node's edges to neighbor blocks
  for (auto con_block_it = node_edges.begin();
       con_block_it != node_edges.end();
       con_block_it++)
  {
    // Edges from new block to t pre move...
    pre_move_prob += old_block_edges[con_block_it->first] + EPS;

    // Edges from old block to t post move...
    post_move_prob += new_block_edges[con_block_it->first] + EPS;
  }

  // Now we can clean up all the calculations into to entropy delta and the
  // probability ratio for the moves and use those to calculate the acceptance
  // probability for the proposed move.
  double entropy_delta = entropy_post - entropy_pre;
  double acceptance_prob = exp(entropy_delta) * (pre_move_prob/post_move_prob);

  return Proposal_Res(
    entropy_delta,
    acceptance_prob > 1 ? 1: acceptance_prob
  );
}

// =============================================================================
// Runs efficient MCMC sweep algorithm on desired node level
// =============================================================================
Sweep_Res SBM::mcmc_sweep(const int level, const bool variable_num_blocks)
{
  PROFILE_FUNCTION();
  const int block_level = level + 1;

  // Initialize the results holder
  Sweep_Res results;

  // Grab level map
  LevelPtr node_map = get_level(level);

  // Get all the nodes at the given level in a shuffleable vector format
  // Initialize vector to hold nodes
  std::vector<NodePtr> node_vec;
  node_vec.reserve(node_map->size());

  // Fill in vector with map elements
  for (auto node_it = node_map->begin();
            node_it != node_map->end();
            node_it++)
  {
    node_vec.push_back(node_it->second);
  }

  // Shuffle node order
  std::shuffle(node_vec.begin(), node_vec.end(), sampler.int_gen);

  // Loop through each node
  for (auto node_it = node_vec.begin(); node_it != node_vec.end(); ++node_it)
  {
    NodePtr curr_node = *node_it;

    // Check if we're running sweep with variable block numbers. If we are, we
    // need to add a new block as a potential for the node to enter
    if (variable_num_blocks)
    {
      // Add a new block node for the current node type
      create_block_node(curr_node->type, block_level);
    }

    // Get a move proposal
    NodePtr proposed_new_block = propose_move(curr_node);

    // If the propsosed block is the nodes current block, we don't need to waste
    // time checking because decision will always result in same state.
    if(curr_node->parent->id == proposed_new_block->id) continue;

    // Calculate acceptance probability based on posterior changes
    Proposal_Res proposal_results = make_proposal_decision(
      curr_node,
      proposed_new_block
    );

    bool move_accepted = sampler.draw_unif() < proposal_results.prob_of_accept;

    if (move_accepted)
    {
      // Move the node
      curr_node->set_parent(proposed_new_block);

      // Update results
      results.nodes_moved.push_back(curr_node->id);
      results.new_groups.push_back(proposed_new_block->id);
      results.entropy_delta += proposal_results.entropy_delta;
      
      // If we are varying number of blocks and we made a move we should clean
      // up the now potentially empty blocks for the next node proposal. 
      if (variable_num_blocks)
      {
        clean_empty_blocks();
      }
    }

  } // End loop over all nodes

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
  for (auto node_it = node_level->begin();
            node_it != node_level->end();
            ++node_it)
  {
    int node_degree = node_it->second->degree;
    n_total_edges += node_degree;
    n_nodes_w_degree[node_degree]++;
  }
  // Divide by two because we double counted all edges
  n_total_edges/=2.0;

  //==========================================================================
  // Next, we calculate the summation of N_k*ln(K!) where K is degree size and
  // N_k is the number of nodes of that degree

  // Calculate first component (sum of node degree counts portion)
  double degree_summation = 0.0;
  for (auto degree_count =  n_nodes_w_degree.begin();
            degree_count != n_nodes_w_degree.end();
            ++degree_count)
  {

    // Using std's built in lgamma here: lgamma(x + 1) = log(x!)
    degree_summation += degree_count->second * lgamma(degree_count->first + 1);
  }

  //============================================================================
  // Last, we calculate the summation of e_rs*ln(e_rs/e_r*e_s)/2 where e_rs is
  // number of edges between blocks r and s and e_r is the total number of
  // edges for block r.

  // Grab all block nodes
  auto block_level = get_level(level + 1);

  double edge_entropy = 0.0;

  // Full loop over all block nodes
  for (auto block_r_it = block_level->begin();
       block_r_it != block_level->end();
       block_r_it++)
  {
    NodePtr block_r = block_r_it->second;
    // Gather all of block r's edges to our level
    auto block_r_edge_counts = block_r->gather_edges_to_level(level + 1);

    // Now loop over all the nodes connected to block r
    for (auto block_s_it = block_r_edge_counts.begin();
         block_s_it != block_r_edge_counts.end();
         block_s_it++)
    {
      // Grab total number of edges between r and s
      double e_rs = block_s_it->second;

      // Compute this iteration's controbution to sum
      edge_entropy += e_rs * log(e_rs / double(block_r->degree * block_s_it->first->degree));
    } // end block s loop
  }   // end block r loop

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

  for (NodePtr member_node : children_to_move)
  {
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
  std::vector<NodePtr> from_blocks;
  std::vector<NodePtr> to_blocks;
  std::vector<double> move_delta;

  const int size_to_return = N_CHECKS_PER_block * all_blocks->size();
  from_blocks.reserve(size_to_return);
  to_blocks.reserve(size_to_return);
  move_delta.reserve(size_to_return);

  // Make sure doing a merge makes sense by checking we have enough blocks
  // of every type
  for (int i = 0; i < node_type_counts.size(); i++)
  {
    if (node_type_counts[i][block_level] < 2)
    {
      throw "To few blocks to perform merge.";
    }
  }

  // Loop over each block and find best merge option
  for (auto block_it = all_blocks->begin();
            block_it != all_blocks->end();
            block_it++)
  {
    NodePtr curr_block = block_it->second;

    std::vector<NodePtr> metablocks_to_search;

    // If we're running algorithm in greedy mode we should just
    // add every possible block to the blocks-to-search list
    if (GREEDY)
    {
      // Get a list of all the potential merges for block
      metablocks_to_search =
          get_nodes_of_type_at_level(curr_block->type, meta_level);
    }
    else
    {
      metablocks_to_search.reserve(N_CHECKS_PER_block);
      // Otherwise, we should sample a given number of blocks to check
      for (int i = 0; i < N_CHECKS_PER_block; i++)
      {
        // Sample a block from potential blocks
        metablocks_to_search.push_back(propose_move(curr_block));
      }
    }

    // Now that we have gathered all the merges to check, we can loop
    // through them and check entropy changes
    for (NodePtr metablock : metablocks_to_search)
    {
      // Get block that the metablock belongs to
      NodePtr merge_block = *((metablock->children).begin());

      // Skip block if it is the current block for this node
      if (merge_block->id == curr_block->id)
        continue;

      // Calculate entropy delta for move
      double entropy_delta = make_proposal_decision(
                                 curr_block,
                                 metablock)
                                 .entropy_delta;

      from_blocks.push_back(curr_block);
      to_blocks.push_back(merge_block);
      move_delta.push_back(entropy_delta);
    }
  }

  // Now we find the top merges
  // Initialize a merge result holder struct
  Merge_Step results;

  // Priority queue to find best moves
  std::priority_queue<std::pair<double, int>> best_moves;

  for (int i = 0; i < move_delta.size(); ++i)
  {
    // Place this move's results in the queue
    best_moves.push(std::pair<double, int>(move_delta[i], i));
  }

  // A set of the unique merges we've made
  std::unordered_set<string> merges_made;

  // Start working our way through the queue of best moves and making merges
  // if they are appropriate...
  bool more_merges_needed = true;
  bool queue_not_empty = true;

  while (more_merges_needed & queue_not_empty)
  {
    // Extract index of best remaining merge
    int merge_index = best_moves.top().second;

    // Get blocks that are being merged (culled)
    NodePtr from_block = from_blocks[merge_index];
    NodePtr to_block = to_blocks[merge_index];

    // Make sure we haven't already merged the culled block
    bool from_still_exists =
        merges_made.find(from_block->id) == merges_made.end();

    // Also make sure that we haven't removed the block we're trying to
    // merge into
    bool to_still_exists = merges_made.find(to_block->id) == merges_made.end();

    if (from_still_exists & to_still_exists)
    {
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
    queue_not_empty = best_moves.size() != 0;
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
std::vector<Merge_Step> SBM::collapse_blocks(const int node_level,
                                             const int num_mcmc_steps,
                                             const int desired_num_blocks,
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
  step_results.reserve(report_all_steps ? num_steps: 1);

  // Get the current number of blocks we have
  int curr_num_blocks = block_level_ptr->size();

  while (curr_num_blocks > desired_num_blocks)
  {
    // Decide how many merges we should do.
    int num_merges = int(curr_num_blocks - (curr_num_blocks / SIGMA));

    // Need to remove at least 1 block
    if (num_merges < 1) num_merges = 1;

    // Make sure we don't overstep the goal number of blocks
    const int num_blocks_after_merge = curr_num_blocks - num_merges;
    if (num_blocks_after_merge < desired_num_blocks)
    {
      num_merges = curr_num_blocks - desired_num_blocks;
    }

    Merge_Step merge_results;

    // Attempt merge step
    try
    {
      // Perform next best merge and record results
      merge_results = agglomerative_merge(
          block_level,
          num_merges);
    }
    catch (...)
    {
      std::cerr << "Collapsibility limit of network reached so we break early\n"
                << "There are currently " << curr_num_blocks << " blocks left.\n";

      // We reached the collapsibility limit of our network so we break early
      break;
    }

    if (num_mcmc_steps != 0)
    {
      // Let model equilibriate with new block layout...
      for (int j = 0; j < num_mcmc_steps; j++)
      {
        mcmc_sweep(node_level, false);
      }
      clean_empty_blocks();
      // Update the step entropy results with new equilibriated model
      if (report_all_steps) merge_results.entropy = compute_entropy(node_level);
    }

    // Update current number of blocks
    curr_num_blocks = block_level_ptr->size();

    if (report_all_steps) 
    {
      // Dump state into step results
      merge_results.state = get_state();

      // Record how many blocks we have after this step
      merge_results.num_blocks = curr_num_blocks;

      // Gather info for return
      step_results.push_back(merge_results);
    }
  } // End main while loop

  // Gather results if we're only reporting final result
  if (!report_all_steps) 
    {
      // Gather info for return
      step_results.push_back(Merge_Step(
        compute_entropy(node_level),
        get_state(),
        curr_num_blocks
      ));
    }

  return step_results;
}
