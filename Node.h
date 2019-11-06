// [[Rcpp::plugins(cpp11)]]
//=================================
// include guard
//=================================
#ifndef __NODE_INCLUDED__
#define __NODE_INCLUDED__

#include <unordered_set>
#include <string>
#include <vector>
#include <list>
#include <memory>

using std::string;
using std::vector;
using std::list;
using std::unordered_set;

//=================================
// What this file declares
//=================================
class  Node;
struct connection_info;

typedef std::shared_ptr<Node> NodePtr;

// For a bit of clarity
typedef unordered_set<NodePtr> ChildSet;

//=================================
// Main node class declaration
//=================================
class Node: public std::enable_shared_from_this<Node> {
  public:
    // Node();
    Node(string, int);            // Constructor function takes ID, node hiearchy level, and assumes default 0 for type
    Node(string, int, int);       // Constructor function takes ID, node hiearchy level, and specification of type as integer
    // ==========================================
    // Attributes
    string           id;          // Unique integer id for node
    list<NodePtr>      connections; // Nodes that are connected to this node  
    int              level;       // What level does this node sit at (0 = data, 1 = cluster, 2 = super-clusters, ...)
    NodePtr            parent;      // What node contains this node (aka its cluster)
    bool             has_parent;  // Does this node have a parent or is it the currently highest level?
    ChildSet         children;    // Nodes that are contained within node (if node is cluster)
    int              type;        // What type of node is this?

    // ==========================================
    // Methods   
    NodePtr          this_ptr();                        // Gets a shared pointer to object (replaces this) 
    void             set_parent(NodePtr);               // Set current node parent/cluster
    void             add_child(NodePtr);                // Add a node to the children vector
    void             remove_child(NodePtr);             // Remove a child node 
    void             add_connection(NodePtr);           // Add connection to another node
    ChildSet         get_children_at_level(int);      // Get all member nodes of current node at a given level
    NodePtr            get_parent_at_level(int);        // Get parent of node at a given level
    vector<NodePtr>    get_connections_to_level(int);   // Get all nodes connected to Node at a given level
    connection_info  connections_to_node(NodePtr);      // Get info on connection between any two nodes
    static void      connect_nodes(NodePtr, NodePtr);     // Static method to connect two nodes to each other with edge
};

// Structure for returning info about connection between two nodes
struct connection_info {
  int n_between;
  int n_total;
};

#endif