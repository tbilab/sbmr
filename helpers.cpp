//=================================
// include guard
//=================================

// #include <Rcpp.h>
#include "Node.h" 
#include <map>
#include <string>
#include <vector>

// using namespace Rcpp;
using std::string;
using std::vector;
using std::map;

// =======================================================
// Print vector of node ids, for debugging
// =======================================================
template <class container> 
string print_node_ids(container nodes) {
  // Template works for iteratable types like vector and sets
  string                  node_ids;
  typename container::iterator node_it;
  
  for (node_it = nodes.begin(); node_it != nodes.end(); ++node_it) {
    // Append node id to return string.
    node_ids.append((*node_it)->id + ", ");
  }
  
  // Remove last comma for cleanliness
  node_ids.erase(node_ids.end() - 2, node_ids.end());
  
  return node_ids;
}

