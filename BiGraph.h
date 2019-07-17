// [[Rcpp::plugins(cpp11)]]
//=================================
// include guard
//=================================
#ifndef __BIGRAPH_INCLUDED__
#define __BIGRAPH_INCLUDED__

#include "Node.h" 
using std::string;
using std::tuple;
using std::vector;
using std::map;


//=================================
// What this file declares
//=================================
class BiGraph;


//=================================
// Main node class declaration
//=================================
class BiGraph {
  public:
    BiGraph(); 
    // ==========================================
    // Attributes
    map<string, Node> a_nodes;
    map<string, Node> b_nodes;
    
    // ==========================================
    // Methods
    void                                  add_node(string, bool);
    void                                  remove_node(string, bool);
    void                                  add_edge(string, string);
    Node*                                 get_node_by_id(string, bool);
    Node*                                 draw_random_node(string, bool);
    void                                  cleanup_empty_clusters();       // Removes nodes with no members
    vector< tuple<string, string> >       take_cluster_snapshot();
    vector< tuple<string, string, int> >  take_connection_snapshot();
};

#endif