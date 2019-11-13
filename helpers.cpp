#include "helpers.h"

// ============================================================================
// Takes a vector of node ids and returns a string of them pasted together
// ============================================================================
std::string print_ids_to_string(std::vector<std::string> node_ids) {
  
  // Sort vector of id strings
  std::sort(node_ids.begin(), node_ids.end());
  
  // Final string that will be filled in
  std::string node_id_string = "";
  
  // Dump vector of id strings to one big string
  for (auto node_id_it  = node_ids.begin(); 
            node_id_it != node_ids.end(); 
            ++node_id_it) 
  {
    // Append node id to return string.
    node_id_string.append(*node_id_it + ", ");
  }
  
  // Remove last comma for cleanliness
  node_id_string.erase(
    node_id_string.end() - 2, 
    node_id_string.end());
  
  return node_id_string;
}


// ============================================================================
// Grab vector of node ids from a sequential container of nodes
// ============================================================================

// =======================================================
// List Version
std::string print_node_ids(std::list<NodePtr> nodes) {
  // Vector of node ids to be filled in
  std::vector<std::string> node_ids;

  // Add node ids to containing vector
  for (auto node_it  = nodes.begin(); 
            node_it != nodes.end(); 
            ++node_it) 
  {
    // Append node id to return string.
    node_ids.push_back((*node_it)->id);
  }
  
  return print_ids_to_string(node_ids);
}

// =======================================================
// unordered set Version
std::string print_node_ids(std::unordered_set<NodePtr> nodes) {
  // Vector of node ids to be filled in
  std::vector<std::string> node_ids;
  
  // Add node ids to containing vector
  for (auto node_it  = nodes.begin(); 
       node_it != nodes.end(); 
       ++node_it) 
  {
    // Append node id to return string.
    node_ids.push_back((*node_it)->id);
  }
  
  return print_ids_to_string(node_ids);
}

// =======================================================
// Vector Version
std::string print_node_ids(std::vector<NodePtr> nodes) {
  // Vector of node ids to be filled in
  std::vector<std::string> node_ids;

  // Add node ids to containing vector
  for (auto node_it  = nodes.begin(); 
       node_it != nodes.end(); 
       ++node_it) 
  {
    // Append node id to return string.
    node_ids.push_back((*node_it)->id);
  }
  
  return print_ids_to_string(node_ids);
}

// =======================================================
// Map version
std::string print_node_ids(std::map<std::string, NodePtr> nodes) {
  // Vector of node ids to be filled in
  std::vector<std::string> node_ids;

  // Add node ids to containing vector
  for (auto node_it  = nodes.begin(); 
            node_it != nodes.end(); 
            ++node_it) 
  {
    // Append node id to return string.
    node_ids.push_back(node_it->second->id);
  }
  
  return print_ids_to_string(node_ids);
}


// ============================================================================
// Builds an alphabetical pair of node ids for looking up edges between groups.
// Insures order doesn't matter for EdgeCount key pair 
// ============================================================================
// std::pair<string, string> find_edges(string a, string b) {
//   return (a < b) ?
//   std::pair<string, string>(a,b):
//   std::pair<string, string>(b,a);
// }

// Also make it work with pointers to nodes, which can be more natural 
std::pair<NodePtr, NodePtr> find_edges(NodePtr a, NodePtr b) {
    return (a->id < b->id) ?
      std::pair<NodePtr, NodePtr>(a,b):
      std::pair<NodePtr, NodePtr>(b,a);
}


// // Single values return a double pair. e.g. find_edges('a') -> ['a','a']
// std::pair<string, string> find_edges(string a) {
//   return find_edges(a, a);
// }
// std::pair<string, string> find_edges(NodePtr a) {
//   return find_edges(a->id, a->id);
// }



// ============================================================================
// Normalizes a vector of doubles so all elements sum to 1 
// ============================================================================
std::vector<double> normalize_vector(std::vector<double> const &vec) 
{
  // Get sum of elements
  double weights_sum = 0.0;
  for (auto el = vec.begin(); el != vec.end(); ++el) 
  {
    weights_sum += *el;
  }
  
  std::vector<double> normalized_vec;
  normalized_vec.reserve(vec.size());
  
  for (auto el = vec.begin(); el != vec.end(); ++el)
  {
    normalized_vec.push_back(*el/weights_sum);
  }

  
  return normalized_vec;
}



// ============================================================================
// Compute's factorial of number by recursion
// ============================================================================
int factorial(int n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}
