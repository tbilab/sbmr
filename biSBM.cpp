// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>
#include <random>
using namespace Rcpp;
using std::string;
using std::vector;

// Setup random number generation
std::random_device rand_dev;
std::mt19937 generator(rand_dev());

// Pre-declare class types
class Node;
class Cluster;

class Node {
private: 
  vector<Node *> connections;

public:
  int            id;
  bool           is_type_a;
  int            degree;
  Cluster *      cluster;
  Node(int);   
  void           set_edge(Node *);
  vector<int>    get_ids_of_connections();
  Node *         random_neighbor();
  static void join_nodes(Node *, Node *);
};

Node::Node(int node_id){
  id = node_id;
}

// Add an edge to the node's edges array
void Node::set_edge(Node *ptr_to_neighbor){
  connections.push_back(ptr_to_neighbor);
  degree++;
}

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
int make_and_return_node(int node_int){
  Node node_a(node_int), 
       node_b(5),
       node_c(2);
  
  Node::join_nodes(&node_a, &node_b);
  
  return node_b.random_neighbor()->id;
}

class Cluster: public Node {
public:
  vector<Node *> members;
  void add_member(Node *);
  void remove_member(Node *);
  int num_edges_to_node(Node *);
  int num_edges_to_cluster(Cluster *);
};


// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically 
// run after the compilation.
//

/*** R
make_and_return_node(34)
*/
