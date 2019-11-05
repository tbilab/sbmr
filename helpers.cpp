#include <string>
#include <algorithm>    // std::sort
#include <vector>

// =======================================================
// Print vector of node ids, for debugging
// Sorts contents so we can test non-order guarenteed containers
// =======================================================
template <class container> 
std::string print_node_ids(container nodes) {
  // Template works for iteratable types like vector and sets
  std::vector<std::string>           node_ids;
  std::vector<std::string>::iterator node_id_it;
  std::string                        node_id_string;
  typename container::iterator node_it;
  
  // Add node ids to containing vector
  for (node_it = nodes.begin(); node_it != nodes.end(); ++node_it) {
    // Append node id to return string.
    node_ids.push_back((*node_it)->id);
  }
  
  // Sort vector of id strings
  std::sort(node_ids.begin(), node_ids.end());

  // Dump vector of id strings to one big string
  for (node_id_it = node_ids.begin(); node_id_it != node_ids.end(); ++node_id_it) {
    // Append node id to return string.
    node_id_string.append(*node_id_it + ", ");
  }
  
  // Remove last comma for cleanliness
  node_id_string.erase(node_id_string.end() - 2, node_id_string.end());
  
  return node_id_string;
}
