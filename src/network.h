#pragma once

#include "Node.h"
#include "Sampler.h"
#include "unordered_map"
#include "vector_helpers.h"

using Node_UPtr_Vec = std::vector<Node_UPtr>;
using Type_Vec      = std::vector<std::vector<Node_UPtr>>;

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

class SBM_Network {

  private:
  // Data
  std::vector<Type_Vec> nodes;
  std::vector<string> types;
  Int_Map<string> type_name_to_int;
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

  public:
  // =========================================================================
  // Constructor
  // =========================================================================
  SBM_Network(const std::vector<std::string>& node_types = { "node" },
              const int random_seed                      = 42)
      : random_sampler(random_seed)
      , types(node_types)
  {
    int c_index = 0;
    for (const auto& type_name : node_types) {
      type_name_to_int[type_name] = c_index++;
    }

    build_level();
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
  State_Dump get_state() const
  {
    // Initialize the return struct
    State_Dump state(num_nodes());

    if (num_levels() == 1) LOGIC_ERROR("No state to export - Try adding blocks");

    // Loop through all levels that have blocks/parents
    for (int level = 0; level < nodes.size() - 1; level++) {
      for (int type_i = 0; type_i < num_types(); type_i++) {
        const string& type_name = types[type_i];
        for (const auto& node : nodes.at(level).at(type_i)) {
          state.ids.push_back(node->get_id());
          state.types.push_back(type_name);
          state.parents.push_back(node->get_parent_id());
          state.levels.push_back(level);
        }
      }
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
    check_for_level(level);

    const int type_index = get_type_index(type);

    // Build new node pointer outside of vector first for ease of pointer retrieval
    auto new_node = Node_UPtr(new Node(id, level, type_index, num_types()));

    // Get raw pointer to node to return
    Node* node_ptr = new_node.get();

    // Move node unique pointer into its type in map
    get_nodes_of_type(type_index, level).push_back(std::move(new_node));

    return node_ptr;
  }

  void initialize_blocks(int num_blocks = -1)
  {
    const bool one_block_per_node = num_blocks == -1;

    const int block_level = build_level();
    const int child_level = block_level - 1;

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
        Node* parent_block = blocks_of_type[i % num_blocks].get();
        Node* child_node   = nodes_of_type[i].get();

        // Add blocks one at a time, looping back after end to each node
        child_node->set_parent(parent_block);
      }
    }
  }

  int build_level()
  {
    // First we create a new vector with one vector per types
    nodes.emplace_back(num_types());

    // Return the index of the new level just inserted
    return nodes.size() - 1;
  }

  void delete_block_level()
  {
    if (nodes.size() == 1)
      LOGIC_ERROR("No block level to delete.");

    // Remove the last layer of nodes.
    nodes.pop_back();
  }

  void delete_all_blocks()
  {
    while (num_levels() > 1) {
      delete_block_level();
    }
  }

  // =============================================================================
  // Load current state of nodes in model from state dump given SBM::get_state()
  // =============================================================================
  void update_state(const std::vector<string>& id,
                    const std::vector<string>& parent,
                    const std::vector<int>& level,
                    const std::vector<string>& type)
  {

    // Remove all block levels

    // Build a map to get nodes by id

    // Setup map to get blocks/parents by id

    // Loop through entries of the state dump

      // If the level of the current entry has gone up

        // Swap the maps as the blocks are now the child nodes

      // Find current entry's node 

      // Grab parent block pointer
        // If it isn't in block map, make it

      // Connect node and parent to eachother 


    // const int n = id.size();

    // for (int i = 0; i < n; i++) {
    //   const std::string node_type    = type[i];
    //   const std::string child_id     = id[i];
    //   const int         child_level  = level[i];
    //   const std::string parent_id    = parent[i];
    //   const int         parent_level = child_level + 1;

    //   auto aquire_node = [node_type, this](const std::string& node_id, const int node_level) {
    //     LevelPtr nodes_at_level = get_level(node_level);

    //     // Attempt to find the node in the network
    //     auto node_loc = nodes_at_level->find(node_id);

    //     if (node_loc == nodes_at_level->end()) {
    //       return add_node(node_id, node_type, node_level);
    //     }
    //     else {
    //       return node_loc->second;
    //     }
    //   };

    //   // "none" indicates the highest level has been reached
    //   if (parent_id == "none") {
    //     continue;
    //   }

    //   // Attempt to find the parent node in the network
    //   // Next grab the child node (this one should exist...)
    //   // Assign the parent node to the child node
    //   aquire_node(child_id, child_level)->set_parent(aquire_node(parent_id, parent_level));
    // }

    // // Now clean up any potentially childless nodes that got kicked
    // // out by this process
    // clean_empty_blocks();
  }

  // =========================================================================
  // Node Grabbers
  // =========================================================================
  Type_Vec& get_nodes_at_level(const int level = 0)
  {
    // Make sure we have the requested level
    check_for_level(level);

    return nodes[level];
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

  Node* get_node_by_id(const string& id, const string& type)
  {
    auto& nodes_of_type = get_nodes_of_type(type);

    return std::find_if(nodes_of_type.begin(),
                        nodes_of_type.end(),
                        [&id](Node_UPtr& node) { return node->get_id() == id; })
        ->get();
  }
};