// [[Rcpp::plugins(cpp11)]]
//=================================
// include guard
//=================================
#ifndef __NODE_INCLUDED__
#define __NODE_INCLUDED__

#include "profiling/Instrument.h"

#include <exception>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>
using std::string;

// =============================================================================
// What this file declares
// =============================================================================
class Node;

// For a bit of clarity
typedef std::shared_ptr<Node>       NodePtr;
typedef std::unordered_set<NodePtr> ChildSet;
typedef std::vector<NodePtr>        NodeVec;
typedef std::list<NodePtr>          NodeList;
typedef std::map<NodePtr, int>      NodeEdgeMap;

//=================================
// Main node class declaration
//=================================
class Node : public std::enable_shared_from_this<Node> {
  public:
  // Constructors
  // =========================================================================

  // Takes ID, node hiearchy level, and assumes default 0 for type
  Node(string node_id, int level)
      : id(node_id)
      , level(level)
      , type(0)
      , degree(0)
  {
  }

  // Takes the node's id, level, and type.
  Node(string node_id, int level, int type)
      : id(node_id)
      , level(level)
      , type(type)
      , degree(0)
  {
  }

  // Attributes
  // =========================================================================
  string   id;       // Unique integer id for node
  NodeList edges;    // Nodes that are connected to this node
  int      level;    // What level does this node sit at (0 = data, 1 = cluster, 2 = super-clusters, ...)
  NodePtr  parent;   // What node contains this node (aka its cluster)
  ChildSet children; // Nodes that are contained within node (if node is cluster)
  int      type;     // What type of node is this?
  int      degree;   // How many edges/ edges does this node have?

  // Methods
  // =========================================================================
  NodePtr     this_ptr();                                        // Gets a shared pointer to object (replaces this)
  void        set_parent(NodePtr new_parent);                    // Set current node parent/cluster
  void        add_child(NodePtr new_child);                      // Add a node to the children vector
  void        remove_child(NodePtr child);                       // Remove a child node
  void        add_edge(NodePtr node);                            // Add edge to another node
  void        update_edges_from_node(NodePtr node, bool remove); // Add or remove edges from nodes edge list
  NodePtr     get_parent_at_level(int level);                    // Get parent of node at a given level
  NodeVec     get_edges_to_level(int level);                     // Get all nodes connected to Node at a given level
  NodeEdgeMap gather_edges_to_level(int level);                  // Get a map keyed by node with value of number of edges for all of a nodes edges to a level
  static void connect_nodes(NodePtr node_a, NodePtr node_b);     // Static method to connect two nodes to each other with edge
};

#endif