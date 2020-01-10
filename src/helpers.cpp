#include "helpers.h"

// ============================================================================
// Takes a vector of node ids and returns a string of them pasted together
// ============================================================================
inline std::string print_ids_to_string(std::vector<std::string> node_ids) {
  
  // Sort vector of id strings
  std::sort(node_ids.begin(), node_ids.end());
  
  // Final string that will be filled in
  std::string node_id_string;

  // A rough guess at how big the result will be
  node_id_string.reserve(node_ids.size() * 5);
  
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




// Build shuffled node vector
void shuffle_nodes(std::vector<NodePtr> & node_vec,
                   const std::shared_ptr<std::map<string, NodePtr>> & node_map,
                   std::mt19937 & sampler)
{
  // Initialize vector size to hold nodes
  node_vec.clear();

  // Fill in vector with map elements
  for (auto node_it = node_map->begin();
       node_it != node_map->end();
       node_it++)
  {
    node_vec.push_back(node_it->second);
  }

  // Shuffle node order
  std::shuffle(node_vec.begin(), node_vec.end(), sampler);
}

