#include <string>
#include <vector>

#include "Node.h"
#include "SBM.h" 

using std::string;
using std::vector;

// A map keyed by ID of nodes that live on a given level of the SBM
typedef std::map<string, Node*>  NodeLevel;

// A map keyed by level integer of each level of nodes 
typedef std::map<int, NodeLevel> LevelMap;

// =======================================================
// Constructor that takes the nodes unique id integer and type
// =======================================================
SBM::SBM(){
  // Setup first level of the node map
  add_level(0);
}

// =======================================================
// Setup a new Node level
// =======================================================
void SBM::add_level(int level) {
  
  // First, make sure level doesn't already exist
  if (nodes.find(level) != nodes.end()) {
    throw "Requested level to create already exists.";
  }
  
  // Setup first level of the node map
  nodes.emplace(level, *(new NodeLevel));
}


// =======================================================
// Find and return a node by its id
// =======================================================
Node* SBM::get_node_by_id(string desired_id) {
  
  try {
    // Attempt to find node on the 'node level' of the SBM
    return nodes.at(0).at(desired_id);
  } catch (...) {
    // Throw informative error if it fails
    throw "Could not find requested node";
  }
  
}


// =======================================================
// Adds a node with an id and type to network
// =======================================================
Node* SBM::add_node(string id, int type){
  Node* new_node;
  
  // Create node
  new_node = new Node(id, 0, type);
  
  // Add node to node list
  nodes.at(0).emplace(id, new_node);

  return new_node;
}; 


// =======================================================
// Validates that a given level has nodes and throws error if it doesn't
// =======================================================
void SBM::check_level_has_nodes(const NodeLevel& level_to_check){
  if (level_to_check.size() == 0) {
    throw "Requested level is empty.";
  }
};    


// =======================================================
// Return nodes of a desired type from level
// =======================================================
list<Node*> SBM::get_nodes_of_type_at_level(int type, int level) {
  list<Node*>           nodes_to_return;
  NodeLevel::iterator   node_it;
  NodeLevel             node_level;
  
  // Grab desired level reference
  node_level = nodes.at(level);
  
  // Make sure level has nodes before looping through it
  check_level_has_nodes(node_level);
  
  // Loop through every node belonging to the desired level
  for (node_it = node_level.begin(); node_it != node_level.end(); ++node_it) {
    
    // If the current node is of desired type...
    if(node_it->second->type == type) {
      
      // ...Place it in returning list
      nodes_to_return.push_back(node_it->second);
    }
  }
  
  return nodes_to_return;
}   


// =======================================================
// Creates a new group node and add it to its neccesary level
// =======================================================
Node* SBM::create_group_node(int type, int level) {
  
  LevelMap::iterator  group_level;
  int                 n_groups_in_level;
  string              group_id;
  Node*               new_group;
  bool                first_in_level;
  
  // Make sure requested level is not 0
  if(level == 0) throw "Can't create group node at first level";
  
  // Grab level for group node
  group_level = nodes.find(level);
  
  // Is this the very first element in this level? 
  first_in_level = group_level == nodes.end();
  
  if (first_in_level) {
    // Add a new node level 
    add_level(level);
    
    // 'find' that new level
    group_level = nodes.find(level);
  }
  
  // Find how many groups are already in the current level (all types)
  n_groups_in_level = group_level->second.size();
  
  // Build group_id
  group_id = std::to_string(type) + "-" + std::to_string(level) + "_" + std::to_string(n_groups_in_level);
  
  // Initialize new node
  new_group = new Node(group_id, level, type);
  
  // Add group node to SBM
  group_level->second.emplace(group_id, new_group);
  
  return new_group;
};

// =======================================================
// Adds a connection between two nodes based on their ids
// =======================================================
void SBM::add_connection(string node1_id, string node2_id) {

  Node::connect_nodes(
    this->get_node_by_id(node1_id), 
    this->get_node_by_id(node2_id)
  );
  
};       


// =======================================================
// Builds and assigns a group node for every node in a given level
// =======================================================
void SBM::give_every_node_a_group_at_level(int level) {
  NodeLevel            node_level;
  NodeLevel::iterator  node_it;
  Node*                new_group;
  
  // Grab all the nodes for the desired level
  node_level = nodes.at(level);
  
  // Make sure level has nodes before looping through it
  check_level_has_nodes(node_level);
  
  // Loop through each of the nodes,
  for (node_it = node_level.begin(); node_it != node_level.end(); ++node_it) {

    // build a group node at the next level
    new_group = create_group_node(node_it->second->type, level + 1);

    // assign that group node to the node
    node_it->second->set_parent(new_group);
  }
  
}      