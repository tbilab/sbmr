// [[Rcpp::plugins(cpp11)]]
#pragma once
#include "error_and_message_macros.h"
#include "profiling/Instrument.h"


#include <exception>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <vector>

// =============================================================================
// What this file declares
// =============================================================================
class Node;

// For a bit of clarity
// using Node*     = std::shared_ptr<Node>;
using NodeUPtr = std::unique_ptr<Node>;

using NodeVec     = std::vector<Node*>;
using NodeList    = std::list<Node*>;
using NodeSet     = std::set<Node*>;
using NodeEdgeMap = std::map<Node*, int>;
using NodeLevel   = std::map<std::string, Node*>;
using LevelPtr    = std::shared_ptr<NodeLevel>;
using LevelMap    = std::map<int, LevelPtr>;

enum Update_Type { Add,
                   Remove };

//=================================
// Main node class declaration
//=================================
class Node {
  private:
  bool have_parent()
  {
    return parent != nullptr;
  }

  public:
  // Constructors
  // =========================================================================

  // Takes ID, node hiearchy level, and assumes default 'a' for type
  Node(const std::string& node_id, const int level)
      : id(node_id)
      , type("a")
      , level(level)
  {
  }

  // Takes the node's id, level, and type.
  Node(const std::string& node_id, const int level, const std::string& type)
      : id(node_id)
      , type(type)
      , level(level)
  {
  }

  // Takes the node's id, level, and type as integer (for legacy api compatability)
  Node(const std::string& node_id, const int level, const int type)
      : id(node_id)
      , type(std::to_string(type))
      , level(level)
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
  std::string id;           // Unique integer id for node
  std::string type;         // What type of node is this?
  int level;                // What level does this node sit at (0 = data, 1 = cluster, 2 = super-clusters, ...)
  NodeList edges;           // Nodes that are connected to this node
  Node* parent = nullptr; // What node contains this node (aka its cluster)
  NodeSet children;         // Nodes that are contained within node (if node is cluster)
  int degree = 0;               // How many edges/ edges does this node have?

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
      parent->update_edges(edges, Remove);

      // Remove self from previous children
      parent->children.erase(this);
    }

    // Add this node's edges to parent's degree count
    new_parent->update_edges(edges, Add);

    // Add this node to new parent's children list
    new_parent->children.insert(this);

    // Set this node's parent
    parent = new_parent;
  }

  // Get parent of node at a given level
  Node* get_parent_at_level(const int level_of_parent)
  {
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
  }

  Node* get_parent() { return parent; }

  // =============================================================================
  // Get all nodes connected to Node at a given level with specified type
  // We return a vector because we need random access to elements in this array
  // and that isn't provided to us with the list format.
  // =============================================================================
  NodeVec get_edges_of_type(const std::string& node_type, const int desired_level) const
  {
    // Vector to return containing parents at desired level for edges
    NodeVec level_cons;

    // Conservatively assume all edges will be taken
    level_cons.reserve(edges.size());

    // Go through every child node's edges list, find parent at
    // desired level and place in connected nodes vector
    for (const auto& edge : edges) {
      if (edge->type == node_type) {
        level_cons.push_back(edge->get_parent_at_level(desired_level));
      }
    }

    return level_cons;
  }

  // =============================================================================
  // Collapse a nodes edge to a given level into a map of
  // connected block id->count
  // =============================================================================
  NodeEdgeMap gather_edges_to_level(const int level) const
  {
    // Setup an edge count map for node
    NodeEdgeMap edges_counts;

    // Fill out edge count map by
    // - looping over all edges
    // - mapping them to the desired level
    // - and adding to their counts
    for (const auto& curr_edge : edges) {
      edges_counts[curr_edge->get_parent_at_level(level)]++;
    }

    return edges_counts;
  }

  // =============================================================================
  // Add edge to another node
  // =============================================================================
  void add_edge(Node* node)
  {
    // propigate new edge upwards to all parents
    Node* current_node = this;
    int current_level    = level;

    while (current_node) {
      // Add node to base edges
      (current_node->edges).push_back(node);
      current_node->degree++;
      current_node = current_node->parent;
      current_level++;
    }
  }

  void remove_edge(NodeList& edge_list, const Node* node_to_remove)
  {
    // Scan through edges untill we find the first instance
    // of the connected node we want to remove
    auto loc_of_edge = std::find(edge_list.begin(),
                                 edge_list.end(),
                                 node_to_remove);

    if (loc_of_edge != edge_list.end()) {
      edge_list.erase(loc_of_edge);
    } else {
      LOGIC_ERROR("Trying to erase non-existant edge from parent node.");
    }
  }

  int no_children() const {
    return children.size() == 0;
  }



  // =============================================================================
  // Add or remove edges from a nodes edge list
  // =============================================================================
  void update_edges(const NodeList& moved_node_edges, const Update_Type& update_type)
  {
    // We will scan upward from the this node up through all its parents
    // First, we start with this node
    auto node_being_updated = this;

    // While we still have a node to continue to in the hierarchy...
    while (node_being_updated) {
      // Loop through all the edges that are being updated...
      // Grab reference to the current nodes edges list
      NodeList& updated_node_edges = node_being_updated->edges;

      // Loop through all the edges from the node being moved
      for (const auto& edge_to_update : moved_node_edges) {

        switch (update_type) {
        case Remove:
          remove_edge(updated_node_edges, edge_to_update);
          break;
        case Add:
          updated_node_edges.push_back(edge_to_update);
          break;
        default:
          LOGIC_ERROR("Something went wrong in edge updating");
          break;
        }
      }

      // Update degree of current node
      node_being_updated->degree = node_being_updated->edges.size();

      // Update current node to nodes parent
      node_being_updated = node_being_updated->parent;
    }
  }

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
