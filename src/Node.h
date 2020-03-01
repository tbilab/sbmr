// [[Rcpp::plugins(cpp11)]]
#ifndef __NODE_INCLUDED__
#define __NODE_INCLUDED__

// We swap out some commonly used error and message funtions depending on if this
// code is being compiled with RCPP available or not. When RCPP is being used ot
// compile the code these functions make sure messages are properly passed to R.
#if NO_RCPP
#define LOGIC_ERROR(msg) throw std::logic_error(msg)
#define RANGE_ERROR(msg) throw std::range_error(msg)
#define WARN_ABOUT(msg)          \
  const std::string w_msg = msg; \
  std::cerr << w_msg << std::endl
#define OUT_MSG std::cout
#define ALLOW_USER_BREAKOUT
#else
#include <Rcpp.h>
// Eases the process of wrapping functions to get errors forwarded to R
#define LOGIC_ERROR(msg)         \
  const std::string e_msg = msg; \
  throw Rcpp::exception(e_msg.c_str(), false)
#define RANGE_ERROR(msg)         \
  const std::string e_msg = msg; \
  throw Rcpp::exception(e_msg.c_str(), false)
#define WARN_ABOUT(msg)          \
  const std::string w_msg = msg; \
  Rcpp::warning(w_msg.c_str())

#define OUT_MSG Rcpp::Rcout
#define ALLOW_USER_BREAKOUT Rcpp::checkUserInterrupt()
#endif

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
using NodePtr     = std::shared_ptr<Node>;
using NodeVec     = std::vector<NodePtr>;
using NodeList    = std::list<NodePtr>;
using NodeSet     = std::set<NodePtr>;
using NodeEdgeMap = std::map<NodePtr, int>;
using NodeLevel   = std::map<std::string, NodePtr>;
using LevelPtr    = std::shared_ptr<NodeLevel>;
using LevelMap    = std::map<int, LevelPtr>;

enum Update_Type { Add,
                   Remove };

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

  // Disable costly copy and move methods for error protection
  // Copy construction
  Node(const Node&) = delete;
  Node& operator=(const Node&) = delete;

  // Move operations
  Node(Node&&)  = delete;
  Node& operator=(Node&&) = delete;

  // Attributes
  // =========================================================================
  std::string id;       // Unique integer id for node
  std::string type;     // What type of node is this?
  int         level;    // What level does this node sit at (0 = data, 1 = cluster, 2 = super-clusters, ...)
  NodeList    edges;    // Nodes that are connected to this node
  NodePtr     parent;   // What node contains this node (aka its cluster)
  NodeSet     children; // Nodes that are contained within node (if node is cluster)
  int         degree;   // How many edges/ edges does this node have?

  // Methods
  // =========================================================================
  NodePtr     this_ptr();                                                                     // Gets a shared pointer to object (replaces this)
  void        set_parent(NodePtr new_parent);                                                 // Set current node parent/cluster
  void        add_edge(NodePtr node);                                                         // Add edge to another node
  void        update_edges(const NodeList& moved_node_edges, const Update_Type& update_type); // Add or remove edges from nodes edge list
  NodePtr     get_parent_at_level(const int level);                                           // Get parent of node at a given level
  NodeVec     get_edges_of_type(const std::string& node_type, const int desired_level) const; // Get all nodes connected to Node at a given level
  NodeEdgeMap gather_edges_to_level(const int level) const;                                   // Get a map keyed by node with value of number of edges for all of a nodes edges to a level
  static void connect_nodes(NodePtr node_a, NodePtr node_b);                                  // Static method to connect two nodes to each other with edge
  bool        operator==(const Node& other_node);
  bool        operator==(const Node& other_node) const;
};

#endif
