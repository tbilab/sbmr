// [[Rcpp::plugins(cpp11)]]
//=================================
// include guard
//=================================
#ifndef __NODE_INCLUDED__
#define __NODE_INCLUDED__

#include <unordered_set>
#include <queue> 
#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>

using std::string;

// =============================================================================
// What this file declares
// =============================================================================
class  Node;

// For a bit of clarity
typedef std::shared_ptr<Node>       NodePtr;
typedef std::unordered_set<NodePtr> ChildSet;

//=================================
// Main node class declaration
//=================================
class Node: public std::enable_shared_from_this<Node> {
  public:

    // Constructors
    // =========================================================================
  
    // Takes ID, node hiearchy level, and assumes default 0 for type
    Node(string node_id, int level):
      id(node_id),
      level(level),
      type(0),
      degree(0){}

    // Takes the node's id, level, and type. 
    Node(string node_id, int level, int type):
      id(node_id),
      level(level),
      type(type),
      degree(0){}   
    
    // Attributes
    // =========================================================================
    // Unique integer id for node
    string id;

    // Nodes that are connected to this node
    std::list<NodePtr> connections;

    // What level does this node sit at (0 = data, 1 = cluster, 2 = super-clusters, ...)
    int level;

    // What node contains this node (aka its cluster)
    NodePtr parent;

    // Nodes that are contained within node (if node is cluster)
    ChildSet children;

    // What type of node is this?
    int type;

    // How many connections/ edges does this node have?
    int degree;

    // Methods
    // =========================================================================
    // Gets a shared pointer to object (replaces this)
    NodePtr this_ptr();

    // Set current node parent/cluster
    void set_parent(NodePtr new_parent);

    // Add a node to the children vector
    void add_child(NodePtr new_child);

    // Remove a child node
    void remove_child(NodePtr child);

    // Add connection to another node
    void add_connection(NodePtr node);

    // Update degree of node by specified amount, propigating to all parents
    void update_degree(int change_in_degree);

    // Get all member nodes of current node at a given level
    ChildSet get_children_at_level(int level);

    // Get parent of node at a given level
    NodePtr get_parent_at_level(int level);

    // Get all nodes connected to Node at a given level
    std::vector<NodePtr> get_connections_to_level(int level);

    // Get a map keyed by node with value of number of connections for all of
    // a nodes connections to a level
    std::map<NodePtr, int> gather_connections_to_level(int level);

    // Static method to connect two nodes to each other with edge
    static void connect_nodes(NodePtr node_a, NodePtr node_b);
};

#endif