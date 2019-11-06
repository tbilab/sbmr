#include "SBM.h" 

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
NodePtr SBM::get_node_by_id(string desired_id) {
  
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
NodePtr SBM::add_node(string id, int type){

  // Create node
  NodePtr new_node = std::make_shared<Node>(id, 0, type);

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
list<NodePtr> SBM::get_nodes_from_level(
    int type, 
    int level, 
    bool match_type) 
{
  // Where we will store all the nodes found from level
  list<NodePtr> nodes_to_return;

  // Grab desired level reference
  NodeLevel node_level = nodes.at(level);
  
  // Make sure level has nodes before looping through it
  check_level_has_nodes(node_level);
  
  // Loop through every node belonging to the desired level
  for (auto node_it  = node_level.begin(); 
            node_it != node_level.end(); 
            ++node_it) 
  {
    // Decide to keep the node or not based on if it matches or doesn't and our
    // keeping preferance
    bool keep_node = match_type ? 
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
list<NodePtr> SBM::get_nodes_of_type_at_level(int type, int level) {
  return get_nodes_from_level(type, level, true);
}   

// =======================================================
// Return nodes _not_ of a specified type from level
// =======================================================
list<NodePtr> SBM::get_nodes_not_of_type_at_level(int type, int level) {
  return get_nodes_from_level(type, level, false);
}   


// =======================================================
// Creates a new group node and add it to its neccesary level
// =======================================================
NodePtr SBM::create_group_node(int type, int level) {

  // Make sure requested level is not 0
  if(level == 0) {
    throw "Can't create group node at first level"; 
  }
  
  // Grab level for group node
  LevelMap::iterator group_level = nodes.find(level);
  
  // Is this the very first element in this level? 
  bool first_in_level = group_level == nodes.end();
  
  if (first_in_level) {
    // Add a new node level 
    add_level(level);
    
    // 'find' that new level
    group_level = nodes.find(level);
  }
  
  // Find how many groups are already in the current level (all types)
  int n_groups_in_level = group_level->second.size();
  
  // Build group_id
  string group_id = std::to_string(type)  + "-" + 
                    std::to_string(level) + "_" + 
                    std::to_string(n_groups_in_level);
  
  // Initialize new node
  NodePtr new_group = std::make_shared<Node>(group_id, level, type);
  
  // Add group node to SBM
  group_level->second.emplace(group_id, new_group);
  
  return new_group;
};

// =======================================================
// Adds a connection between two nodes based on their ids
// =======================================================
void SBM::add_connection(string node1_id, string node2_id) {
  
  Node::connect_nodes(
    get_node_by_id(node1_id), 
    get_node_by_id(node2_id)
  );
  
};    

void SBM::add_connection(NodePtr node1, NodePtr node2) {
  
  Node::connect_nodes(
    node1, 
    node2
  );
  
};  


// =======================================================
// Builds and assigns a group node for every node in a given level
// =======================================================
void SBM::give_every_node_a_group_at_level(int level) {

  // Grab all the nodes for the desired level
  NodeLevel node_level = nodes.at(level);
  
  // Make sure level has nodes before looping through it
  check_level_has_nodes(node_level);
  
  // Loop through each of the nodes,
  for (auto node_it  = node_level.begin(); 
            node_it != node_level.end(); 
            ++node_it) 
  {
    // build a group node at the next level
    NodePtr new_group = create_group_node(node_it->second->type, level + 1);

    // assign that group node to the node
    node_it->second->set_parent(new_group);
  }
  
}    


// =======================================================
// Grabs the first node found at a given level, used in testing.
// =======================================================
NodePtr SBM::get_node_from_level(int level) {
  return nodes.at(level).begin()->second;
}

// =======================================================
// Calculates probabilities for joining a given new group based on current SBM
// state
// =======================================================
Trans_Probs SBM::get_transition_probs_for_groups(NodePtr node_to_move) {
  // Ergodicity tuning parameter
  double epsilon = 0.01;

  // If we have a polypartite network we want to avoid that type when finding
  // neighbor nodes to look at. Otherwise we want to get all types, which we do
  // by supplying the 'null' type of -1.
  int type_to_ignore = unique_node_types.size() > 1 ? node_to_move->type : -1;

  // Grab all groups that could belong to connections
  list<NodePtr> neighboring_groups = get_nodes_not_of_type_at_level(
    type_to_ignore,
    node_to_move->level + 1 );

  // Map for precalculating the connections between node and all neighbor groups
  map<NodePtr, connection_info> node_outward_connections;

  // First we gather info on how the node to move is connected in terms of its
  // neighbors's groups
  for (auto neighbor_group_it  = neighboring_groups.begin();
            neighbor_group_it != neighboring_groups.end();
            ++neighbor_group_it)
  {
    // What proportion of this node's edges are to nodes in current group?
    node_outward_connections.emplace(
      *neighbor_group_it,
      node_to_move->connections_to_node(*neighbor_group_it)
    );
  }

  // Now loop through all the groups that the node could join
  list<NodePtr> potential_groups = get_nodes_of_type_at_level(
    type_to_ignore,
    node_to_move->level + 1);

  // Number of potential groups
  int B = potential_groups.size();
  
  // Initialize holder of transition probabilities
  vector<double> probabilities;
  probabilities.reserve(B);

  // Initialize holder of groups to match transition probs
  vector<NodePtr> groups;
  groups.reserve(B);

  // Start main loop over all the potential groups that the node could join
  for (auto potential_group_it  = potential_groups.begin();
            potential_group_it != potential_groups.end();
            ++potential_group_it)
  {
    NodePtr potential_group = *potential_group_it;

    // Send currently investigated group to groups vector
    groups.push_back(potential_group);

    // Start out sum at 0.
    double cummulative_prob = 0;

    // Loop over the neighbor groups again
    for (auto neighbor_group_it  = neighboring_groups.begin();
              neighbor_group_it != neighboring_groups.end();
              ++neighbor_group_it)
    {
      NodePtr neighbor_group = *neighbor_group_it;

      // Get connection info for the potential group to the neighbor group
      connection_info potential_to_neighbor_connections = neighbor_group->
        connections_to_node(potential_group);

      // Grab pre-calculated connection info from node to this neighbor
      connection_info node_to_neighbor_connections = node_outward_connections
        .at(neighbor_group);

      // Get fraction of the nodes connections to the current neighbor. This
      // serves as an indicator of how close we should consider the connections
      // of this neighbor node when deciding the new group.
      // double P_si = double(node_to_neighbor_connections.n_between) /
      //               double(node_to_neighbor_connections.n_total);      
      
      // We can just calculate this value instead of the prob because the denominator of the 
      // probability is the same for every iteration so we can just use the numerator and 
      // then the normalization of vector to 1 at end will take care of this scalar. 
      
      // How many connections does this node have to group of interest? 
      double e_si = node_to_neighbor_connections.n_between;
      
      // How many connections there are between our neighbor group and the
      // potential group
      double e_sr = potential_to_neighbor_connections.n_between;

      // How many total connection the neighbor node has
      double e_s = potential_to_neighbor_connections.n_total;

      // Finally calculate partial probability and add to cummulative sum
      cummulative_prob += e_si * ( (e_sr + epsilon) / (e_s + epsilon*(B + 1)) );
    }

    // Add the final cumulative probabiltiy sum to potential group's element in
    // probability vector
    probabilities.push_back(cummulative_prob);
  }
  
  // Normalize vector to sum to 1
  double total_of_probs = std::accumulate(probabilities.begin(), probabilities.end(), double(0));
  for (auto prob = probabilities.begin(); prob != probabilities.end(); ++prob)
  {
    *prob = *prob/total_of_probs;
  }
  
  return Trans_Probs(probabilities, groups);
}
