#include "Network.h"

// =============================================================================
// Setup a new Node level
// =============================================================================
void Network::add_level(const int level)
{
  PROFILE_FUNCTION();
  // First, make sure level doesn't already exist
  if (nodes.find(level) == nodes.end()) {
    // Setup first level of the node map
    nodes.emplace(level, std::make_shared<NodeLevel>());
  }
}

// =============================================================================
// Grab reference to a desired level map. If level doesn't exist yet, it will be
// created
// =============================================================================
LevelPtr Network::get_level(const int level)
{
  PROFILE_FUNCTION();
  // Grab level for block node
  LevelMap::iterator block_level = nodes.find(level);

  // Is this a new level?
  bool level_doesnt_exist = block_level == nodes.end();

  if (level_doesnt_exist) {
    // Add a new node level
    add_level(level);

    // 'find' that new level
    block_level = nodes.find(level);
  }

  return block_level->second;
}

// =============================================================================
// Find and return a node by its id
// =============================================================================
NodePtr Network::get_node_by_id(const std::string& id,
                                const int          level)
{
  PROFILE_FUNCTION();
  try {
    // Attempt to find node on the 'node level' of the Network
    return nodes.at(level)->at(id);
  }
  catch (...) {
    // Throw informative error if it fails
    std::cerr << "Could not find requested node" << std::endl;
    throw "Could not find requested node";
  }
}

// =============================================================================
// Builds a block id from a scaffold for generated new blocks
// =============================================================================
string Network::build_block_id(const std::string& type,
                               const int          level,
                               const int          index)
{
  PROFILE_FUNCTION();
  return type + "-" + std::to_string(level) + "_" + std::to_string(index);
}

// =============================================================================
// Adds a node with an id and type to network
// =============================================================================
NodePtr Network::add_node(const std::string& id,
                          const std::string& type,
                          const int          level)
{
  PROFILE_FUNCTION();
  // Grab level
  LevelPtr node_level = get_level(level);

  // Check if we need to make the id or not
  string node_id = id == "new block" ? build_block_id(type, level, node_level->size()) : id;

  // Create node
  NodePtr new_node = std::make_shared<Node>(node_id, level, type);

  (*node_level)[node_id] = new_node;

  // Add this node to node counting map
  node_type_counts[type][level]++;

  return new_node;
};

// =============================================================================
// Creates a new block node and add it to its neccesary level
// =============================================================================
NodePtr Network::create_block_node(const std::string& type, const int level)
{
  PROFILE_FUNCTION();

  // Make sure requested level is not 0
  if (level == 0) {
    std::cerr << "Can't create block node at first level" << std::endl;
    throw "Can't create block node at first level";
  }

  // Initialize new node
  return add_node("new block", type, level);
};

// =============================================================================
// Return nodes of a desired type from level. If match_type = true then the
// nodes returned are of the same type as specified, otherwise the nodes
// returned are _not_ of the same type.
// =============================================================================
NodeVec Network::get_nodes_from_level(const std::string& type,
                                      const int          level,
                                      const bool         match_type)
{
  PROFILE_FUNCTION();
  // Grab desired level reference
  LevelPtr node_level = nodes.at(level);

  // Make sure level has nodes before looping through it
  if (node_level->size() == 0) {
    std::cerr << "Requested level " << level << " is empty of nodes of type " << type << " when "
              << (match_type ? "" : "not ") << "matching type" << std::endl;
    throw "Requested level is empty.";
  }

  // Where we will store all the nodes found from level
  NodeVec nodes_to_return;
  nodes_to_return.reserve(node_level->size());

  // Loop through every node belonging to the desired level
  for (const auto& node : *node_level) {

    // Decide to keep the node or not based on if it matches or doesn't and our
    // keeping preferance
    bool keep_node = match_type ? (node.second->type == type) : (node.second->type != type);

    if (keep_node) {
      // ...Place it in returning list
      nodes_to_return.push_back(node.second);
    }
  }

  return nodes_to_return;
}

// =============================================================================
// Return nodes of a desired type from level.
// =============================================================================
NodeVec Network::get_nodes_of_type_at_level(const std::string& type, const int level)
{
  PROFILE_FUNCTION();
  return get_nodes_from_level(type, level, true);
}

// =============================================================================
// Adds a edge between two nodes based on their references
// =============================================================================
void Network::add_edge(const NodePtr node1, const NodePtr node2)
{
  PROFILE_FUNCTION();

  // Check if we have an explicite list of allowed edge patterns or if we should
  // add this edge as a possible pair.
  if (!specified_allowed_edges) {
    add_edge_types(node1->type, node2->type);
  }

  Node::connect_nodes(node1, node2);   // Connect nodes to eachother
  edges.push_back(Edge(node1, node2)); // Add edge to edge tracking list
};

// =============================================================================
// Adds a edge between two nodes based on their ids
// =============================================================================
void Network::add_edge(const std::string& id_a, const std::string& id_b)
{
  PROFILE_FUNCTION();
  Network::add_edge(get_node_by_id(id_a), get_node_by_id(id_b));
};

// =============================================================================
// Add an alowed pairing of node types for edges
// =============================================================================
void Network::add_edge_types(const std::string& from_type, const std::string& to_type)
{
  edge_type_pairs[from_type].insert(to_type);
  edge_type_pairs[to_type].insert(from_type);
}

// Vectorized version of add edge types for when a whole set is passed at once
void Network::add_edge_types(const std::vector<std::string>& from_types, const std::vector<std::string>& to_types)
{
  // Clear old allowed pairs (if they exist)
  edge_type_pairs.clear();

  // Add pairs to network map of allowed pairs
  const int num_pairs = from_types.size();
  for (int i = 0; i < num_pairs; i++) {
    Network::add_edge_types(from_types[i], to_types[i]);
  }

  // Let object know that we're working with specified types now.
  specified_allowed_edges = true;
}

// =============================================================================
// Builds and assigns a block node for every node in a given level
// =============================================================================
void Network::give_every_node_at_level_own_block(const int level)
{
  initialize_blocks(-1, level);
}

// =============================================================================
// Adds a desired number of blocks and randomly assigns them for a given level
// num_blocks = -1 means every node gets their own block
// =============================================================================
void Network::initialize_blocks(const int num_blocks, const int level)
{
  PROFILE_FUNCTION();

  const int block_level = level + 1;

  // Clear all previous nodes in block level out
  get_level(block_level)->clear();

  // Grab all the nodes for the desired level
  LevelPtr node_level = nodes.at(level);

  const int num_nodes_in_level = node_level->size();

  // Setup a sampler
  Sampler block_sampler;

  // Make sure level has nodes before looping through it
  if (num_nodes_in_level == 0) {
    std::cerr << "Requested level is empty. (initialize_blocks())" << std::endl;
    throw "Requested level is empty.";
  }

  // Figure out how we're making blocks, is it one block per node or a set number
  // of blocks total?
  bool one_block_per_node = num_blocks == -1;

  // Make a map that gives us type -> array of new blocks
  std::map<std::string, NodeVec> type_to_blocks;

  // If we're randomly distributing nodes, we'll use this map to sample a random
  // block for a given node by its type
  if (!one_block_per_node) {
    for (const auto& type : node_type_counts) {
      // Reserve proper number of slots for new blocks
      type_to_blocks[type.first].reserve(num_blocks);

      // Buid new blocks to fill those slots
      for (int i = 0; i < num_blocks; i++) {
        // build a block node at the next level
        type_to_blocks[type.first].push_back(create_block_node(type.first, level + 1));
      }
    }
  }

  // Loop through every node in the level and assign it its new parent
  // Loop through each of the nodes,
  for (const auto& node : *node_level) {

    // build a block node at the next level
    // We either build a new block for node if we're giving each node a block
    // or sample new block from available list of blocks for this type
    NodePtr new_block = one_block_per_node
        ? create_block_node(node.second->type, level + 1)
        : block_sampler.sample(type_to_blocks[node.second->type]);

    // assign that block node to the node
    node.second->set_parent(new_block);
  }
}

// =============================================================================
// Grabs the first node found at a given level, used in testing.
// =============================================================================
NodePtr Network::get_node_from_level(const int level)
{
  PROFILE_FUNCTION();
  return nodes.at(level)->begin()->second;
}

// =============================================================================
// Scan through entire Network and remove all block nodes that have no children.
// Returns the number removed
// =============================================================================
NodeVec Network::clean_empty_blocks()
{
  PROFILE_FUNCTION();
  int num_levels    = nodes.size();
  int total_deleted = 0;

  NodeVec blocks_removed;

  // Scan through all levels up to final
  for (int level = 1; level < num_levels; ++level) {
    // Grab desired level
    LevelPtr block_level = nodes.at(level);

    // Create a vector to store block ids that we want to delete
    std::queue<string> blocks_to_delete;

    // Loop through every node at level
    for (const auto& block : *block_level) {

      // If there are no children for the current block
      if (block.second->children.size() == 0) {
        // Remove block from children of its parent (if it has one)
        if (block.second->parent) {
          block.second->parent->remove_child(block.second);
        }

        blocks_removed.push_back(block.second);

        // Add current block to the removal list
        blocks_to_delete.push(block.second->id);

        // Remove nodes contribution to node counts map
        node_type_counts[block.second->type][level]--;
      }
    }

    // Remove all the blocks in the removal list
    while (!blocks_to_delete.empty()) {
      block_level->erase(blocks_to_delete.front());

      // Remove reference from queue
      blocks_to_delete.pop();

      // Increment total blocks deleted counter
      total_deleted++;
    }
  }

  return blocks_removed;
}

// =============================================================================
// Export current state of nodes in model
// =============================================================================
State_Dump Network::get_state()
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
// Load current state of nodes in model from state dump given Network::get_state()
// =============================================================================
void Network::load_from_state(const State_Dump state)
{
  PROFILE_FUNCTION();

  const int n = state.parent.size();

  for (int i = 0; i < n; i++) {
    const std::string node_type    = state.type[i];
    const std::string child_id     = state.id[i];
    const int         child_level  = state.level[i];
    const std::string parent_id    = state.parent[i];
    const int         parent_level = child_level + 1;

    auto aquire_node = [node_type, this](string node_id, int node_level) {
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
BlockEdgeCounts Network::get_block_counts_at_level(const int level)
{
  BlockEdgeCounts block_counts;

  // Loop through edges and gather at desired level
  for (auto& edge : edges) {
    block_counts[edge.at_level(level)]++;
  }

  return block_counts;
}
