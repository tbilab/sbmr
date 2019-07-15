// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>
#include <random>
#include "Node.h" 

using namespace Rcpp;
using std::string;
using std::vector;

// Setup random number generation
std::random_device rand_dev;
std::mt19937 generator(rand_dev());


// Constructor that takes the nodes unique id integer
Node::Node(int node_id){
  id = node_id;
}


// Add an edge to the node's edges array
void Node::set_edge(Node *ptr_to_neighbor){
  connections.push_back(ptr_to_neighbor);
  // Increment the nodes degree because it has a new edge
  degree++;
}

// Dump all the currently connected node ids to a vector and return
vector<int> Node::get_ids_of_connections(){
  vector<int> connected_ids(connections.size());
  
  for(int i = 0; i < connections.size(); i++){
    connected_ids[i] = connections[i]->id;
  }
  
  return connected_ids;
}

// Grab and return pointer to a random connection
Node * Node::random_neighbor(){
  std::uniform_int_distribution<int> distr(0, connections.size()-1);
  return connections[distr(generator)];
}

// Join two nodes together with an edge
void Node::join_nodes(Node * node_1_ptr, Node * node_2_ptr){
  node_1_ptr->set_edge(node_2_ptr);
  node_2_ptr->set_edge(node_1_ptr);
}



// [[Rcpp::export]]
int make_node_and_print(){
  Node my_node(4);
  return my_node.id;
}



// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically 
// run after the compilation.
//

/*** R
make_node_and_print()
*/
