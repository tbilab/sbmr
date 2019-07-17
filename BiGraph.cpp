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
  
  // Pointer to the correct map for type
  NodeMap*            map_for_type_ptr = type_a ? &a_nodes : &b_nodes;
  NodeMap::iterator   node_to_delete;                                 // Itterator to loop through node map
  
  // Try and find the node.
  node_to_delete = map_for_type_ptr->find(name);
  
  // Is there already a connection to this node?
  bool node_exists = node_to_delete != map_for_type_ptr->end();
  
  // If node exists, remove it.
  if(node_exists){
    map_for_type_ptr->erase(node_to_delete);
  }
}


//=================================
// Get pointer to node 
// If node doesn't exist it is created
//=================================
Node* BiGraph::get_node_by_id(string name, bool type_a){
  NodeMap*            map_for_type_ptr;   // Pointer to the correct map for type
  NodeMap::iterator   node_to_find;       // Iterator for finding node
  Node *              node_to_return_ptr; // Pointer for node requested
  // Grab correct node map
  map_for_type_ptr = type_a ? &a_nodes : &b_nodes;
  
  // Try and find the node.
  node_to_find = map_for_type_ptr->find(name);
  
  // Is there already a connection to this node?
  bool node_doesnt_exist = node_to_find == map_for_type_ptr->end();
  
  if(node_doesnt_exist){
    // Insert node into map and assign newly inserted value
    // to our node finding iterator.
    node_to_find = map_for_type_ptr->insert(
        std::pair<string, Node>(
            name, 
            Node(name, type_a)
        )
    ).first; // .first is the the equivalent of map->find(...)'s results
  }
  
  // Return a pointer to the node
  return &node_to_find->second;
}




//=================================
// Methods to declare
//=================================


// void                                  BiGraph::add_edge(string, string);
// Node*                                 BiGraph::draw_random_node(string, bool);
// void                                  BiGraph::cleanup_empty_clusters();       // Removes nodes with no members
// vector< tuple<string, string> >       BiGraph::take_cluster_snapshot();
// vector< tuple<string, string, int> >  BiGraph::take_connection_snapshot();



// [[Rcpp::export]]
List load_data(vector<string> edges_a, vector<string> edges_b, bool delete_a_node){
  BiGraph my_bigraph;
  
  my_bigraph.get_node_by_id("node_1", true);
  my_bigraph.add_node("node_2", true);
  my_bigraph.add_node("node_3", false);
  my_bigraph.get_node_by_id("node_1", true);
  
  
  if(delete_a_node){
    my_bigraph.remove_node("node_1", true);
  }
  
  return List::create(
    _["n_nodes"]                 = my_bigraph.a_nodes.size()
  );
}



/*** R
data <- readr::read_csv('southern_women.csv', col_types = readr::cols(event = 'c', individual = 'c')) 
load_data(data$event, data$individual, FALSE)
load_data(data$event, data$individual, TRUE)
*/
