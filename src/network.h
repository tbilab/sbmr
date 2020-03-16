#pragma once

#include "Node.h"
#include "Sampler.h"
#include "vector_helpers.h"

using Type_Map = std::map<std::string, std::vector<NodeUPtr>>;
// using Type_Map = std::vector<std::vector<NodeUPtr>>;

class SBM_Network {

  private:
  // Data
  std::vector<Type_Map> nodes;

  Type_Map& get_nodes_at_level(const int level = 0)
  {
    // Make sure we have the requested level
    if (level >= nodes.size()) {
      RANGE_ERROR("Node requested in level that does not exist");
    }

    return nodes.at(level);
  }

  std::vector<NodeUPtr>& get_nodes_of_type(const std::string& type, const int level = 0)
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

  // Getters
  int num_nodes() const 
  {
    return total_num_elements(nodes);
  }

  int num_nodes_at_level(const int level) 
  {
      return total_num_elements(get_nodes_at_level(level));
  }
};