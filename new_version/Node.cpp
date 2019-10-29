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
// Build a map of cluster -> # connections for current node
// =======================================================

map<Node*, int> Node::build_counts_to_clusters() {
  
  vector<Node*> nodes_to_scan;
  vector<Node*>::iterator connection_it;
  map<Node*, int> cluster_connections; 
  Node* connected_cluster;
  
  // If Node is a cluster, gather nodes to scan from its members
  if(is_cluster) {
    vector<Node*>::iterator member_it; // Iterator for member nodes
    vector<Node*> member_connections; // Pointer to member's connections vector
    
    // First loop over each member of this cluster
    for(member_it = members.begin(); member_it != members.end(); ++member_it){
      // Grab pointer to member connections vector. First we de-reference the
      // pointer to each member, which is itself a pointer to a node. We then
      // enter the node itself to get its connections with ->
      member_connections = (*member_it)->connections; 
      
      // Loop over the member connections vector
      for(connection_it = member_connections.begin(); connection_it != member_connections.end(); ++connection_it) {
        // Convert the current connected node to a real pointer from iterator
        // and send to the nodes-to-scan vector
        nodes_to_scan.push_back(*connection_it);
      }
    } // end member loop
  } else {
    // If the current node is just a normal node we don't need to run into its
    // members before looping over connections
    for(connection_it = connections.begin(); connection_it != connections.end(); ++connection_it) {
      nodes_to_scan.push_back(*connection_it);
    }
  } // end else statement
  
  // Finally, loop over all the connections and record the cluster membership
  for(connection_it = nodes_to_scan.begin(); connection_it != nodes_to_scan.end(); ++connection_it) {
    connected_cluster = (*connection_it)->cluster;
    cluster_connections[connected_cluster] += 1;
  }
  
  return cluster_connections;
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
