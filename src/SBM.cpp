#include "SBM.h"

// =============================================================================
// Grab reference to a desired level map. If level doesn't exist yet, it will be
// created
// =============================================================================
LevelPtr SBM::get_level(const int level)
{
  PROFILE_FUNCTION();
  // Grab level for block node
  LevelMap::iterator block_level = nodes.find(level);

  // Is this a new level?
  bool level_doesnt_exist = block_level == nodes.end();

  if (level_doesnt_exist) {
    // Add a new node level
    nodes.emplace(level, std::make_shared<NodeLevel>());

    // 'find' that new level
    block_level = nodes.find(level);
  }

  return block_level->second;
}

// Const version that doesn't append level
LevelPtr SBM::get_level(const int level) const
{
  PROFILE_FUNCTION();
  const auto level_loc = nodes.find(level);

  if (level_loc == nodes.end()) {
    RANGE_ERROR("No nodes/ blocks at level " + std::to_string(level));
  }
  else {
    return level_loc->second;
  }
}

// =============================================================================
// Find and return a node by its id
// =============================================================================
NodePtr SBM::get_node_by_id(const std::string& id,
                            const int          level) const
{
  PROFILE_FUNCTION();
  const auto node_level = get_level(level);
  const auto node_loc   = node_level->find(id);

  if (node_loc == node_level->end()) {
    RANGE_ERROR("Could not find node " + id + " in network");
  }
  else {
    return node_loc->second;
  }
}

// =============================================================================
// Adds a node with an id and type to network
// =============================================================================
NodePtr SBM::add_node(const std::string& id,
                      const std::string& type,
                      const int          level)
{
  PROFILE_FUNCTION();
  // Grab level
  LevelPtr node_level = get_level(level);

  const auto assign_result = node_level->emplace(id, std::make_shared<Node>(id, level, type));

  const bool node_is_new = assign_result.second;
  if (node_is_new) {
    // Add this node to node counting map
    node_type_counts[type][level]++;
  }

  return assign_result.first->second;
};

// =============================================================================
// Creates a new block node and add it to its neccesary level
// =============================================================================
NodePtr SBM::create_block_node(const std::string& type, const int level)
{
  PROFILE_FUNCTION();

  // Make sure requested level is not 0
  if (level == 0) {
    LOGIC_ERROR("Can't create block node at first level");
  }

  std::string block_id = type + "-" + std::to_string(level) + "_" + std::to_string(id_counter++);

  // Create new block id and iterate id counter forward
  return add_node(block_id, type, level);
};

// =============================================================================
// Return nodes of a desired type from level.
// =============================================================================
NodeVec SBM::get_nodes_of_type_at_level(const std::string& type, const int level)
{
  PROFILE_FUNCTION();

  // Grab desired level reference
  LevelPtr node_level = get_level(level);

  // Make sure level has nodes before looping through it
  if (node_level->size() == 0) {

    RANGE_ERROR("Requested level " + std::to_string(level) + " is empty.");
  }

  // Where we will store all the nodes found from level
  NodeVec nodes_to_return;
  nodes_to_return.reserve(node_level->size());

  // Loop through every node belonging to the desired level
  for (const auto& node : *node_level) {

    // Decide to keep the node or not based on if it matches or doesn't and our
    // keeping preferance
    if (node.second->type == type) {
      // ...Place it in returning list
      nodes_to_return.push_back(node.second);
    }
  }

  return nodes_to_return;
}

// =============================================================================
// Adds a edge between two nodes based on their ids
// =============================================================================
void SBM::add_edge(const std::string& id_a, const std::string& id_b)
{
  PROFILE_FUNCTION();
  const NodePtr node_a = get_node_by_id(id_a);
  const NodePtr node_b = get_node_by_id(id_b);

  // Check if we have an explicite list of allowed edge patterns or if we should
  // add this edge as a possible pair.
  // If the user has specified allowed edges explicitely, make sure that this edge follows protocol
  if (specified_allowed_edges) {
    const bool a_to_b_bad = !(edge_type_pairs.at(node_a->type).count(node_b->type));
    const bool b_to_a_bad = !(edge_type_pairs.at(node_b->type).count(node_a->type));

    if (a_to_b_bad | b_to_a_bad) {
      LOGIC_ERROR("Edge of " + id_a + " - " + id_b + " does not fit allowed specified edge_types type combos.");
    }
  }
  else {
    add_edge_type(edge_type_pairs, node_a->type, node_b->type);
  }

  Node::connect_nodes(node_a, node_b);   // Connect nodes to eachother
  edges.push_back(Edge(node_a, node_b)); // Add edge to edge tracking list
};

// Vectorized version of add edge types for when a whole set is passed at once
void SBM::add_edge_types(const std::vector<std::string>& from_types, const std::vector<std::string>& to_types)
{
  // Clear old allowed pairs (if they exist)
  edge_type_pairs.clear();

  // Add pairs to network map of allowed pairs
  const int num_pairs = from_types.size();
  for (int i = 0; i < num_pairs; i++) {
    add_edge_type(edge_type_pairs, from_types[i], to_types[i]);
  }

  // Let object know that we're working with specified types now.
  specified_allowed_edges = true;
}

// =============================================================================
// Adds a desired number of blocks and randomly assigns them for a given level
// num_blocks = -1 means every node gets their own block
// =============================================================================
void SBM::initialize_blocks(const int level, const int num_blocks)
{
  PROFILE_FUNCTION();

  // Grab all the nodes for the desired level
  const LevelPtr nodes              = get_level(level);
  const int      num_nodes_in_level = nodes->size();

  // Make sure level has nodes before looping through it
  if (num_nodes_in_level == 0) {
    RANGE_ERROR("Requested level (" + std::to_string(level) + ") is empty.");
  }

  // Figure out how we're making blocks, is it one block per node or a set number
  // of blocks total?
  const bool one_block_per_node = num_blocks == -1;

  // Make a map that gives us type -> array of new blocks
  std::map<std::string, NodeVec> type_to_blocks;

  // If we're randomly distributing nodes, we'll use this map to sample a random
  // block for a given node by its type
  if (!one_block_per_node) {
    for (const auto& type : node_type_counts) {
      int num_blocks_to_build = num_blocks;

      // Check to make sure there are enough nodes of this type at the desired level to fill them with this many blocks
      if(type.second.at(level) < num_blocks){
        num_blocks_to_build = type.second.at(level);
        WARN_ABOUT("Fewer nodes of type " + type.first + " at level " + std::to_string(level) + "(" + std::to_string(num_blocks_to_build) + ") than requested number of groups (" + std::to_string(num_blocks) + ")");
      }
      
      // Reserve proper number of slots for new blocks
      type_to_blocks[type.first].reserve(num_blocks_to_build);

      // Buid new blocks to fill those slots
      for (int i = 0; i < num_blocks_to_build; i++) {
        // build a block node at the next level
        type_to_blocks[type.first].push_back(create_block_node(type.first, level + 1));
      }
    }
  }

  // Loop through every node in the level and assign it its new parent
  // Loop through each of the nodes,
  for (const auto& id_node_pair : *nodes) {
    const NodePtr node = id_node_pair.second;

    // build a block node at the next level
    NodePtr new_block;
    if (one_block_per_node) {
      // We either build a new block for node if we're giving each node a block
      new_block = create_block_node(node->type, level + 1);
    }
    else {
      // or sample new block from available list of blocks for this type
      new_block = sampler.sample(type_to_blocks[node->type]);
    }

    // assign that block node to the node
    node->set_parent(new_block);
  }

  // Clean up any straggling blocks from before initialization
  clean_empty_blocks();
}

// =============================================================================
// Scan through entire Network and remove all block nodes that have no children.
// Returns the number removed
// =============================================================================
void SBM::clean_empty_blocks(const int start_level)
{
  PROFILE_FUNCTION();
  int num_levels    = nodes.size();
  int total_deleted = 0;

  // Scan through all levels up to final
  for (int level = start_level; level < num_levels; ++level) {
    // Grab desired level
    LevelPtr block_level = get_level(level);

    // Create a vector to store block ids that we want to delete
    std::queue<std::string> blocks_to_delete;

    // Loop through every node at level
    for (const auto& block : *block_level) {

      // If there are no children for the current block
      if (block.second->children.size() == 0) {
        // Add current block to the removal list
        blocks_to_delete.push(block.second->id);

        // Remove block from children of its parent (if it has one)
        if (block.second->parent) {
          const NodePtr parent_block = block.second->parent;
          parent_block->children.erase(block.second);
        }

        // Remove nodes contribution to node counts map
        node_type_counts[block.second->type][level]--;
      }
    }

    // Remove all the blocks in the removal list
    while (!(blocks_to_delete.empty())) {
      block_level->erase(blocks_to_delete.front());

      // Remove reference from queue
      blocks_to_delete.pop();

      // Increment total blocks deleted counter
      total_deleted++;
    }
  }
}

// =============================================================================
// Export current state of nodes in model
// =============================================================================
State_Dump SBM::get_state() const
{
  PROFILE_FUNCTION();
  // Initialize the return struct
  State_Dump state;

  // Keep track of how many nodes we've seen so we can preallocate vector sizes
  int n_nodes_seen = 0;

  for (const auto& level : nodes) {

    // Add level's nodes to current total
    n_nodes_seen += level.second->size();

    // Update sizes of the state vectors
    state.id.reserve(n_nodes_seen);
    state.level.reserve(n_nodes_seen);
    state.parent.reserve(n_nodes_seen);
    state.type.reserve(n_nodes_seen);

    // Loop through each node in level
    for (const auto& node : *level.second) {

      // Dump all its desired info into its element in the state vectors
      state.id.push_back(node.second->id);
      state.level.push_back(level.first);
      state.type.push_back(node.second->type);

      // Record parent if node has one
      state.parent.push_back(node.second->parent ? node.second->parent->id : "none");

    } // End node loop
  }   // End level loop

  return state;
}

// =============================================================================
// Load current state of nodes in model from state dump given SBM::get_state()
// =============================================================================
void SBM::set_state(const std::vector<std::string>& id,
                    const std::vector<std::string>& parent,
                    const std::vector<int>&         level,
                    const std::vector<std::string>& type)
{
  PROFILE_FUNCTION();

  const int n = id.size();

  for (int i = 0; i < n; i++) {
    const std::string node_type    = type[i];
    const std::string child_id     = id[i];
    const int         child_level  = level[i];
    const std::string parent_id    = parent[i];
    const int         parent_level = child_level + 1;

    auto aquire_node = [node_type, this](const std::string& node_id, const int node_level) {
      LevelPtr nodes_at_level = get_level(node_level);

      // Attempt to find the node in the network
      auto node_loc = nodes_at_level->find(node_id);

      if (node_loc == nodes_at_level->end()) {
        return add_node(node_id, node_type, node_level);
      }
      else {
        return node_loc->second;
      }
    };

    // "none" indicates the highest level has been reached
    if (parent_id == "none") {
      continue;
    }

    // Attempt to find the parent node in the network
    // Next grab the child node (this one should exist...)
    // Assign the parent node to the child node
    aquire_node(child_id, child_level)->set_parent(aquire_node(parent_id, parent_level));
  }

  // Now clean up any potentially childless nodes that got kicked
  // out by this process
  clean_empty_blocks();
}

// Gathers counts of edges between all pairs of connected blocks in network
BlockEdgeCounts SBM::get_block_edge_counts(const int level) const
{
  BlockEdgeCounts block_counts;

  // Make sure we have blocks at the level asked for before proceeding
  if (nodes.count(level) == 0) {
    RANGE_ERROR("Model has no blocks at level " + std::to_string(level));
  }

  // Loop through edges and gather at desired level
  for (auto& edge : edges) {
    block_counts[edge.at_level(level)]++;
  }

  return block_counts;
}

NodeEdgeMap SBM::get_node_to_block_edge_counts(const std::string& id,
                                               const int         node_level,
                                               const int         connections_level) const
{
  // Get edges to desired level
  return get_node_by_id(id, node_level)->gather_edges_to_level(connections_level);
}

// =============================================================================
// Propose a potential block move for a node.
// =============================================================================
NodePtr SBM::propose_move(const Node& node, const double& eps)
{
  PROFILE_FUNCTION();

  const int block_level = node.level + 1;

  // Grab a list of all the blocks that the node could join
  const NodeVec potential_blocks = get_nodes_of_type_at_level(node.type, block_level);

  // Sample a random neighbor of node
  const NodePtr rand_neighbor = sampler.sample(node.edges)->get_parent_at_level(node.level);

  // Get number total number edges for neighbor's block
  const int neighbor_block_degree = rand_neighbor->parent->degree;

  // Decide if we are going to choose a random block for our node
  const double ergo_amnt            = eps * potential_blocks.size();
  const double prob_of_random_block = ergo_amnt / (neighbor_block_degree + ergo_amnt);

  // Decide where we will get new block from and draw from potential candidates
  return sampler.draw_unif() < prob_of_random_block ? sampler.sample(potential_blocks)
                                                    : sampler.sample(rand_neighbor->get_edges_of_type(node.type, block_level));
}

inline void print_node_info(const Node& node)
{
  OUT_MSG << "\nID:" << node.id << ", "
          << "Level:" << node.level << ", "
          << "N-Children:" << node.children.size() << ", "
          << "Parent:" << node.parent->id << std::endl;
}

NodeVec SBM::propose_moves(const Node& node,
                           const int     num_moves,
                           const double&  eps)
{
  NodeVec proposed_moves;
  proposed_moves.reserve(num_moves);

  const int block_level = node.level + 1;

  // Grab a list of all the blocks that the node could join
  const NodeVec potential_blocks = get_nodes_of_type_at_level(node.type, block_level);

  for (int i = 0; i < num_moves; i++) {
    // Sample a random neighbor of node

    if (node.degree == 0) {
      print_node_info(node);
      LOGIC_ERROR("Trying to sample edges from " + node.id + " but no edges were found");
    }
    const NodePtr rand_neighbor = sampler.sample(node.edges)->get_parent_at_level(node.level);

    // Get number total number edges for neighbor's block
    const int neighbor_block_degree = rand_neighbor->parent->degree;

    // Decide if we are going to choose a random block for our node
    const double ergo_amnt            = eps * potential_blocks.size();
    const double prob_of_random_block = ergo_amnt / (neighbor_block_degree + ergo_amnt);

    // Decide where we will get new block from and draw from potential candidates
    const NodePtr chosen_move = sampler.draw_unif() < prob_of_random_block
        ? sampler.sample(potential_blocks)
        : sampler.sample(rand_neighbor->get_edges_of_type(node.type, block_level));

    proposed_moves.push_back(chosen_move);
  }

  return proposed_moves;
}

// =============================================================================
// Make a decision on the proposed new block for node
// =============================================================================
Proposal_Res SBM::make_proposal_decision(const Node& node,
                                         const Node& new_block,
                                         const double&  eps)
{
  PROFILE_FUNCTION();

  const Node old_block = *(node.parent); // Reference to old block that would be swapped for new_block
  // Make sure we're actually doing something
  if (old_block == new_block) {
    return Proposal_Res(0.0, 0.0);
  }

  const double node_degree = node.degree;
  const double block_level = new_block.level; // The level that this proposal is taking place on

  // Setup a struct to hold all the info we need about a given pair
  struct Node_Move_Cons {
    int old_to_neighbor  = 0;
    int new_to_neighbor  = 0;
    int node_to_neighbor = 0;
  };
  std::map<NodePtr, Node_Move_Cons> move_edge_counts;

  // Gather the node to edge counts together to one main map
  int node_to_old_block = 0;
  int node_to_new_block = 0;

  for (const auto& edge : old_block.edges) {
    move_edge_counts[edge->get_parent_at_level(block_level)].old_to_neighbor++;
  }

  for (const auto& edge : new_block.edges) {
    move_edge_counts[edge->get_parent_at_level(block_level)].new_to_neighbor++;
  }

  for (const auto& edge : node.edges) {
    const NodePtr edge_block = edge->get_parent_at_level(block_level);

    if (old_block == *edge_block) {
      node_to_old_block++;
    }
    else if (new_block == *edge_block) {
      node_to_new_block++;
    }

    move_edge_counts[edge_block].node_to_neighbor++;
  }

  // How many possible neighbor blocks are there?
  // Loop over all the possible neighbor node types for this node and add up.
  int n_possible_neighbors = 0;

  const auto possible_neighbor_types = edge_type_pairs.at(node.type);
  for (const auto& neighbor_type : possible_neighbor_types) {
    n_possible_neighbors += node_type_counts[neighbor_type][block_level];
  }

  // These are constants for edge connections that are used in entropy calc
  const int node_to_old_new_delta = node_to_old_block - node_to_new_block;
  const int pre_old_degree        = old_block.degree;
  const int post_old_degree       = pre_old_degree - node_degree;
  const int pre_new_degree        = new_block.degree;
  const int post_new_degree       = pre_new_degree + node_degree;

  // These will get summed into as we loop over all the neighbor blocks
  double entropy_delta  = 0;
  double pre_move_prob  = 0;
  double post_move_prob = 0;

  for (const auto& move_edges : move_edge_counts) {
    const Node&        neighbor = *move_edges.first;
    const Node_Move_Cons& pre      = move_edges.second;

    // Degree of neighbor group before move
    const int pre_neighbor_degree = neighbor.degree;

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
                            const double& eps,
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

  // Check if we have any blocks ready in the network...
  const bool no_blocks_present = get_level(block_level)->size() == 0;

  if (no_blocks_present) {
    initialize_blocks(level);

    if (verbose) {
      WARN_ABOUT("No blocks present. Initializing one block per node.");
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
  // Grab level map
  const LevelPtr node_map = get_level(level);
  NodeVec        nodes_to_sweep;
  nodes_to_sweep.reserve(node_map->size());
  for (const auto& node : *node_map) {
    nodes_to_sweep.push_back(node.second);
  }

  for (int i = 0; i < num_sweeps; i++) {
    // Book keeper variables for this sweeps stats
    int    num_nodes_moved = 0;
    double entropy_delta   = 0;

    // Shuffle order order of nodes to be run through for sweep
    std::shuffle(nodes_to_sweep.begin(), nodes_to_sweep.end(), sampler.generator);

    // Setup container to track what pairs need to be updated for sweep
    std::set<std::string> pair_moves;

    int steps_taken = 0;
    // Loop through each node
    for (const NodePtr& curr_node : nodes_to_sweep) {
      // Check if we're running sweep with variable block numbers. If we are, we
      // need to make sure we don't have any extra unoccupied blocks floating around,
      // then we need to add a new block as a potential for the node to enter
      if (variable_num_blocks) {
        clean_empty_blocks();
        create_block_node(curr_node->type, block_level);
      }

      // Get a move proposal
      const NodePtr proposed_new_block = propose_move(*curr_node, eps);

      // If the proposed block is the nodes current block, we don't need to waste
      // time checking because decision will always result in same state.
      if (curr_node->parent == proposed_new_block) {
        continue;
      }

      if (verbose) {
        OUT_MSG << i
                << "," << curr_node->id
                << "," << (curr_node->parent)->id
                << "," << proposed_new_block->id
                << ",";
      }
      // Calculate acceptance probability based on posterior changes
      Proposal_Res proposal_results = make_proposal_decision(*curr_node, *proposed_new_block, eps);

      // Make movement decision
      const bool move_accepted = proposal_results.prob_of_accept > sampler.draw_unif();

      if (verbose) {
        OUT_MSG << proposal_results.entropy_delta << "," << proposal_results.prob_of_accept << ","
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
          Block_Consensus::update_changed_pairs(curr_node->id,
                                                old_block->children,
                                                proposed_new_block->children,
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

    // Update the concensus pairs map with results if needed.
    if (track_pairs) {
      results.block_consensus.update_pair_tracking_map(pair_moves);
    }
    ALLOW_USER_BREAKOUT; // Let R used break out of loop if need be
  }                      // End multi-sweep loop

  return results;
}

// =============================================================================
// Compute microcononical entropy of current model state
// Note that this is currently only the degree corrected entropy
// =============================================================================
double SBM::get_entropy(const int level) const
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

  if (block_level->size() == 0) {
    LOGIC_ERROR("Network has not had block structure initialized.");
  }

  // Now calculate the edge entropy betweeen nodes.
  double edge_entropy = 0.0;

  // Gather block-to-block edge counts
  const std::map<Edge, int> block_edges = get_block_edge_counts(level + 1);
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
void SBM::merge_blocks(const NodePtr& absorbing_block, const NodePtr& absorbed_block)
{
  PROFILE_FUNCTION();
  // Place all the members of block b under block a
  const NodeSet children_to_move = absorbed_block->children;
  for (const NodePtr& child_node : children_to_move) {
    child_node->set_parent(absorbing_block);
  }

  // Remove node and all its parents from their respective level blocks
  NodePtr current_node = absorbed_block;
  while (current_node) {
    const std::string node_type  = current_node->type;
    const int         node_level = current_node->level;
    const std::string node_id    = current_node->id;
    current_node                 = current_node->parent;

    // Remove node's contribution to node counts map
    node_type_counts[node_type][node_level]--;

    // Delete the now absorbed block from level map
    get_level(node_level)->erase(node_id);
  }
}

// =============================================================================
// Merge blocks at a given level based on the best probability of doing so
// =============================================================================
Merge_Step SBM::agglomerative_merge(const int    block_level,
                                    const int    num_merges_to_make,
                                    const int    num_checks_per_block,
                                    const double& eps)
{
  PROFILE_FUNCTION();
  // Quick check to make sure reasonable request
  if (num_merges_to_make <= 0) {
    LOGIC_ERROR("Zero merges requested.");
  }

  // Level that the block metablocks will sit at
  const int meta_level = block_level + 1;

  // Build a single meta-block for each block
  initialize_blocks(block_level);

  // Grab all the blocks we're looking to merge
  const LevelPtr all_blocks = get_level(block_level);

  // Priority queue to find best moves
  std::priority_queue<std::pair<double, std::pair<NodePtr, NodePtr>>> best_moves_q;

  // Set to keep track of what pairs of nodes we have checked already so we dont double check
  std::set<std::string> checked_pairs;

  // Make sure doing a merge makes sense by checking we have enough blocks of every type
  for (const auto& type_count : node_type_counts) {
    if (type_count.second.at(block_level) < 2) {
      LOGIC_ERROR("To few blocks to perform merge.");
    }
  }

  // Loop over each block and find best merge option
  for (const auto& block : *all_blocks) {

    // if(block.second->children.size() == 0){
    //   // We may have old block nodes floating around that have yet to be cleaned up
    //   // if we encounter these, just ignore them. This is cheaper than running
    //   // clean_empty_blocks() every time.
    //   break;
    // }

    NodeVec metablocks_to_search;

    if (block.second->children.size() == 0) {
      LOGIC_ERROR("Block " + block.second->id + " with no childen found in search");
    }

    // No point in running M checks if there are < M blocks left.
    const bool less_blocks_than_checks = node_type_counts.at(block.second->type).at(meta_level) <= num_checks_per_block;

    if (less_blocks_than_checks) {
      // Get a list of all the potential metablocks for block
      metablocks_to_search = get_nodes_of_type_at_level(block.second->type, meta_level);
    }
    else {
      // Otherwise, we should sample a given number of blocks to check
      metablocks_to_search = propose_moves(*(block.second), num_checks_per_block, eps);
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
      const bool unchecked_pair = checked_pairs.insert(make_pair_key(merge_block->id, block.second->id)).second;

      if (unchecked_pair) {

        const NodePtr& block_a = merge_block;
        const NodePtr& block_b = block.second;

        // Build a map of neighbor to pair of both groups connections to that neighbor.
        std::map<NodePtr, std::pair<int, int>> pair_counts_to_neighbor;

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
        best_moves_q.push(std::make_pair(-entropy_delta,
                                         std::make_pair(block.second,
                                                        merge_block)));
      }
    }
  }

  // Now we find the top merges...
  // Start by initializing a merge result struct
  Merge_Step results;

  // A set of the blocks that have been merged already this step and thus are off limits
  std::set<NodePtr> merged_blocks;
  int               num_merges_made = 0;

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
CollapseResults SBM::collapse_blocks(const int    node_level,
                                     const int    num_mcmc_steps,
                                     const int    desired_num_blocks,
                                     const int    num_checks_per_block,
                                     const double& sigma,
                                     const double& eps,
                                     const bool   report_all_steps)
{
  PROFILE_FUNCTION();
  const int block_level = node_level + 1;

  // Start by giving every node at the desired level its own block and every
  // one of those blocks its own metablock
  initialize_blocks(node_level);

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
  CollapseResults step_results;
  step_results.reserve(report_all_steps ? num_steps : 1);

  // Counter to calculate the total entropy delta of this collapse run. Only used when not reporting all results
  double total_entropy_delta = 0;

  while (curr_num_blocks > desired_num_blocks) {
    // Decide how many merges we should do. Make sure we don't overstep the goal
    // number of blocks and we need to remove at least 1 block
    const int delta_to_desired    = curr_num_blocks - desired_num_blocks;
    const int sigma_derived_delta = std::floor(curr_num_blocks - (curr_num_blocks / sigma));
    const int num_merges          = std::max(std::min(delta_to_desired, sigma_derived_delta), 1);

    // Perform next best merge and record results
    Merge_Step merge_results = agglomerative_merge(block_level, num_merges, num_checks_per_block, eps);

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
    step_results.at(0).entropy = initial_entropy + total_entropy_delta;
  }
  else {
    // Update the results entropy values with the true entropy rather than the delta provided.
    double current_entropy = initial_entropy;
    for (auto& step_result : step_results) {
      step_result.entropy = current_entropy + step_result.entropy_delta;
      current_entropy     = step_result.entropy;
    }
  }

  return step_results;
}

// =============================================================================
// Repeat the collapse_blocks method with a ranging number of desired blocks to
// collapse to and report just the final result for all
// =============================================================================
CollapseResults SBM::collapse_run(const int              node_level,
                                  const int              num_mcmc_steps,
                                  const int              num_checks_per_block,
                                  const double&           sigma,
                                  const double&           eps,
                                  const std::vector<int>& block_nums)
{
  CollapseResults run_results;
  run_results.reserve(block_nums.size());

  for (const int target_num : block_nums) {

    auto collapse_results = collapse_blocks(node_level,
                                            num_mcmc_steps,
                                            target_num,
                                            num_checks_per_block,
                                            sigma,
                                            eps,
                                            false);

    if (collapse_results.size() < 1) {
      LOGIC_ERROR("Collapse result has zero entries");
    }
    else {
      run_results.push_back(collapse_results.at(0));
    }
  }

  return run_results;
}
