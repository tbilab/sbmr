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



// Helper to build alphabetically string pair of two node ids for pair maps
std::string make_pair_key(const std::string a_node, const std::string b_node)
{
  return a_node > b_node
             ? a_node + "--" + b_node
             : b_node + "--" + a_node;
}

// Build shuffled node vector
void shuffle_nodes(std::vector<NodePtr> & node_vec,
                   const std::shared_ptr<std::map<string, NodePtr>> & node_map,
                   std::mt19937 & sampler)
{
  // Initialize vector size to hold nodes
  node_vec.reserve(node_map->size());

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


// Update the set of pairs that need to be updated for a given sweep.
void update_changed_pairs(NodePtr curr_node,
                          ChildSet &old_connections,
                          ChildSet &new_connections,
                          std::unordered_set<std::string> &pair_moves)
{
  // Loop through all the nodes in the previous group node changes
  for (auto lost_pair_it = old_connections.begin();
       lost_pair_it != old_connections.end();
       lost_pair_it++)
  {
    pair_moves.insert(make_pair_key(curr_node->id, (*lost_pair_it)->id));
  }

  // Repeat for the new groups children
  for (auto new_pair_it = new_connections.begin();
       new_pair_it != new_connections.end();
       new_pair_it++)
  {
    // Make sure we don't add this node to itself.
    if ((*new_pair_it)->id != curr_node->id)
    {
      pair_moves.insert(make_pair_key(curr_node->id, (*new_pair_it)->id));
    }
  }
}