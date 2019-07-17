#include <Rcpp.h>
#include <random>
#include "Node.h" 
#include "BiGraph.h" 

using namespace Rcpp;
using std::string;
using std::vector;
using std::map;

typedef map<string, Node> NodeMap;
typedef map<string, Edge> EdgeMap;

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
  NodeMap*            map_for_type_ptr;    // Pointer to the correct map for type
  NodeMap::iterator   node_to_delete;      // Iterator to loop through node map
  bool                node_exists;         // Does the node exist in our graph?
  EdgeMap             node_edges;          // Edges for deleted node
  EdgeMap::iterator   node_edges_it;       // Iterator for those edges
  Node *              node_to_delete_ptr;  // Reference to node to be deleted
  Node *              neighbor_node_ptr;   // Reference for neighbor of node
  
  // Grab correct node map
  map_for_type_ptr = type_a ? &a_nodes : &b_nodes;
    
  // Try and find the node.
  node_to_delete = map_for_type_ptr->find(name);
  
  // Does this node exist in our graph?
  node_exists = node_to_delete != map_for_type_ptr->end();
  
  // If node exists, remove it.
  if(node_exists){
    // Grab reference to the actual node to delete itself
    node_to_delete_ptr = &(node_to_delete->second);
    
    // Remove it as an edge from all its neighbors
    node_edges = node_to_delete->second.edges;
    
    // Loop through and remove node as member from all its neighbors
    for(node_edges_it = node_edges.begin(); node_edges_it != node_edges.end(); node_edges_it++){
      
      // Grab reference to neighbor node 
      neighbor_node_ptr = node_edges_it->second.node;
      
      // Delete all edges to node from the neighbor
      neighbor_node_ptr->remove_edge(node_to_delete_ptr, true);
    }
    
    // Erase node from node map
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
// Add an edge between a pair of node names
//=================================
void BiGraph::add_edge(string a_name, string b_name){
  // Grab pointers to the two nodes in edge or add them
  // if they don't exist
  Node* a_node = get_node_by_id(a_name, true);
  Node* b_node = get_node_by_id(b_name, false);
  
  // Connect the two nodes with an edge
  Node::connect_nodes(a_node, b_node);
}


//=================================
// Methods still needed to be filled out
//=================================
// Node*                                 BiGraph::draw_random_node(string, bool);
// void                                  BiGraph::cleanup_empty_clusters();       // Removes nodes with no members
// vector< tuple<string, string> >       BiGraph::take_cluster_snapshot();
// vector< tuple<string, string, int> >  BiGraph::take_connection_snapshot();



// [[Rcpp::export]]
List load_data(vector<string> edges_a, vector<string> edges_b){
  BiGraph my_bigraph;
  int n_edges = edges_a.size();

  for(int i = 0; i < n_edges; i++){
    my_bigraph.add_edge(edges_a[i], edges_b[i]);
  }

  return List::create(
    _["n_a_nodes"] = my_bigraph.a_nodes.size(),
    _["n_b_nodes"] = my_bigraph.b_nodes.size()
  );
}



/*** R
data <- readr::read_csv('southern_women.csv', col_types = readr::cols(event = 'c', individual = 'c')) 
load_data(data$event, data$individual)
*/
