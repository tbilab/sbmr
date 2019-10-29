// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>
#include <random>
#include "Node.h" 

using namespace Rcpp;
using std::string;
using std::vector;
using std::map;


// =======================================================
// Constructor that takes the nodes unique id integer and type
// =======================================================
Node::Node(string node_id, bool node_is_cluster):
  id(node_id),
  is_cluster(node_is_cluster){}


// =======================================================
// Add connection to another node
// =======================================================
void Node::add_connection(Node* node_ptr) {
  // Add element to connections array
  connections.push_back(node_ptr);
}          


// =======================================================
// Swap current cluster with a new one
// =======================================================
void Node::set_cluster(Node* cluster_node_ptr) {
  cluster = cluster_node_ptr;
}


// =======================================================
// Static method to connect two nodes to each other with edge
// =======================================================
void Node::connect_nodes(Node* node1_ptr, Node* node2_ptr) {
  // Add node2 to connections of node1
  node1_ptr->add_connection(node2_ptr);
  // Do the same for node2
  node2_ptr->add_connection(node1_ptr);
}


// [[Rcpp::export]]
List make_node_and_print( ) {
  Node node1("n1", false),
       node2("n2", false),
       node3("n3", false);

  Node::connect_nodes(&node1, &node2);
  Node::connect_nodes(&node1, &node3);
  
  return List::create(
    _["id"]                 = node1.id,
    _["num_edges"]          = node1.connections.size()
  );
  
}



/*** R
make_node_and_print()
*/
