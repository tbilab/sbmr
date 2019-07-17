#include <Rcpp.h>
#include <random>
#include "Node.h" 
#include "BiGraph.h" 

using namespace Rcpp;
using std::string;
using std::vector;
using std::map;

typedef map<string, Node> NodeMap;
  
// =======================================================
// Constructor that takes the nodes unique id integer and type
// =======================================================
BiGraph::BiGraph(){
  // Nothing really needs doing
}

//=================================
// Add a node to the graph
//=================================
void BiGraph::add_node(string name, bool type_a){
  
  // Pointer to the correct map for type
  NodeMap* map_for_type_ptr = type_a ? &a_nodes : &b_nodes;
  
  map_for_type_ptr->insert(
    std::pair<string, Node>(
      name, 
      Node(name, type_a)
    )
  );
}


//=================================
// Remove node from graph
//=================================
void BiGraph::remove_node(string name, bool type_a){
  
  map<string, Node>            map_for_type = type_a ? a_nodes : b_nodes; // Node map for type
  map<string, Node>::iterator  node_to_delete;                            // Itterator to loop through node map
  
  // Try and find the node.
  node_to_delete = map_for_type.find(name);
  
  // Is there already a connection to this node?
  bool node_exists = node_to_delete != map_for_type.end();
  
  // If node exists, remove it.
  if(node_exists){
    map_for_type.erase(node_to_delete);
  }
  
}



//=================================
// Methods to declare
//=================================

// void                                  BiGraph::add_edge(string, string);
// Node*                                 BiGraph::get_node_by_id(string, bool);
// Node*                                 BiGraph::draw_random_node(string, bool);
// void                                  BiGraph::cleanup_empty_clusters();       // Removes nodes with no members
// vector< tuple<string, string> >       BiGraph::take_cluster_snapshot();
// vector< tuple<string, string, int> >  BiGraph::take_connection_snapshot();



// [[Rcpp::export]]
List load_data(vector<string> edges_a, vector<string> edges_b){
  BiGraph my_bigraph;
  
  my_bigraph.add_node("node_1", true);
  my_bigraph.add_node("node_2", true);
  my_bigraph.add_node("node_3", false);
  
  return List::create(
    _["n_nodes"]                 = my_bigraph.a_nodes.size()
  );
}



/*** R
data <- readr::read_csv('southern_women.csv', col_types = readr::cols(event = 'c', individual = 'c')) 
load_data(data$event, data$individual)

*/
