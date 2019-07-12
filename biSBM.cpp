#include <Rcpp.h>
#include <random>
using namespace Rcpp;
using std::string;
using std::vector;

// [[Rcpp::plugins(cpp11)]]

// This is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp 
// function (or via the Source button on the editor toolbar). Learn
// more about Rcpp at:
//
//   http://www.rcpp.org/
//   http://adv-r.had.co.nz/Rcpp.html
//   http://gallery.rcpp.org/
//

class Node {
private: 

public:
  Node(int);
  int id;
  bool is_type_a;
  vector<Node *> connections;
  void set_edge(Node *);
  vector<int> get_ids_of_connections();
  Node * random_neighbor();
};

Node::Node(int node_id){
  id = node_id;
}

// Add an edge to the node's edges array
void Node::set_edge(Node *ptr_to_neighbor){
  connections.push_back(ptr_to_neighbor);
}

vector<int> Node::get_ids_of_connections(){
  vector<int> connected_ids(connections.size());
  
  for(int i = 0; i < connections.size(); i++){
    connected_ids[i] = connections[i]->id;
  }
  
  return connected_ids;
}

Node * Node::random_neighbor(){
  
  std::random_device rand_dev;
  std::mt19937 generator(rand_dev());
  std::uniform_int_distribution<int>  distr(0, connections.size());
  
  return connections[distr(generator)];
}

// [[Rcpp::export]]
int make_and_return_node(int node_int){
  Node node_a(node_int), 
       node_b(5),
       node_c(2);
  
  node_a.set_edge(&node_b);
  node_a.set_edge(&node_c);
  
  return node_a.random_neighbor()->id;
}


// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically 
// run after the compilation.
//

/*** R
make_and_return_node(34)
*/
