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
using Node_Ptr_Vec   = std::vector<Node*>;
using Edges_By_Type  = std::vector<Node_Ptr_Vec>;
using Node_Vec       = std::vector<Node*>;
using Edge_Count_Map = std::map<Node*, int>;

enum Update_Type { Add,
                   Remove };

//=================================
// Main node class declaration
//=================================
class Node {
  private:
  Node* parent = nullptr; // What node contains this node (aka its cluster)
  Edges_By_Type edges;
  Node_Vec children; // Nodes that are contained within node (if node is cluster)
  int degree = 0;    // How many edges/ edges does this node have?
  std::string id;    // Unique integer id for node
  int type;          // What type of node is this?
  int level;         // What level does this node sit at (0 = data, 1 = cluster, 2 = super-clusters, ...)

  public:
  // =========================================================================
  // Constructors
  // =========================================================================
  Node(const std::string& node_id, const int level, const int type, const int num_types = 1)
      : id(node_id)
      , type(type)
      , level(level)
      , edges(num_types)
  {
  }

  Node(const int type, const int level, const int num_types = 1)
      : type(type)
      , level(level)
      , edges(num_types)
  {
  }

  // =========================================================================
  // Constant attribute getters - these are static after node creation
  // =========================================================================
  std::string get_id() const { return id; }
  int get_type() const { return type; }
  int get_level() const { return level; }

  // Disable costly copy and move methods for error protection
  // Copy construction
  Node(const Node&) = delete;
  Node& operator=(const Node&) = delete;

  // Move operations
  Node(Node&&)  = delete;
  Node& operator=(Node&&) = delete;

  // =========================================================================
  // Children-Related methods
  // =========================================================================
  void add_child(Node* child)
  {
    children.push_back(child);

    // Add new child's edges
    update_edges(child->all_edges(), Add);
  }

  void remove_child(Node* child)
  {
    delete_from_vector(children, child);

    // Remove child's edges
    update_edges(child->all_edges(), Remove);
  }

  int num_children() const
  {
    return children.size();
  }

  bool no_children() const
  {
    return num_children() == 0;
  }

  bool is_child(Node* node) const
  {
    return std::find(children.begin(),
                     children.end(),
                     node)
        != children.end();
  }

  // =========================================================================
  // Parent-Related methods
  // =========================================================================
  void set_parent(Node* new_parent)
  {

    if (level != new_parent->level - 1) {
      LOGIC_ERROR("Parent node must be one level above child");
    }

    // Remove self from previous parents children list (if it existed)
    if (parent != nullptr)
      parent->remove_child(this);

    // Add this node to new parent's children list
    new_parent->add_child(this);

    // Set this node's parent
    parent = new_parent;
  }

  // Get parent of node at a given level
  Node* get_parent_at_level(const int level_of_parent)
  {
    // First we need to make sure that the requested level is not less than that
    // of the current node.
    if (level_of_parent < level)
      LOGIC_ERROR("Requested parent level (" + as_str(level_of_parent)
                  + ") lower than current node level (" + as_str(level) + ").");

    // Start with this node as current node
    Node* current_node = this;

    while (current_node->level != level_of_parent) {
      if (!parent)
        RANGE_ERROR("No parent at level " + as_str(level_of_parent) + " for " + id);

      // Traverse up parents until we've reached just below where we want to go
      current_node = current_node->parent;
    }

    // Return the final node, aka the parent at desired level
    return current_node;
  }

  Node* get_parent() const
  {
    return parent;
  }

  // =========================================================================
  // Edge-Related methods
  // =========================================================================
  Node_Ptr_Vec& get_edges_of_type(const int node_type)
  {
    return edges.at(node_type);
  }

  // Collapse edges to a given level into a map of connected block id->count
  Edge_Count_Map gather_edges_to_level(const int level) const
  {
    // Setup an edge count map for node
    Edge_Count_Map edges_counts;

    for (const auto& edges_of_type : edges) {
      for (const auto& edge : edges_of_type) {
        edges_counts[edge->get_parent_at_level(level)]++;
      }
    }

    return edges_counts;
  }

  void add_edge(Node* node)
  {
    get_edges_of_type(node->type).push_back(node);
    degree++;
  }

  Edges_By_Type& all_edges()
  {
    return edges;
  }

  void update_edges(const Edges_By_Type& edges_to_update, const Update_Type& update_type)
  {
    int type_i = 0;

    for (const auto& edges_of_type : edges_to_update) {
      // Get references to this node's edges to type (update type for next go-round)
      auto& node_edges_of_type = get_edges_of_type(type_i++);

      for (const auto& edge : edges_of_type) {
        switch (update_type) {
        case Remove:
          delete_from_vector(node_edges_of_type, edge);
          degree--;
          break;
        case Add:
          node_edges_of_type.push_back(edge);
          degree++;
          break;
        default:
          LOGIC_ERROR("Something went wrong in edge updating");
        }
      }
    }
  }

  // =========================================================================
  // Comparison operators
  // =========================================================================
  bool operator==(const Node& other_node) { return id == other_node.id; }
  bool operator==(const Node& other_node) const { return id == other_node.id; }
};

// =============================================================================
// Static method to connect two nodes to each other with edge
// =============================================================================
inline void connect_nodes(Node* node_a, Node* node_b)
{
  node_a->add_edge(node_b);
  node_b->add_edge(node_a);
}
