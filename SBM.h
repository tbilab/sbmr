#ifndef __SBM_INCLUDED__
#define __SBM_INCLUDED__

#include <map>
#include "Node.h" 

// A map keyed by ID of nodes that live on a given level of the SBM
typedef std::map<string, Node*>  NodeLevel;

// A map keyed by level integer of each level of nodes 
typedef std::map<int, NodeLevel> LevelMap;


//=================================
// What this file declares
//=================================
//class SBM;


//=================================
// Main node class declaration
//=================================
class SBM {
  public:
    SBM(); 
    // ==========================================
    // Attributes
    LevelMap   nodes;                          // A map keyed by level integer of each level of nodes 

    // ==========================================
    // Methods
    void        add_level(int);                          // Setup a new Node level
    void        check_level_has_nodes(const NodeLevel&); // Validates that a given level has nodes and throws error if it doesn;t
    list<Node*> get_nodes_of_type_at_level(int, int);    // Return nodes of a desired type from level
    Node*       create_group_node(int, int);             // Creates a new group node and adds it to its neccesary level
    int         clean_empty_groups();                    // Parses through network and removes all empty groups from network, returns num culled
    Node*       get_node_by_id(string);                  // Grabs and returns node of specified id, if node doesn't exist, node is created first
    Node*       add_node(string, int);                   // Grabs and returns node of specified id, if node doesn't exist, node is created first
    void        add_connection(string, string);          // Adds a connection between two nodes based on their ids
    void        give_every_node_a_group_at_level(int);   // Builds and assigns a group node for every node in a given level
};

#endif