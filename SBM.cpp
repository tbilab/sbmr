#include <string>
#include <vector>

#include "Node.h"
#include "SBM.h" 

using std::string;
using std::vector;

typedef vector<Node*> NodeList;
typedef vector<NodeList> NodeMap;

// =======================================================
// Constructor that takes the nodes unique id integer and type
// =======================================================
SBM::SBM(){
  // Nothing needs doin'
}


// =======================================================
// Grabs nodes from desired level, if level doesn't exist, it makes it
// =======================================================
NodeList* SBM::get_node_level(int level) {
  
  // If desired level is missing, create it
  if (nodes.size() < (level + 1)) {
    
    // Make sure that the requested level isn't too far away
    if (nodes.size() < level) throw "Requested unavailable level";
    
    // Create a new node list and insert into node map
    nodes.push_back(*(new NodeList()));
  }
  
  return &nodes.at(level);
};            

// =======================================================
// Find and return a node by its id
// =======================================================
Node* SBM::find_node_by_id(string desired_id) {
  NodeList::iterator  node_it;
  NodeList*           node_level;
  bool                node_missing;
  Node*               desired_node;
  
  // Grab the bottom "data" level of nodes
  node_level = get_node_level(0);
  
  // Search for node in level zero of the node data
  for (node_it = node_level->begin(); node_it != node_level->end(); ++node_it) {
    
    // Check if our current node id matches the desired id
    if ((*node_it)->id == desired_id) {
      
      // If it matches, assign it as desired node
      desired_node = *node_it;
      
      // Let method know we found node
      node_missing = false;
      
      // No need to continue since we found node
      break;
    }
    
  }
  
  if (node_missing) throw "Could not find requested node";
  
  return desired_node;
}


// =======================================================
// Adds a node with an id and type to network
// =======================================================
Node* SBM::add_node(string id, int type){
  Node* new_node;
  
  // Create node
  new_node = new Node(id, 0, type);
  
  // Add node to node list
  get_node_level(0)->push_back(new_node);
  
  return new_node;
}; 


// =======================================================
// Return nodes of a desired type from level
// =======================================================
NodeList SBM::get_nodes_of_type_at_level(int type, int level) {
  NodeList            nodes_to_return;
  NodeList::iterator  node_it;
  NodeList*           node_level;
  
  // Grab desired level reference
  node_level = get_node_level(level);
  
  // Loop through every node belonging to the desired level
  for (node_it = node_level->begin(); node_it != node_level->end(); ++node_it) {
    
    // If the current node is of desired type...
    if((*node_it)->type == type) {
      
      // ...Place it in returning vector
      nodes_to_return.push_back(*node_it);
    }
  }
  
  return nodes_to_return;
}   


// =======================================================
// Creates a new group node and adds it to its neccesary level
// =======================================================
Node* SBM::create_group_node(int type, int level) {
  
  NodeList*  group_level;
  int        n_groups_in_level;
  string     group_id;
  Node*      new_group;
  
  // Make sure requested level is not 0
  if(level == 0) throw "Can't create group node at first level";
  
  // Grab level for group node
  group_level = get_node_level(level);
  
  // Find how many groups are already in the current level (all types)
  n_groups_in_level = group_level->size();
  
  // Build group_id
  group_id = std::to_string(type) + "-" + std::to_string(level) + "_" + std::to_string(n_groups_in_level);
  
  // Initialize new node
  new_group = new Node(group_id, level, type);
  
  // Add group node to SBM
  group_level->push_back(new_group);
  
  return new_group;
};

// =======================================================
// Adds a connection between two nodes based on their ids
// =======================================================
void SBM::add_connection(string node1_id, string node2_id){
  Node* node1;
  Node* node2;
  
  // node1 = this->get_node_by_id(node1_id);
  // node2 = this->get_node_by_id(node2_id);
  // 
  // Node::connect_nodes(Node* node1, Node* node2);
};       
