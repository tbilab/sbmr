// [[Rcpp::plugins(cpp11)]]
#pragma once
#include "error_and_message_macros.h"
#include "vector_helpers.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

// =============================================================================
// What this file declares
// =============================================================================
class Node;

using string = std::string;

template <typename T>
inline string as_str(const T& val) { return std::to_string(val); }

// For a bit of clarity
using Node_UPtr      = std::unique_ptr<Node>;
using Node_UPtr_Vec  = std::vector<Node_UPtr>;
using Node_Ptr_Vec   = std::vector<Node*>;
using Edges_By_Type  = std::vector<Node_Ptr_Vec>;
using Node_Vec       = std::vector<Node*>;
using Edge_Count_Map = std::map<const Node*, int>;
using Type_Vec       = std::vector<std::vector<Node_UPtr>>;

enum Update_Type { Add,
                   Remove };

//=================================
// Main node class declaration
//=================================
class Node {
  private:
  Node* parent_node = nullptr; // What node contains this node (aka its cluster)
  int _degree       = 0;       // How many neighbors does this node have?
  Node_Vec _children;          // Nodes that are contained within node (if node is cluster)
  string _id;                  // Unique integer id for node
  int _level;                  // What level does this node sit at (0 = data, 1 = cluster, 2 = super-clusters, ...)
  int _type;                   // What type of node is this?
  Edges_By_Type _neighbors;

  public:
  // =========================================================================
  // Constructors
  // =========================================================================
  Node(const string& node_id,
       const int level,
       const int type,
       const int n_types = 1)
      : _id(node_id)
      , _level(level)
      , _type(type)
      , _neighbors(n_types)
  {
  }

  // Destructor
  ~Node()
  {
    std::for_each(_children.begin(), _children.end(), [](Node* c) { c->remove_parent(); });
  }

  // Disable costly copy and move methods for error protection
  // Copy construction
  Node(const Node&) = delete;
  Node& operator=(const Node&) = delete;
  Node(Node&&)                 = delete;
  Node& operator=(Node&&) = delete;

  // =========================================================================
  // Constant attribute getters - these are static after node creation
  // =========================================================================
  string id() const { return _id; }
  int type() const { return _type; }
  Node* parent() const { return parent_node; }
  int degree() const { return _degree; }
  int level() const { return _level; }

  // =========================================================================
  // Children-Related methods
  // =========================================================================
  const Node_Vec& children() const
  {
    return _children;
  }

  void add_child(Node* child)
  {
    _children.push_back(child);

    // Add new child's neighbors
    update_neighbors(child->neighbors(), Add);
  }

  void remove_child(Node* child)
  {
    delete_from_vector(_children, child);

    // Remove child's neighbors
    update_neighbors(child->neighbors(), Remove);
  }

  int n_children() const
  {
    return _children.size();
  }

  bool is_empty() const
  {
    return _children.empty();
  }

  bool has_child(Node* node) const
  {
    return std::find(_children.begin(), _children.end(), node) != _children.end();
  }

  Node* get_only_child() const
  {
    if (n_children() > 1) LOGIC_ERROR("Cant get only child, block has more than one child");

    return _children[0];
  }

  void empty_children()
  {
    // Remove all children from vector
    _children.clear();
  }

  // =========================================================================
  // Parent-Related methods
  // =========================================================================
  void set_parent(Node* new_parent, const bool remove_from_old = true)
  {
    if (_level != new_parent->level() - 1) LOGIC_ERROR("Parent node must be one level above child");

    // Remove self from previous parent's children list (if it existed)
    if (remove_from_old && has_parent()) parent_node->remove_child(this);

    // Add this node to new parent's children list
    new_parent->add_child(this);

    // Set this node's parent
    parent_node = new_parent;
  }

  // Get parent of node at a given level
  Node* parent_at_level(const int level_of_parent) const
  {
    // First we need to make sure that the requested level is not less than that
    // of the current node.
    if (level_of_parent < _level) LOGIC_ERROR("Requested parent level ("
                                              + as_str(level_of_parent)
                                              + ") lower than current node level ("
                                              + as_str(_level) + ").");

    // Start with this node as current node
    Node* cur_node     = parent_node;
    int cur_node_level = _level + 1;

    while (cur_node_level != level_of_parent && cur_node->has_parent()) {
      // Traverse up parents until we've reached just below where we want to go
      cur_node = cur_node->parent();
      cur_node_level++;
    }

    if (cur_node_level != level_of_parent) RANGE_ERROR("No parent at level "
                                                       + as_str(level_of_parent)
                                                       + " for " + id());

    // Return the final node, aka the parent at desired level
    return cur_node;
  }

  bool has_parent() const { return parent_node != nullptr; }

  void remove_parent() { parent_node = nullptr; }

  // =========================================================================
  // Neighbor-Related methods
  // =========================================================================
  const Edges_By_Type& neighbors() const
  {
    return _neighbors;
  }

  const Node_Ptr_Vec& neighbors_of_type(const int node_type) const
  {
    return _neighbors.at(node_type);
  }

  // Collapse neighbors to a given level into a map of connected block id->count
  Edge_Count_Map gather_neighbors_at_level(const int level) const
  {
    // Setup an neighbor count map for node
    Edge_Count_Map counts;

    for_all_neighbors([&](const Node* n) { counts[n->parent_at_level(level)]++; });

    return counts;
  }

  void add_neighbor(Node* node)
  {
    _neighbors.at(node->type()).push_back(node);
    _degree++;
  }

  void update_neighbors(const Edges_By_Type& neighbors_to_update, const Update_Type& update_type)
  {

    int type_i = 0;

    for (const auto& nodes_of_type : neighbors_to_update) {
      // Get references to this node's neighbors to type (update type for next go-round)
      auto& node_neighbors_of_type = _neighbors.at(type_i++);

      for (const auto& node : nodes_of_type) {
        switch (update_type) {
        case Remove:
          delete_from_vector(node_neighbors_of_type, node);
          _degree--;
          break;
        case Add:
          node_neighbors_of_type.push_back(node);
          _degree++;
          break;
        }
      }
    }

    // Propagate edge changes up hierarchy
    if (has_parent()) parent_node->update_neighbors(neighbors_to_update, update_type);
  }

  // Apply a lambda function over all nodes in network
  void for_all_neighbors(std::function<void(const Node* node)> fn) const
  {
    for (const auto& neighbors_of_type : _neighbors) {
      std::for_each(neighbors_of_type.begin(), neighbors_of_type.end(), fn);
    }
  }

  // =========================================================================
  // Comparison operators
  // =========================================================================
  bool operator==(const Node& other_node) { return id() == other_node.id(); }
  bool operator==(const Node& other_node) const { return id() == other_node.id(); }
};

// =============================================================================
// Static method to connect two nodes to each other with an edge
// =============================================================================
inline void connect_nodes(Node* node_a, Node* node_b)
{
  node_a->add_neighbor(node_b);
  node_b->add_neighbor(node_a);
}
