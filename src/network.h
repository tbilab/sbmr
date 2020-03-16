#pragma once

#include "Node.h"
#include "Sampler.h"
#include "vector_helpers.h"

using NodeUPtr_Vec = std::vector<NodeUPtr>;
using Type_Map     = std::map<std::string, NodeUPtr_Vec>;
// using Type_Map = std::vector<std::vector<NodeUPtr>>;

class SBM_Network {

  private:
  // Data
  std::vector<Type_Map> nodes;
  Sampler random_sampler;

  Type_Map& get_nodes_at_level(const int level = 0)
  {
    // Make sure we have the requested level
    if (level >= nodes.size()) {
      RANGE_ERROR("Node requested in level that does not exist");
    }

    return nodes.at(level);
  }

  NodeUPtr_Vec& get_nodes_of_type(const std::string& type, const int level = 0)
  {
    Type_Map& node_holder = get_nodes_at_level(level);

    auto loc_for_type = node_holder.find(type);
    if (loc_for_type == node_holder.end()) {
      auto inserted_vec = node_holder.emplace(type, std::vector<NodeUPtr>());
      return inserted_vec.first->second;
    } else {
      return loc_for_type->second;
    }
  }

  public:
  // Setters

  SBM_Network()
      : nodes(1)
  {
  }

  SBM_Network(const int random_seed)
      : nodes(1)
      , random_sampler(random_seed)
  {
  }

  Node* add_node(const std::string& id,
                 const std::string& type = "a",
                 const int level         = 0)
  {
    // Make sure we have the requested level
    if (level >= nodes.size()) {
      RANGE_ERROR("Node requested in level that does not exist");
    }

    std::vector<NodeUPtr>& nodes_of_type = get_nodes_of_type(type, level);

    // Build new node pointer outside of vector first for ease of pointer retrieval
    auto new_node = NodeUPtr(new Node(id, level, type));

    // Get raw pointer to node to return
    Node* node_ptr = new_node.get();

    // Move node unique pointer into its type in map
    nodes_of_type.push_back(std::move(new_node));

    return node_ptr;
  }

  void initialize_blocks(int num_blocks)
  {
    const bool one_block_per_node = num_blocks == -1;

    const int block_level = nodes.size();
    // Make a new level for the blocks
    nodes.push_back(Type_Map());

    Type_Map& child_nodes = get_nodes_at_level(block_level - 1);
    Type_Map& block_nodes = get_nodes_at_level(block_level);

    // Loop over all node types present in previous level
    for (auto& child_type : child_nodes) {
      const std::string& type     = child_type.first;
      NodeUPtr_Vec& nodes_of_type = child_type.second;

      // If we're in the 1-block-per-node mode make sure we reflect that in reserved size
      if (one_block_per_node)
        num_blocks = nodes_of_type.size();

      if (num_blocks > nodes_of_type.size()) {
        LOGIC_ERROR("Can't initialize more blocks than there are nodes of a given type");
      }

      // Setup a new empty vector for this nodes type and get a reference to it
      NodeUPtr_Vec& blocks_for_type = block_nodes.emplace(type, NodeUPtr_Vec()).first->second;

      // Reserve enough spaces for the blocks to be inserted
      blocks_for_type.reserve(num_blocks);

      for (int i = 0; i < num_blocks; i++) {
        // Build a new block node wrapped in smart pointer in it's type vector
        blocks_for_type.emplace_back(new Node("block", block_level, type));
      }

      // Shuffle child nodes if we're randomly assigning blocks
      if (!one_block_per_node) random_sampler.shuffle(nodes_of_type);

      // Loop through now shuffled children nodes
      for (int i = 0; i < nodes_of_type.size(); i++) {
        Node* parent_block = blocks_for_type[i % num_blocks].get();
        Node* child_node   = nodes_of_type[i].get();

        // Add blocks one at a time, looping back after end to each node
        child_node->set_parent(parent_block);
      }
    }
  }

  // Getters
  int num_nodes() const
  {
    return total_num_elements(nodes);
  }

  int num_nodes_at_level(const int level) 
  {
    return total_num_elements(get_nodes_at_level(level));
  }

  int num_levels() const {
    return nodes.size();
  }

  int num_nodes_of_type(const std::string type, const int level = 0) {
    return get_nodes_of_type(type, level).size();
  }

  int num_types() const {
    return nodes.at(0).size();
  }
};