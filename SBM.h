// [[Rcpp::plugins(cpp11)]]
//=================================
// include guard
//=================================
#ifndef __SBM_INCLUDED__
#define __SBM_INCLUDED__

#include "Node.h" 
using std::map;

typedef vector<Node*> NodeList;
typedef vector<NodeList> NodeMap;


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
    NodeMap   nodes;                           // Map keyed by level of vectors of nodes

    // ==========================================
    // Methods
    NodeList*  get_node_level(int);            // Grabs reference to nodes at desired level, if level doesn't exist, it makes it
    NodeList   get_nodes_of_type_at_level(int, int);    // Return nodes of a desired type from level
    Node*      create_group_node(int, int);    // Creates a new group node and adds it to its neccesary level
    int        clean_empty_groups();           // Parses through network and removes all empty groups from network, returns num culled
    Node*      get_node_by_id(string, int);    // Grabs and returns node of specified id, if node doesn't exist, node is created first
};

#endif