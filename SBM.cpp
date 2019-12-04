#include "SBM.h" 


// =============================================================================
// Propose a potential group move for a node.
// =============================================================================
NodePtr SBM::propose_move(const NodePtr node)
{
  PROFILE_FUNCTION();
  int group_level = node->level + 1;
  
  // Grab a list of all the groups that the node could join
  std::vector<NodePtr> potential_groups = get_nodes_of_type_at_level(
    node->type,
    group_level);

  // Sample a random neighbor of the current node
  NodePtr rand_neighbor = sampler.sample(
    node->get_connections_to_level(node->level)
  );
  
  // Get number total number connections for neighbor's group
  int neighbor_group_degree = rand_neighbor->parent->degree;
  
  // Decide if we are going to choose a random group for our node
  double ergo_amnt = EPS*potential_groups.size();
  double prob_of_random_group = ergo_amnt/(neighbor_group_degree + ergo_amnt);
  
  // Decide where we will get new group from and draw from potential candidates
  return sampler.draw_unif() < prob_of_random_group ?
    sampler.sample(potential_groups):
    sampler.sample(rand_neighbor->get_connections_to_level(group_level));
}

// =============================================================================
// Make a decision on the proposed new group for node
// =============================================================================
Proposal_Res SBM::make_proposal_decision(const NodePtr node,
                                         const NodePtr new_group)
{
  PROFILE_FUNCTION();
  // The level that this proposal is taking place on
  int group_level = node->level + 1;

  // Reference to old group that would be swapped for new_group
  NodePtr old_group = node->parent;
  
  // Grab degree of the node to move
  double node_degree = node->degree;

  // Get degrees of the two groups pre-move
  double old_group_degree_pre = old_group->degree;
  double new_group_degree_pre = new_group->degree;

  // Get degrees of the two groups post-move
  double old_group_degree_post = old_group_degree_pre - node_degree;
  double new_group_degree_post = new_group_degree_pre + node_degree;
 
  // Initialize the partial edge entropy sum holders
  double entropy_pre = 0;
  double entropy_post = 0;

  // Gather connection maps for the node and its moved groups as these will have
  // changes in their entropy contribution
  std::map<NodePtr, int> node_edges = node->gather_connections_to_level(group_level);

  std::map<NodePtr, int> new_group_edges = new_group->gather_connections_to_level(group_level);

  std::map<NodePtr, int> old_group_edges = old_group->gather_connections_to_level(group_level);
 
  // Initialize edge counts to hold new and old group counts to connected groups
  EdgeCounts post_move_edge_counts;

  // Lambda function to process a pair of groups contribution to edge entropy.
  // Needs to know what group is contributing the pair with moved_is_old_group.
  auto process_group_pair = [&](bool old_group_pair,
                                NodePtr neighbor_group,
                                double edge_count_pre) 
  {
    // How many edges does the node being moved contributed to total
    // edges between group node and neighbor group?
    double edges_from_node = node_edges[neighbor_group];

    // The old and new edge counts we need to compute entropy
    // If we're looking at the neighbor groups for the old group we need to 
    // subtract the edges the node contributed, otherwise we need to add.
    double edge_count_post = edge_count_pre + 
      (old_group_pair ? -1: 1) * edges_from_node;
    
    // Grab the degree of the group node for pre and post depending on which 
    // pair we're looking at.
    double moved_degree_pre = old_group_pair ? old_group_degree_pre: 
                                               new_group_degree_pre;

    double moved_degree_post = old_group_pair ? old_group_degree_post: 
                                                new_group_degree_post;
    
    // Neighbor node degree
    double neighbor_degree = neighbor_group->degree;

    
    // Calculate entropy contribution pre move 
    entropy_pre += edge_count_pre > 0 ?
      edge_count_pre* 
      log(edge_count_pre / (moved_degree_pre*neighbor_degree)):
      0; 
    
    // Calculate entropy contribution post move 
    entropy_post += edge_count_post > 0 ?
      edge_count_post * 
      log(edge_count_post / (moved_degree_post*neighbor_degree)):
      0;
  };

  // Loop through and calculate the entropy contribution for each pair of 
  // old group - neighbor
  for(auto con_group_it = old_group_edges.begin(); 
           con_group_it != old_group_edges.end(); 
           con_group_it++)
  {
    process_group_pair(true, con_group_it->first, con_group_it->second);
  }
  
  // Do the same for the new group - neighbor
  for(auto con_group_it = new_group_edges.begin(); 
           con_group_it != new_group_edges.end(); 
           con_group_it++)
  {
    process_group_pair(false, con_group_it->first, con_group_it->second);
  }

  // Now we move on to calculating the probability ratios for the node moving 
  // from old->new and then new->old assuming node was already in new.  
  double pre_move_prob = 0.0;
  double post_move_prob = 0.0;
  
  // Loop over all the node's connections to neighbor groups
  for(auto con_group_it = node_edges.begin(); 
           con_group_it != node_edges.end(); 
           con_group_it++)
  {
    NodePtr group_t = con_group_it->first;
    
    // Edges from node to group t
    double e_it = con_group_it->second;
    
    // Edges from new group to t pre move...
    double new_to_t_edges = old_group_edges[group_t];
 
    // Edges from old group to t post move...
    double old_to_t_edges = new_group_edges[group_t];
    
    pre_move_prob  += new_to_t_edges + EPS;
    post_move_prob += old_to_t_edges + EPS;
  }

  // Now we can clean up all the calculations into to entropy delta and the 
  // probability ratio for the moves and use those to calculate the acceptance 
  // probability for the proposed move.
  double entropy_delta = entropy_post - entropy_pre;
  double acceptance_prob = exp(BETA*entropy_delta) * 
                          (pre_move_prob/post_move_prob);
  
  return Proposal_Res(
    entropy_delta,
    acceptance_prob > 1 ? 1: acceptance_prob
  );
}

// =============================================================================
// Runs efficient MCMC sweep algorithm on desired node level
// =============================================================================
int SBM::mcmc_sweep(const int level, const bool variable_num_groups) 
{
  PROFILE_FUNCTION();
  int num_changes = 0;
  int group_level = level + 1;
  
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
    
    // Get a move proposal
    NodePtr proposed_new_group = propose_move(curr_node);

    // If the propsosed group is the nodes current group, we don't need to waste
    // time checking because decision will always result in same state.
    if(curr_node->parent->id == proposed_new_group->id) continue;
    
    // Calculate acceptance probability based on posterior changes
    Proposal_Res proposal_results = make_proposal_decision(
      curr_node,
      proposed_new_group
    );
    
    bool move_accepted = sampler.draw_unif() < proposal_results.prob_of_accept;

    if (move_accepted) 
    {   
      // Move the node
      set_node_parent(curr_node, proposed_new_group);
      
      num_changes++;
    }
    
    // Check if we're running sweep with variable group numbers. If we are, we
    // need to remove empty groups and add a new group as a potential for the
    // node to enter
    if (variable_num_groups) 
    {
      // Clean up empty groups
      clean_empty_groups();
      
      // Add a new group node for the current node type
      create_group_node(curr_node->type, group_level);
    }
  } // End loop over all nodes
  
  return num_changes;
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
  int sum_of_degrees = 0;
  
  // Grab pointer to current level and start loop
  LevelPtr node_level = get_level(level);
  for (auto node_it = node_level->begin(); 
            node_it != node_level->end(); 
            ++node_it)
  {
    int node_degree = node_it->second->degree;
    sum_of_degrees += node_degree;
    n_nodes_w_degree[node_degree]++;
  }
  
  //==========================================================================
  // Next, we calculate the summation of N_k*ln(K!) where K is degree size and
  // N_k is the number of nodes of that degree
  
  // Compute total number of edges and convert to double
  double n_total_edges = double(sum_of_degrees)/2.0;


  // Calculate first component (sum of node degree counts portion)
  double degree_summation = 0.0;
  for (auto degree_count =  n_nodes_w_degree.begin(); 
            degree_count != n_nodes_w_degree.end(); 
            ++degree_count)
  {
    int k = degree_count->first;
    int num_nodes = degree_count->second;
    degree_summation += num_nodes * log_factorial(k);
  }
  
  //============================================================================
  // Last, we calculate the summation of e_rs*ln(e_rs/e_r*e_s)/2 where e_rs is
  // number of connections between groups r and s and e_r is the total number of
  // edges for group r.

  // Grab all group nodes
  auto group_level = get_level(level + 1);
  
  double edge_entropy = 0.0;

  // Full loop over all group nodes
  for (auto group_r_it = group_level->begin();
       group_r_it != group_level->end();
       group_r_it++)
  {
    NodePtr group_r = group_r_it->second;
    // Gather all of group r's connections to our level
    auto group_r_edge_counts = group_r->gather_connections_to_level(level + 1);

    // Now loop over all the nodes connected to group r
    for (auto group_s_it = group_r_edge_counts.begin();
         group_s_it != group_r_edge_counts.end();
         group_s_it++)
    {
      // Grab total number of connections between r and s
      double e_rs = group_s_it->second;

      // Compute this iteration's controbution to sum
      edge_entropy += e_rs * log(e_rs / double(group_r->degree * group_s_it->first->degree));
    } // end group s loop
  }   // end group r loop

  // Add three components together to return
  return -1 * (n_total_edges + degree_summation + edge_entropy / 2);
}

// =============================================================================
// Merge two groups, placing all nodes that were under group_b under group_a and 
// deleting group_a from model.
// =============================================================================
void SBM::merge_groups(NodePtr group_a, NodePtr group_b)
{
  PROFILE_FUNCTION();
  // Place all the members of group b under group a
  auto children_to_move = group_b->children;

  for (NodePtr member_node : children_to_move)
  {
    set_node_parent(member_node, group_a);
  }
}  

// =============================================================================
// Merge groups at a given level based on the best probability of doing so
// =============================================================================
Merge_Step SBM::agglomerative_merge(const int group_level,
                                    const int num_merges_to_make)
{
  PROFILE_FUNCTION();
  // Quick check to make sure reasonable request
  if (num_merges_to_make == 0)
    throw "Zero merges requested.";

  // Level that the group metagroups will sit at
  int meta_level = group_level + 1;

  // Build a single meta-group for each node at desired level
  give_every_node_at_level_own_group(group_level);

  // Grab all the groups we're looking to merge
  LevelPtr all_groups = get_level(group_level);

  // Gather how many groups of each type we have
  std::map<int, int> n_groups_of_type;
  for (auto group_it = all_groups->begin(); group_it != all_groups->end();
       group_it++)
  {
    n_groups_of_type[group_it->second->type]++;
  }

  // Build vectors for recording merges
  std::vector<NodePtr> from_groups;
  std::vector<NodePtr> to_groups;
  std::vector<double> move_delta;

  const int size_to_return = N_CHECKS_PER_GROUP * all_groups->size();
  from_groups.reserve(size_to_return);
  to_groups.reserve(size_to_return);
  move_delta.reserve(size_to_return);

  // Make sure doing a merge makes sense by checking we have enough groups
  // of every type
  for (auto node_type_it = n_groups_of_type.begin();
       node_type_it != n_groups_of_type.end(); node_type_it++)
  {
    if (node_type_it->second < 2)
    {
      throw "To few groups to perform merge.";
    }
  }

  // Loop over each group and find best merge option
  for (auto group_it = all_groups->begin(); group_it != all_groups->end();
       group_it++)
  {
    NodePtr curr_group = group_it->second;

    std::vector<NodePtr> metagroups_to_search;

    // If we're running algorithm in greedy mode we should just
    // add every possible group to the groups-to-search list
    if (GREEDY)
    {
      // Get a list of all the potential merges for group
      metagroups_to_search =
          get_nodes_of_type_at_level(curr_group->type, meta_level);
    }
    else
    {
      metagroups_to_search.reserve(N_CHECKS_PER_GROUP);
      // Otherwise, we should sample a given number of groups to check
      for (int i = 0; i < N_CHECKS_PER_GROUP; i++)
      {
        // Sample a group from potential groups
        metagroups_to_search.push_back(propose_move(curr_group));
      }
    }

    // Now that we have gathered all the merges to check, we can loop
    // through them and check entropy changes
    for (NodePtr metagroup : metagroups_to_search)
    {
      // Get group that the metagroup belongs to
      NodePtr merge_group = *((metagroup->children).begin());

      // Skip group if it is the current group for this node
      if (merge_group->id == curr_group->id)
        continue;

      // Calculate entropy delta for move
      double entropy_delta = make_proposal_decision(
                                 curr_group,
                                 metagroup)
                                 .entropy_delta;

      from_groups.push_back(curr_group);
      to_groups.push_back(merge_group);
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

    // Get groups that are being merged (culled)
    NodePtr from_group = from_groups[merge_index];
    NodePtr to_group = to_groups[merge_index];

    // Make sure we haven't already merged the culled group
    bool from_still_exists =
        merges_made.find(from_group->id) == merges_made.end();

    // Also make sure that we haven't removed the group we're trying to
    // merge into
    bool to_still_exists = merges_made.find(to_group->id) == merges_made.end();

    if (from_still_exists & to_still_exists)
    {
      // Insert new culled group into set
      merges_made.insert(from_group->id);

      // Merge the best group pair
      merge_groups(to_group, from_group);

      // Record pair for results
      results.from_node.push_back(from_group->id);
      results.to_node.push_back(to_group->id);
    }

    // Remove the last index from our queue and go again
    best_moves.pop();

    // Update termination checking conditions
    more_merges_needed = merges_made.size() < num_merges_to_make;
    queue_not_empty = best_moves.size() != 0;
  }

  // Erase the empty groups and metagroups
  auto removed_groups = clean_empty_groups();

  // // Force regathering of edge counts now we're removed groups
  // // go through all edge counts and remove entries pertaining to 
  // // any of the just deleted groups
  // auto level_counts = get_edge_counts(group_level).counts;

  // for (auto edge_it = level_counts->begin();
  //           edge_it != level_counts->end();
  //           edge_it++)
  // {
  //   NodePtr group_r = (edge_it->first).first;
  //   NodePtr group_s = (edge_it->first).second;

  //   auto find_in_removed = [removed_groups](NodePtr group) {
  //     return std::find(
  //                removed_groups.begin(),
  //                removed_groups.end(),
  //                group) == removed_groups.end();
  //   };

  //   bool contains_removed_group = find_in_removed(group_r) | find_in_removed(group_s);

  //   if (contains_removed_group) level_counts->erase(edge_it);
  // }

  // Return the entropy for new model and merges done
  results.entropy = compute_entropy(group_level - 1);

  return results;
}

// =============================================================================
// Run mcmc chain initialization by finding best organization
// of B' groups for all B from B = N to B = 1.
// =============================================================================
std::vector<Merge_Step> SBM::collapse_groups(const int node_level,
                                             const int num_mcmc_steps,
                                             const int desired_num_groups)
{
  PROFILE_FUNCTION();
  int group_level = node_level + 1;

  // Start by giving every node at the desired level its own group
  give_every_node_at_level_own_group(node_level);

  // Get starting number of clusters
  int B_start = get_level(group_level)->size();

  // Now clean up the empty groups
  // (if there were previously group nodes for the level)
  clean_empty_groups();

  int num_steps = desired_num_groups == -1 
    ? B_start - node_type_counts.size()
    : desired_num_groups;

  // Setup vector to hold all merge step results
  std::vector<Merge_Step> step_results;
  step_results.reserve(num_steps);

  int i;
  for (i = 0; i < num_steps; i++)
  {
    
    // Get the current number of groups we have
    int curr_num_groups = get_level(group_level)->size();

    // Decide how many merges we should do. 
    int num_merges = std::max(
        int(curr_num_groups - (curr_num_groups / SIGMA)),
        1);

    // Make sure we don't overstep the goal number of groups
    if ((curr_num_groups - num_merges) < desired_num_groups)
    {
      num_merges = curr_num_groups - desired_num_groups;
    }

    Merge_Step merge_results;

    // Attempt merge step
    try
    {
      // Perform next best merge and record results
      merge_results = agglomerative_merge(
          group_level,
          num_merges);
    }
    catch (...)
    {
      // We reached the collapsibility limit of our network so we break early
      break;
    }

  

    if (num_mcmc_steps != 0)
    {
      // Let model equilibriate with new group layout...
      for (int j = 0; j < num_mcmc_steps; j++)
      {
        mcmc_sweep(
            node_level,
            false);
      }
      // Update the step entropy results with new equilibriated model
      merge_results.entropy = compute_entropy(node_level);
    }

    // Dump state into step results
    merge_results.state = get_state();

    // Gather info for return
    step_results.push_back(merge_results);
  }

  

  return step_results;
}
