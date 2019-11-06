#include "helpers.h"

// =======================================================
// Takes a vector of node ids and returns a single string of them pasted together
// =======================================================
std::string print_ids_to_string(std::vector<std::string> node_ids) {
  std::vector<std::string>::iterator node_id_it;
  std::string                        node_id_string;
  
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


// =======================================================
// Grab vector of node ids from a sequential container of nodes
// =======================================================

// =======================================================
// List Version
std::string print_node_ids(std::list<NodePtr> nodes) {
  // Template works for iteratable types like vector and sets
  std::vector<std::string>         node_ids;
  std::list<NodePtr>::iterator       node_it;
  
  // Add node ids to containing vector
  for (node_it = nodes.begin(); node_it != nodes.end(); ++node_it) {
    // Append node id to return string.
    node_ids.push_back((*node_it)->id);
  }
  
  return print_ids_to_string(node_ids);
}

// =======================================================
// Vector Version
std::string print_node_ids(std::vector<NodePtr> nodes) {
  // Template works for iteratable types like vector and sets
  std::vector<std::string>           node_ids;
  std::vector<NodePtr>::iterator       node_it;
  
  // Add node ids to containing vector
  for (node_it = nodes.begin(); node_it != nodes.end(); ++node_it) {
    // Append node id to return string.
    node_ids.push_back((*node_it)->id);
  }
  
  return print_ids_to_string(node_ids);
}

// =======================================================
// Map version
std::string print_node_ids(std::map<std::string, NodePtr> nodes) {
  std::vector<std::string>                 node_ids;
  std::map<std::string, NodePtr>::iterator   node_it;
  
  // Add node ids to containing vector
  for (node_it = nodes.begin(); node_it != nodes.end(); ++node_it) {
    // Append node id to return string.
    node_ids.push_back(node_it->second->id);
  }
  
  return print_ids_to_string(node_ids);
}

