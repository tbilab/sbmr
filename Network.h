#ifndef __NETWORK_INCLUDED__
#define __NETWORK_INCLUDED__

#include "Node.h" 
#include "helpers.h" 
#include <math.h>   

#include <map>
#include <set>
#include <memory>
#include <numeric>
#include <utility> 
#include <iostream>


// =============================================================================
// What this file declares
// =============================================================================
class SBM;
struct State_Dump;

// Some type definitions for cleaning up ugly syntax
typedef std::shared_ptr<Node>                      NodePtr;
typedef std::map<string, NodePtr>                  NodeLevel;
typedef std::shared_ptr<NodeLevel>                 LevelPtr;
typedef std::map<int, LevelPtr>                    LevelMap;
typedef std::map<std::pair<NodePtr, NodePtr>, int> EdgeCounts;

using std::map;
using std::string;
using std::vector;

// =============================================================================
// Main node class declaration
// =============================================================================
class Network {
  public:
    // Constructor
    // Network();

    // ==========================================
    // Attributes
    LevelMap nodes;                  // A map keyed by level integer of each level of nodes
    std::set<int> unique_node_types; // Vector storing all the unique types of nodes seen. Used to make sure the correct move proposals are made

    // ==========================================
    // Methods
    State_Dump get_state();                                    // Export current state of nodes in model
    void add_level(int); // Setup a new Node level
    LevelPtr get_level(int);
    NodePtr add_node(string, int, int);                                // Adds a node of specified id of a type and at a level
    NodePtr add_node(string, int);                                     // Adds a node of specified id of a type and at level 0
    NodePtr create_group_node(int, int);                               // Creates a new group node and adds it to its neccesary level
    NodePtr get_node_by_id(string, int);                               // Grabs and returns node of specified id, at desired level
    NodePtr get_node_by_id(string);                                    // Grabs and returns node of specified id, at first (level=0) level
    NodePtr get_node_from_level(int);                                  // Grabs the first node found at a given level, used in testing.
    list<NodePtr> get_nodes_from_level(int, int, bool);                // Return nodes of a desired type from level can be switched from matching or not ma
    list<NodePtr> get_nodes_of_type_at_level(int, int);                // Return nodes of a desired type from level can be switched from matching or not ma
    list<NodePtr> get_nodes_not_of_type_at_level(int, int);            // Return nodes node of a specified type from level
    void add_connection(string, string);                               // Adds a connection between two nodes based on their ids
    void add_connection(NodePtr, NodePtr);                             // Adds a connection between two nodes based on their pointers
    void give_every_node_at_level_own_group(int);                        // Builds and assigns a group node for every node in a given level
    int clean_empty_groups();                                          // Scan through levels and remove all group nodes that have no children. Returns # removed
    EdgeCounts gather_edge_counts(int);                                // Builds a id-id paired map of edge counts between nodes of the same level
    static void update_edge_counts(EdgeCounts &, int, NodePtr, NodePtr, NodePtr); // Update an EdgeCount map after moving a node around to avoid rescanning
    static string build_group_id(int, int, int);                                  // Builds a group id from a scaffold for generated new groups
};


struct State_Dump
{
  vector<string> id;
  vector<string> parent;
  vector<int> level;
  vector<int> type;
};


#endif