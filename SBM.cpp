#include <string>
#include <vector>

#include "Node.h"
#include "SBM.h" 

using std::string;
using std::vector;
using std::map;

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
  
  // Send node type to the types set
  unique_node_types.insert(type);
  
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
// Return nodes of a desired type from level. If match_type = true
// then the nodes returned are of the same type as specified, otherwise
// the nodes returned are _not_of the same type.
// =======================================================
list<Node*> SBM::get_nodes_from_level(int type, int level, bool match_type) {
  list<Node*>           nodes_to_return;
  NodeLevel::iterator   node_it;
  NodeLevel             node_level;
  bool                  keep_node; 
  
  // Grab desired level reference
  node_level = nodes.at(level);
  
  // Make sure level has nodes before looping through it
  check_level_has_nodes(node_level);
  
  // Loop through every node belonging to the desired level
  for (node_it = node_level.begin(); node_it != node_level.end(); ++node_it) {
    
    // Decide to keep the node or not based on if it matches or doesn't and our
    // keeping preferance
    keep_node = match_type ? 
    (node_it->second->type == type) : 
    (node_it->second->type != type);
    
    if(keep_node) {
      // ...Place it in returning list
      nodes_to_return.push_back(node_it->second);
    }
  }
  
  return nodes_to_return;
} 

// =======================================================
// Return nodes of a desired type from level. 
// =======================================================
list<Node*> SBM::get_nodes_of_type_at_level(int type, int level) {
  return get_nodes_from_level(type, level, true);
}   

// =======================================================
// Return nodes _not_ of a specified type from level
// =======================================================
list<Node*> SBM::get_nodes_not_of_type_at_level(int type, int level) {
  return get_nodes_from_level(type, level, false);
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


// =======================================================
// Grabs the first node found at a given level, used in testing.
// =======================================================
Node* SBM::get_node_from_level(int level) {
  return nodes.at(level).begin()->second;
}

// =======================================================
// Calculates probabilities for joining a given new group based on current SBM state
// =======================================================
Transition_Probs SBM::get_transition_probs_for_groups(Node* node_to_move) {
  list<Node*>                 neighboring_groups;        // Groups of connection to search over, will be all for unipartite, but a subset for polypartite networks
  list<Node*>::iterator       neighbor_group_it;         // For parsing through all groups to check
  Node*                       neighbor_group; 
  list<Node*>                 potential_groups;          // Groups that the node can join
  list<Node*>::iterator       potential_group_it;        // For parsing through all groups to check
  Node*                       potential_group;
  bool                        is_polypartite;            // Is this SBM polypartite
  int                         type_to_ignore;            // What type of nodes do we want to avoid when finding connections?
  double                      epsilon;                   // Ergodicity tuning parameter
  double                      cummulative_prob;          // Variable to accumulate probabilities over sum
  int                         B;        // Number of potential groups
  map<Node*, connection_info> node_outward_connections;  // Map for precalculating the connections between node and all neighbor groups
  connection_info             potential_to_neighbor_connections; 
  connection_info             node_to_neighbor_connections; 
  Transition_Probs            transition_probs;
  
  // Some variables that make the final calculation a bit clearer to understand
  // fraction node connections to neighbor
  double P_si;
  // number connections between neighbor and potential
  double e_sr;
  // total connections for neighbor
  double e_s;
  
  epsilon = 0.01; // This will eventually be passed to function

  // First check what type of network we're working with
  is_polypartite = unique_node_types.size() > 1;
  
  // If we have a polypartite network we want to avoid that type when finding
  // neighbor nodes to look at. Otherwise we want to get all types, which we do
  // by supplying the 'null' type of -1.
  type_to_ignore = is_polypartite ? node_to_move->type : -1;
  
  // Grab all groups that could belong to connections
  neighboring_groups = get_nodes_not_of_type_at_level(type_to_ignore, node_to_move->level + 1);
  
  // First we gather info on how the node to move is connected in terms of its
  // neighbors's groups
  for (neighbor_group_it = neighboring_groups.begin(); neighbor_group_it != neighboring_groups.end(); ++neighbor_group_it) {
    neighbor_group = *neighbor_group_it;
    
    // What proportion of this node's edges are to nodes in current group?
    node_outward_connections.emplace(
      neighbor_group,
      node_to_move->connections_to_node(neighbor_group)
    );
  }
  
  // Now loop through all the groups that the node could join
  potential_groups = get_nodes_of_type_at_level(type_to_ignore, node_to_move->level + 1);
  
  B = potential_groups.size();
  
  // Reserve proper number of elements for the two info holding vectors
  transition_probs.probability.reserve(B);
  transition_probs.group.reserve(B);
  
  // Start main loop over all the potential groups that the node could join
  for (potential_group_it = potential_groups.begin(); potential_group_it != potential_groups.end(); ++potential_group_it) {
    potential_group = *potential_group_it;
    
    // Send currently investigated group to groups vector
    transition_probs.group.push_back(potential_group);
    
    // Start out sum at 0.
    cummulative_prob = 0.0;
    
    // Loop over the neighbor groups again
    for (neighbor_group_it = neighboring_groups.begin(); neighbor_group_it != neighboring_groups.end(); ++neighbor_group_it) {
      neighbor_group = *neighbor_group_it;
      
      // Get connection info for the potential group to the neighbor group
      potential_to_neighbor_connections = neighbor_group->connections_to_node(potential_group);
      
      // Grab pre-calculated connection info from node to this neighbor
      node_to_neighbor_connections = node_outward_connections.at(neighbor_group);
      
      // Get fraction of the nodes connections to the current neighbor. This
      // serves as an indicator of how close we should consider the connections
      // of this neighbor node when deciding the new group.
      P_si = double(node_to_neighbor_connections.n_between) / double(node_to_neighbor_connections.n_total);
      
      // How many connections there are between our neighbor group and the potential group
      e_sr = double(potential_to_neighbor_connections.n_between);
      
      // How many total connection the neighbor node has
      e_s = double(potential_to_neighbor_connections.n_total);
      
      // Finally calculate partial probability and add to cummulative sum 
      cummulative_prob += P_si * (e_sr + epsilon) / (e_s + epsilon*(B + 1));
    }
    
    // Add the final cumulative probabiltiy sum to potential group's element in probability vector
    transition_probs.probability.push_back(cummulative_prob);
    
  }
  
  return transition_probs;
}
