#pragma once

#include "Node.h"
#include "Sampler.h"
#include "unordered_map"
#include "vector_helpers.h"
#include <unordered_map>

// These are seperate and will change based on compiler environemnt and only apply to the
// constructor and externally callable methods.
using Input_String_Vec = std::vector<string>;
using Input_Int_Vec    = std::vector<int>;

using Node_UPtr_Vec = std::vector<Node_UPtr>;
using Type_Vec      = std::vector<std::vector<Node_UPtr>>;

template <typename T>
using String_Map = std::unordered_map<string, T>;

struct State_Dump {
  std::vector<string> ids;
  std::vector<string> types;
  std::vector<string> parents;
  std::vector<int> levels;
  State_Dump(const int size)
  {
    ids.reserve(size);
    types.reserve(size);
    parents.reserve(size);
    levels.reserve(size);
  }
  int size() const { return ids.size(); }
};

enum Partite_Structure {
  unipartite,             // One node type
  multipartite,           // Multiple node types, all possible edges between non_like types allowed
  multipartite_restricted // Multiple node types, only specific type combos allowed for edges
};

class SBM_Network {

  private:
  // Data
  std::vector<Type_Vec> nodes;
  std::vector<string> types;
  Int_Map<string> type_name_to_int;
  std::map<int, std::set<int>> connection_types;
  String_Map<Node*> id_to_node;
  Partite_Structure edge_types;

  Sampler random_sampler;
  int block_counter = 0; // Keeps track of how many block we've had

  // =========================================================================
  // Private helper methods
  // =========================================================================
  int get_type_index(const string name) const
  {
    const auto name_it = type_name_to_int.find(name);

    // If this is a previously unseen type, add new entry
    if (name_it == type_name_to_int.end())
      LOGIC_ERROR("Type " + name + " doesn't exist in network");

    return name_it->second;
  }

  void check_for_level(const int level) const
  {
    // Make sure we have the requested level
    if (level >= nodes.size())
      RANGE_ERROR("Node requested in level that does not exist");
  }

  void check_for_type(const int type_index) const
  {
    if (type_index >= num_types())
      RANGE_ERROR("Type " + as_str(type_index) + " does not exist in network.");
  }

  // Apply a lambda function over all nodes in network
  void for_all_nodes_at_level(const int level,
                              std::function<void(const Node_UPtr& node)> fn) const
  {
    check_for_level(level);
    for (const auto& nodes_vec : nodes.at(level)) {
      std::for_each(nodes_vec.begin(), nodes_vec.end(), fn);
    }
  }

  public:
  // =========================================================================
  // Constructors
  // =========================================================================
  SBM_Network(const std::vector<std::string>& node_types = { "node" },
              const int random_seed                      = 42)
      : random_sampler(random_seed)
      , types(node_types)
      , type_name_to_int(build_val_to_index_map(node_types))
  {
    build_level(); // Setup empty first level of nodes
  }

  SBM_Network(const Input_String_Vec& node_ids,
              const Input_String_Vec& node_types,
              const Input_String_Vec& edges_a,
              const Input_String_Vec& edges_b,
              const Input_String_Vec& all_types,
              const int random_seed                   = 42,
              const Input_String_Vec& allowed_edges_a = {},
              const Input_String_Vec& allowed_edges_b = {})
      : random_sampler(random_seed)
      , types(all_types)
      , type_name_to_int(build_val_to_index_map(all_types))
      , edge_types(all_types.size() == 1
                       ? unipartite
                       : allowed_edges_a.size() == 0
                           ? multipartite
                           : multipartite_restricted)
  {
    build_level(node_ids.size()); // Setup empty first level of nodes with conservative space reserving

    // Fill in map to get allowed connection types if they are provided
    if (edge_types == multipartite_restricted) {
      for (int i = 0; i < allowed_edges_a.size(); i++) {
        validate_edge(get_type_index(allowed_edges_a[i]),
                      get_type_index(allowed_edges_b[i]), true);
      }
    }

    // Add nodes to network
    for (int i = 0; i < node_ids.size(); i++) {
      add_node(node_ids[i], node_types[i]);
    }

    // Connect nodes with edges
    for (int i = 0; i < edges_a.size(); i++) {
      add_edge(edges_a[i], edges_b[i]);
    }
  }

  // =========================================================================
  // Information
  // =========================================================================
  int num_nodes() const
  {
    return total_num_elements(nodes);
  }

  int num_nodes_at_level(const int level) const
  {
    check_for_level(level);
    return total_num_elements(nodes.at(level));
  }

  int num_levels() const
  {
    return nodes.size();
  }

  int num_nodes_of_type(const int type_i, const int level = 0) const
  {
    check_for_level(level);
    check_for_type(type_i);
    return nodes.at(level).at(type_i).size();
  }

  int num_nodes_of_type(const string& type, const int level = 0) const
  {
    check_for_level(level);
    return nodes.at(level).at(get_type_index(type)).size();
  }

  int num_types() const
  {
    return types.size();
  }

  // Export current state of nodes in model
  State_Dump state() const
  {
    if (num_levels() == 1)
      LOGIC_ERROR("No state to export - Try adding blocks");

    // Initialize the return struct
    State_Dump state(num_nodes());

    // No need to record the last level's nodes as they are already included
    // in the previous node's parent slot
    for (int level = 0; level < num_levels() - 1; level++) {
      for_all_nodes_at_level(level, [&](const Node_UPtr& node) {
        state.ids.push_back(node->get_id());
        state.types.push_back(types[node->get_type()]);
        state.parents.push_back(node->get_parent_id());
        state.levels.push_back(level);
      });
    }

    return state;
  }

  // =========================================================================
  // Modification
  // =========================================================================
  Node* add_node(const std::string& id,
                 const std::string& type = "a",
                 const int level         = 0)
  {
    const int type_index = get_type_index(type);

    // Build new node pointer outside of vector first for ease of pointer retrieval
    auto new_node = Node_UPtr(new Node(id, level, type_index, num_types()));

    // Get raw pointer to node to return
    Node* node_ptr = new_node.get();

    // Place this node in the id-to-node map if its a data-level node
    if (level == 0)
      id_to_node.emplace(id, node_ptr);

    // Move node unique pointer into its type in map
    get_nodes_of_type(type_index, level).push_back(std::move(new_node));

    return node_ptr;
  }

  void add_edge(const string& node_a, const string& node_b)
  {
    Node* a = get_node_by_id(node_a);
    Node* b = get_node_by_id(node_b);

    validate_edge(a->get_type(), b->get_type());

    a->add_edge(b);
    b->add_edge(a);
  }

  void validate_edge(const int type_a, const int type_b, const bool loading = false)
  {
    if (edge_types == unipartite) {
      return; // Do nothing
    }

    if (loading || edge_types == multipartite) {
      // Load the type into connection types for later use
      connection_types[type_a].insert(type_b);
      connection_types[type_b].insert(type_a);
    } else {
      // If we're in a restricted multipartite network
      // make sure that this is an acceptable edgetype combo
      const bool edge_not_allowed = connection_types.at(type_a).count(type_b) == 0;

      if (edge_not_allowed)
        LOGIC_ERROR("Connection provided between nodes of types "
                    + types[type_a] + " & " + types[type_b]
                    + " which was not a specified combination in allowed edge types");
    }
  }

  void initialize_blocks(int num_blocks = -1)
  {
    const bool one_block_per_node = num_blocks == -1;
    const int block_level         = num_levels();
    const int child_level         = block_level - 1;

    // Build empty level
    build_level(one_block_per_node ? 0 : num_blocks);

    // Loop over all node types
    for (int type_i = 0; type_i < num_types(); type_i++) {

      Node_UPtr_Vec& nodes_of_type  = get_nodes_of_type(type_i, child_level);
      Node_UPtr_Vec& blocks_of_type = get_nodes_of_type(type_i, block_level);

      // If we're in the 1-block-per-node mode make sure we reflect that in reserved size
      if (one_block_per_node)
        num_blocks = nodes_of_type.size();

      if (num_blocks > nodes_of_type.size())
        LOGIC_ERROR("Can't initialize more blocks than there are nodes of a given type");

      // Reserve enough spaces for the blocks to be inserted
      blocks_of_type.reserve(num_blocks);

      for (int i = 0; i < num_blocks; i++) {
        // Build a new block node wrapped in smart pointer in it's type vector
        blocks_of_type.emplace_back(new Node(block_counter++, type_i, block_level, num_types()));
      }

      // Shuffle child nodes if we're randomly assigning blocks
      if (!one_block_per_node)
        random_sampler.shuffle(nodes_of_type);

      // Loop through now shuffled children nodes
      for (int i = 0; i < nodes_of_type.size(); i++) {
        // Add blocks one at a time, looping back after end to each node
        nodes_of_type[i]->set_parent(blocks_of_type[i % num_blocks].get());
      }
    }
  }

  void build_level(const int reserve_size = 0)
  {
    nodes.emplace_back(num_types());

    // If we were told to reserve a size for each type vec, do so.
    if (reserve_size > 0) {
      for (auto& type_vec : nodes.at(num_levels() - 1)) {
        type_vec.reserve(reserve_size);
      }
    }
  }

  void delete_block_level()
  {
    if (has_blocks()) {
      // Remove the last layer of nodes.
      nodes.pop_back();
    } else {
      LOGIC_ERROR("No block level to delete.");
    }
  }

  void delete_all_blocks()
  {
    while (has_blocks()) {
      delete_block_level();
    }
  }

  bool has_blocks() const
  {
    return num_levels() > 1;
  }

  // =============================================================================
  // Load current state of nodes in model from state dump given SBM::state()
  // =============================================================================
  void update_state(const std::vector<string>& ids,
                    const std::vector<string>& parents,
                    const std::vector<int>& levels,
                    const std::vector<string>& types)
  {
    delete_all_blocks(); // Remove all block levels
    build_level();       // Add an empty block level to fill in

    // Make a copy of the id_to_node map (We will later overwrite it)
    String_Map<Node*> node_by_id = id_to_node;

    // Setup map to get blocks/parents by id
    String_Map<Node*> block_by_id;

    // Loop through entries of the state dump
    int last_level = 0;
    for (int i = 0; i < ids.size(); i++) {
      const string& id     = ids[i];
      const string& parent = parents[i];
      const string& type   = types[i];
      const int level      = levels[i];

      // If the level of the current entry has gone up
      // Swap the maps as the blocks are now the child nodes
      if (last_level != level) {
        node_by_id = std::move(block_by_id); // block_by_id will be empty now
        build_level();                       // Setup new level for blocks
        last_level = level;                  // Update the current level
      }

      // Find current entry's node
      Node* current_node = [&]() {
        const auto node_it = node_by_id.find(id);
        if (node_it == node_by_id.end())
          LOGIC_ERROR("Node in state (" + id + ") is not present in network");
        return node_it->second;
      }();

      // Grab parent block pointer
      Node* parent_node = [&]() {
        const auto parent_it = block_by_id.find(parent);
        // If this block is newly seen, create it
        if (parent_it == block_by_id.end())
          return block_by_id.emplace(parent, add_node(parent, type, level + 1)).first->second;
        return parent_it->second;
      }();

      // Connect node and parent to eachother
      current_node->set_parent(parent_node);
    }
  }

  void update_state(const State_Dump& state)
  {
    update_state(state.ids, state.parents, state.levels, state.types);
  }

  // =========================================================================
  // Node Grabbers
  // =========================================================================
  Type_Vec& get_nodes_at_level(const int level = 0)
  {
    check_for_level(level);
    return nodes.at(level);
  }

  Node_UPtr_Vec& get_nodes_of_type(const int type_index, const int level = 0)
  {
    check_for_type(type_index);
    return get_nodes_at_level(level)[type_index];
  }

  Node_UPtr_Vec& get_nodes_of_type(const std::string& type, const int level = 0)
  {
    return get_nodes_of_type(get_type_index(type), level);
  }

  Node* get_node_by_id(const string& id)
  {
    const auto node_it = id_to_node.find(id);

    if (node_it == id_to_node.end()) {
      LOGIC_ERROR("Node " + id + " not found in network");
    }

    return node_it->second;
  }
};