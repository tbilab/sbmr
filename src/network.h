#pragma once

#include "Node.h"
#include "Sampler.h"
#include "unordered_map"
#include "vector_helpers.h"

using NodeUPtr_Vec = std::vector<NodeUPtr>;
using Type_Vec = std::vector<std::vector<NodeUPtr>>;


class SBM_Network {

  private:
  // Data
  std::vector<Type_Vec> nodes;
  std::unordered_map<string, int> type_name_to_int;
  Sampler random_sampler;

  Type_Vec& get_nodes_at_level(const int level = 0)
  {
    // Make sure we have the requested level
    if (level >= nodes.size()) {
      RANGE_ERROR("Node requested in level that does not exist");
    }

    return nodes.at(level);
  }

  int get_type_index(const string name)
  {
    const auto name_it = type_name_to_int.find(name);

    // If this is a previously unseen type, add new entry
    if (name_it == type_name_to_int.end()) {
      LOGIC_ERROR("Type " + name + " doesn't exist in network");
    }

    return name_it->second;
  }

  int build_level() {
    // First we create a new vector with one vector per types
    nodes.emplace_back(num_types());

    // Return the index of the new level just inserted
    return nodes.size() - 1;
  }

  public:
  // Setters

  SBM_Network(const std::vector<std::string>& node_types = { "node" },
              const int random_seed                      = 42)
      : random_sampler(random_seed)
  {
    int c_index = 0;
    for (const auto& type_name : node_types) {
      type_name_to_int[type_name] = c_index;
      c_index++;
    }

    build_level();
  }

  Node* add_node(const std::string& id,
                 const std::string& type = "a",
                 const int level         = 0)
  {
    // Make sure we have the requested level
    if (level >= nodes.size()) {
      RANGE_ERROR("Node requested in level that does not exist");
    }

    const int type_index = get_type_index(type);

    // Build new node pointer outside of vector first for ease of pointer retrieval
    auto new_node = NodeUPtr(new Node(id, level, type_index));

    // Get raw pointer to node to return
    Node* node_ptr = new_node.get();

    // Move node unique pointer into its type in map
    nodes.at(level).at(type_index).push_back(std::move(new_node));

    return node_ptr;
  }

  void initialize_blocks(int num_blocks = -1)
  {
    const bool one_block_per_node = num_blocks == -1;

    const int block_level = build_level();

    Type_Vec& child_nodes = nodes.at(block_level-1);
    Type_Vec& block_nodes = nodes.at(block_level);

    // Loop over all node types present in previous level
    for (int type_i = 0; type_i < child_nodes.size(); type_i++) {
      NodeUPtr_Vec& nodes_of_type  = child_nodes[type_i];
      NodeUPtr_Vec& blocks_of_type = block_nodes[type_i];

      // If we're in the 1-block-per-node mode make sure we reflect that in reserved size
      if (one_block_per_node)
        num_blocks = nodes_of_type.size();

      if (num_blocks > nodes_of_type.size()) {
        LOGIC_ERROR("Can't initialize more blocks than there are nodes of a given type");
      }

      // Reserve enough spaces for the blocks to be inserted
      blocks_of_type.reserve(num_blocks);

      for (int i = 0; i < num_blocks; i++) {
        // Build a new block node wrapped in smart pointer in it's type vector
        blocks_of_type.emplace_back(new Node("block", block_level, type_i));
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

  void delete_blocks()
  {

    if (nodes.size() == 1) {
      LOGIC_ERROR("No block level to delete.");
    }
    // Remove the last layer of nodes.
    nodes.pop_back();
  }

  // Getters
  NodeUPtr_Vec& get_nodes_of_type(const std::string& type, const int level = 0)
  {
    return get_nodes_of_type(get_type_index(type), level);
  }

  NodeUPtr_Vec& get_nodes_of_type(const int type_index, const int level = 0)
  {
    Type_Vec& node_holder = get_nodes_at_level(level);

    return node_holder[type_index];
  }

  int num_nodes() const
  {
    return total_num_elements(nodes);
  }

  int num_nodes_at_level(const int level)
  {
    return total_num_elements(get_nodes_at_level(level));
  }

  int num_levels() const
  {
    return nodes.size();
  }

  template <typename T>
  int num_nodes_of_type(const T type, const int level = 0)
  {
    return get_nodes_of_type(type, level).size();
  }

  int num_types() const
  {
    return type_name_to_int.size();
  }
};