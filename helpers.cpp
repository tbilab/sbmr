#include "helpers.h"

// =======================================================
// Takes a vector of node ids and returns a single string of them pasted
// together
// =======================================================
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


// =======================================================
// Grab vector of node ids from a sequential container of nodes
// =======================================================

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


// ======================================================= 
// Helper to ensure desired id pair is always the same 
// regardless of passed order
// =======================================================
std::pair<string, string> find_edges(string a, string b) {
  return (a < b) ?
  std::pair<string, string>(a,b):
  std::pair<string, string>(b,a);
}

// When we're just looking for a single node's connections
std::pair<string, string> find_edges(string a) {
  return find_edges(a, a);
}

// Also make it work with pointers to nodes, which can be more natural 
std::pair<string, string> find_edges(NodePtr a, NodePtr b) {
  return find_edges(a->id, b->id);
}

std::pair<string, string> find_edges(NodePtr a) {
  return find_edges(a->id, a->id);
}


// Normalize a vector and return a new copy
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


// Compute a factorial
int factorial(int n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}
