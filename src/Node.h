// [[Rcpp::plugins(cpp11)]]
//=================================
// include guard
//=================================
#ifndef __NODE_INCLUDED__
#define __NODE_INCLUDED__

#if NO_RCPP
#define RCPP_ERRORS_BEGIN
#define RCPP_ERRORS_END
#else
#include <Rcpp.h>
// Eases the process of wrapping functions to get errors forwarded to R
#define RCPP_ERRORS_BEGIN try {
#define RCPP_ERRORS_END                      \
  }                                          \
  catch (const std::exception& ex)           \
  {                                          \
    throw Rcpp::exception(ex.what(), false); \
  }
#endif

#define LOGIC_ERROR std::logic_error
#define RANGE_ERROR std::range_error

#include "profiling/Instrument.h"

#include <exception>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using std::string;

// =============================================================================
// What this file declares
// =============================================================================
class Node;

// For a bit of clarity
typedef std::shared_ptr<Node>            NodePtr;
typedef std::unordered_set<NodePtr>      ChildSet;
typedef std::vector<NodePtr>             NodeVec;
typedef std::list<NodePtr>               NodeList;
typedef std::unordered_map<NodePtr, int> NodeEdgeMap;
typedef std::shared_ptr<Node>            NodePtr;
typedef std::map<string, NodePtr>        NodeLevel;
typedef std::shared_ptr<NodeLevel>       LevelPtr;
typedef std::map<int, LevelPtr>          LevelMap;

//=================================
// Main node class declaration
//=================================
class Node : public std::enable_shared_from_this<Node> {
  public:
  // Constructors
  // =========================================================================

  // Takes ID, node hiearchy level, and assumes default 'a' for type
  Node(std::string node_id, int level)
      : id(node_id)
      , type("a")
      , level(level)
      , degree(0)
  {
  }

  // Takes the node's id, level, and type.
  Node(std::string node_id, int level, std::string type)
      : id(node_id)
      , type(type)
      , level(level)
      , degree(0)
  {
  }

  // Takes the node's id, level, and type as integer (for legacy api compatability)
  Node(std::string node_id, int level, int type)
      : id(node_id)
      , type(std::to_string(type))
      , level(level)
      , degree(0)
  {
  }

  // Attributes
  // =========================================================================
  std::string id;       // Unique integer id for node
  std::string type;     // What type of node is this?
  int         level;    // What level does this node sit at (0 = data, 1 = cluster, 2 = super-clusters, ...)
  NodeList    edges;    // Nodes that are connected to this node
  NodePtr     parent;   // What node contains this node (aka its cluster)
  ChildSet    children; // Nodes that are contained within node (if node is cluster)
  int         degree;   // How many edges/ edges does this node have?

  // Methods
  // =========================================================================
  NodePtr     this_ptr();                                                                // Gets a shared pointer to object (replaces this)
  void        set_parent(NodePtr new_parent);                                            // Set current node parent/cluster
  void        add_child(NodePtr new_child);                                              // Add a node to the children vector
  void        remove_child(NodePtr child);                                               // Remove a child node
  void        add_edge(NodePtr node);                                                    // Add edge to another node
  void        update_edges_from_node(NodePtr node, bool remove);                         // Add or remove edges from nodes edge list
  NodePtr     get_parent_at_level(int level);                                            // Get parent of node at a given level
  NodeVec     get_edges_to_level(const int desired_level, const std::string& node_type); // Get all nodes connected to Node at a given level
  NodeEdgeMap gather_edges_to_level(int level);                                          // Get a map keyed by node with value of number of edges for all of a nodes edges to a level
  static void connect_nodes(NodePtr node_a, NodePtr node_b);                             // Static method to connect two nodes to each other with edge
};

#endif