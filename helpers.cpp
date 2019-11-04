#include <string>

// =======================================================
// Print vector of node ids, for debugging
// =======================================================
template <class container> 
std::string print_node_ids(container nodes) {
  // Template works for iteratable types like vector and sets
  std::string                  node_ids;
  typename container::iterator node_it;
  
  for (node_it = nodes.begin(); node_it != nodes.end(); ++node_it) {
    // Append node id to return string.
    node_ids.append((*node_it)->id + ", ");
  }
  
  // Remove last comma for cleanliness
  node_ids.erase(node_ids.end() - 2, node_ids.end());
  
  return node_ids;
}

