#include <Rcpp.h>
#include <random>
#include "Node.h" 
#include "SBM.h" 

using namespace Rcpp;
using std::string;
using std::vector;
using std::map;

typedef vector<Node*> NodeList;
typedef vector<NodeList> NodeMap;


typedef vector<int> IntList;
typedef vector<IntList> IntMap;

// =======================================================
// Constructor that takes the nodes unique id integer and type
// =======================================================
SBM::SBM(){
  // Kick of node vector with empty array
  nodes.push_back(*(new NodeList()));
}

// =======================================================
// Grabs and returns node of specified id, if node doesn't exist, node is created first
// =======================================================
Node* SBM::get_node_by_id(string desired_id, int node_type){
  NodeList::iterator node_it;
  Node*              desired_node;
  bool               node_missing;
  
  // Start by assuming we couldn't find desired node
  node_missing = true;
  
  // If we have nodes already loaded, check requested node them
  if (!nodes.at(0).empty()) {
    // Search for node in level zero of the node data
    for (node_it = nodes.at(0).begin(); node_it != nodes.at(0).end(); ++node_it) {
      if ((*node_it)->id == desired_id) {
        desired_node = *node_it;
        node_missing = false;
        break;
      }
    }
  }
  
  if (node_missing) {
    // Create node
    desired_node = new Node(desired_id, 0, node_type);
    
    // Add node to node list
    nodes.at(0).push_back(desired_node);
  }
  
  return desired_node;
}         

// [[Rcpp::export]]
List load_data(vector<string> edges_a, vector<string> edges_b){
  SBM my_SBM;
  int n_edges = edges_a.size();
  
  return List::create(
    _["n_edges"] = n_edges
  );
}

// [[Rcpp::export]]
List setup_SBM(){
  SBM my_SBM;
  
  //IntMap test_vec;
  //test_vec.push_back(new IntList);
  
  //test_vec.at(0).push_back(1);
  
  // Add some nodes to SBM
  my_SBM.get_node_by_id("n1", 0);
  my_SBM.get_node_by_id("n2", 0);
  my_SBM.get_node_by_id("n3", 0);
  my_SBM.get_node_by_id("m1", 0);
  my_SBM.get_node_by_id("m2", 0);
  my_SBM.get_node_by_id("m3", 0);

  
  return List::create(
    _["test"]         = "I am working",
    _["has nodes"]    = my_SBM.nodes.empty() ? "empty" : "has nodes",
    _["num_nodes"] = my_SBM.nodes[0].size()
  );
}




// =======================================================

// =======================================================


/*** R
data <- readr::read_csv('southern_women.csv', col_types = readr::cols(event = 'c', individual = 'c'))
load_data(data$event, data$individual)

setup_SBM()
*/
