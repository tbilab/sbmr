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
void Node::build_counts_to_clusters() {
  
  // Start by flushing counts to clusters map incase it was previously set
  counts_to_clusters.clear();
  vector<Node*> nodes_to_scan;
  vector<Node*>::iterator connection_it;
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
    counts_to_clusters[connected_cluster] += 1;
  }
  
}


// =======================================================
// Print cluster connections, for debugging
// =======================================================
string Node::print_counts_to_clusters(){
  string all_connections;
  map<Node*, int>::iterator connection_it;
  string cluster_id;
  int connection_count;
  
  for(connection_it = counts_to_clusters.begin(); connection_it != counts_to_clusters.end(); ++connection_it) {
    cluster_id = (connection_it->first)->id;
    connection_count = connection_it->second;
    all_connections.append(cluster_id + ":" + std::to_string(connection_count) + ",");
  }
  
  return all_connections;
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
  Node n1("n1", false),
       n2("n2", false),
       n3("n3", false),
       c1("c1", true),
       c2("c2", true);
  
  n1.set_cluster(&c1);
  n2.set_cluster(&c1);
  n3.set_cluster(&c2);
  
  Node::connect_nodes(&n1, &n2);
  Node::connect_nodes(&n1, &n3);
  
  n1.build_counts_to_clusters();
  
  return List::create(
    _["id"]                  = n1.id,
    _["n1_cluster"]          = n1.cluster->id,
    _["n2_cluster"]          = n2.cluster->id,
    _["num_edges"]           = n1.connections.size(),
    _["n_connected_clusters"]= n1.counts_to_clusters.size(),
    _["cluster_connections"] = n1.print_counts_to_clusters()
  );
}



/*** R
make_node_and_print()
*/
