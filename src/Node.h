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

// For a bit of clarity
using Node_UPtr      = std::unique_ptr<Node>;
using Node_Ptr_Vec   = std::vector<Node*>;
using Edges_By_Type  = std::vector<Node_Ptr_Vec>;
using Node_Vec       = std::vector<Node*>;
using Node_Set       = std::set<Node*>;
using Edge_Count_Map = std::map<Node*, int>;

enum Update_Type { Add,
                   Remove };

//=================================
// Main node class declaration
//=================================
class Node {
  private:
  Node* parent = nullptr; // What node contains this node (aka its cluster)
  Edges_By_Type typed_edges;
  Node_Vec children; // Nodes that are contained within node (if node is cluster)

  bool have_parent() { return parent != nullptr; }

  public:
  // Constructors
  // =========================================================================

  // Takes the node's id, level, and type.
  Node(const std::string& node_id, const int level, const int type, const int num_types = 1)
      : id(node_id)
      , type(type)
      , level(level)
      , typed_edges(num_types)
  {
  }

  // Takes a type int and a level and leaves id empty (useful for block nodes)
  Node(const int type, const int level, const int num_types = 1)
      : type(type)
      , level(level)
      , typed_edges(num_types)
  {
  }

  // Disable costly copy and move methods for error protection
  // Copy construction
  Node(const Node&) = delete;
  Node& operator=(const Node&) = delete;

  // Move operations
  Node(Node&&)  = delete;
  Node& operator=(Node&&) = delete;

  // Attributes
  // =========================================================================
  std::string id; // Unique integer id for node
  int type;       // What type of node is this?
  int level;      // What level does this node sit at (0 = data, 1 = cluster, 2 = super-clusters, ...)
  int degree = 0; // How many edges/ edges does this node have?

  // Methods
  // =========================================================================
  void set_parent(Node* new_parent)
  {

    if (level != new_parent->level - 1) {
      LOGIC_ERROR("Parent node must be one level above child");
    }

    // Remove self from previous parents children list (if it existed)
    if (have_parent()) {
      // Remove this node's edges contribution from parent's
      parent->update_edges(typed_edges, Remove);

      // Remove self from previous children
      parent->remove_child(this);
    }

    // Add this node's edges to parent's degree count
    new_parent->update_edges(typed_edges, Add);

    // Add this node to new parent's children list
    new_parent->add_child(this);

    // Set this node's parent
    parent = new_parent;
  }

  void add_child(Node* child)
  {
    children.push_back(child);
  }

  void remove_child(Node* child)
  {
    delete_from_vector(children, child);
  }

  int num_children() const { return children.size(); }

  bool is_child(Node* node) const { return std::find(children.begin(), children.end(), node) != children.end(); }

  // Get parent of node at a given level
  Node* get_parent_at_level(const int level_of_parent)
  {
    // First we need to make sure that the requested level is not less than that
    // of the current node.
    if (level_of_parent < level) {
      LOGIC_ERROR("Requested parent level (" + std::to_string(level_of_parent) + ") lower than current node level (" + std::to_string(level) + ").");
    }

    // Start with this node as current node
    Node* current_node = this;

    while (current_node->level != level_of_parent) {
      if (!parent) {
        RANGE_ERROR("No parent at level " + std::to_string(level_of_parent) + " for " + id);
      }

      // Traverse up parents until we've reached just below where we want to go
      current_node = current_node->parent;
    }

    // Return the final node, aka the parent at desired level
    return current_node;
  }

  Node* get_parent() { return parent; }

  // =============================================================================
  // Get all nodes connected to Node at a given level with specified type
  // We return a vector because we need random access to elements in this array
  // and that isn't provided to us with the list format.
  // =============================================================================
  Node_Ptr_Vec& get_edges_of_type(const int node_type)
  {

    return typed_edges.at(node_type);
  }

  // =============================================================================
  // Collapse a nodes edge to a given level into a map of
  // connected block id->count
  // =============================================================================
  Edge_Count_Map gather_edges_to_level(const int level) const
  {
    // Setup an edge count map for node
    Edge_Count_Map edges_counts;

    for (const auto& edges_of_type : typed_edges) {
      for (const auto& edge : edges_of_type) {
        edges_counts[edge->get_parent_at_level(level)]++;
      }
    }

    return edges_counts;
  }

  // =============================================================================
  // Add edge to another node
  // =============================================================================
  void add_edge(Node* node)
  {
    get_edges_of_type(node->type).push_back(node);
    degree++;
  }

  void remove_edge(Node_Vec& edge_list, Node* node_to_remove)
  {
    delete_from_vector(get_edges_of_type(node_to_remove->type), node_to_remove);
  }

  // =============================================================================
  // Add or remove edges from a nodes edge list
  // =============================================================================
  void update_edges(const Edges_By_Type& edges_to_update, const Update_Type& update_type)
  {

    for (int type_i = 0; type_i < edges_to_update.size(); type_i++) {
      auto& node_edges_of_type = get_edges_of_type(type_i);

      for (const auto& edge : edges_to_update.at(type_i)) {
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
          break;
        }
      }
    }
  }

  int no_children() const { return children.size() == 0; }
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
