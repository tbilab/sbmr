#include "SBM.h" 

// =======================================================
// Constructor that takes the nodes unique id integer and type
// =======================================================
SBM::SBM(){
  // Nothing needs doin'
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
  nodes.emplace(level, std::make_shared<NodeLevel>());
}

// =======================================================
// Grab reference to a desired level map. If level doesn't exist yet, this
// method will create it
// =======================================================
LevelPtr SBM::get_level(int level) {
  
  // Grab level for group node
  LevelMap::iterator group_level = nodes.find(level);

  // Is this a new level?
  bool level_doesnt_exist = group_level == nodes.end();

  if (level_doesnt_exist) {
    // Add a new node level
    add_level(level);

    // 'find' that new level
    group_level = nodes.find(level);
  }
  
  return group_level->second;
}


// =======================================================
// Find and return a node by its id
// =======================================================
NodePtr SBM::get_node_by_id(string desired_id) {
  
  try {
    // Attempt to find node on the 'node level' of the SBM
    return nodes.at(0)->at(desired_id);
  } catch (...) {
    // Throw informative error if it fails
    throw "Could not find requested node";
  }
  
}


// =======================================================
// Builds a group id from a scaffold for generated new groups
// =======================================================
string SBM::build_group_id(int type, int level, int index) {
  return std::to_string(type)  + "-" +
    std::to_string(level) + "_" +
    std::to_string(index);
}


// =======================================================
// Adds a node with an id and type to network
// =======================================================
NodePtr SBM::add_node(string id, int type, int level){
  
  // Grab level
  LevelPtr node_level = get_level(level);

  // Check if we need to make the id or not
  string node_id = id == "new group" ?
    build_group_id(type, level, node_level->size()):
    id;
  
  // Create node
  NodePtr new_node = std::make_shared<Node>(node_id, level, type);
  
  node_level->emplace(node_id, new_node);
 
  // Update node types set with new node's type
  unique_node_types.insert(type);
  
  return new_node;
}; 


// =======================================================
// Adds a node with an id and type to network at level 0
// =======================================================
NodePtr SBM::add_node(string id, int type){
  return add_node(id, type, 0);
}; 


// =======================================================
// Creates a new group node and add it to its neccesary level
// =======================================================
NodePtr SBM::create_group_node(int type, int level) {

  // Make sure requested level is not 0
  if(level == 0) {
    throw "Can't create group node at first level";
  }
  
  // Initialize new node
  return add_node("new group", type, level);
};


// =======================================================
// Validates that a given level has nodes and throws error if it doesn't
// =======================================================
void SBM::check_level_has_nodes(const LevelPtr level_to_check){
  if (level_to_check->size() == 0) {
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
  LevelPtr node_level = nodes.at(level);
  
  // Make sure level has nodes before looping through it
  check_level_has_nodes(node_level);
  
  // Loop through every node belonging to the desired level
  for (auto node_it  = node_level->begin(); 
            node_it != node_level->end(); 
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
// Adds a connection between two nodes based on their ids
// =======================================================
void SBM::add_connection(string node1_id, string node2_id) {
  
  Node::connect_nodes(
    get_node_by_id(node1_id), 
    get_node_by_id(node2_id)
  );
  
};    


// =======================================================
// Adds a connection between two nodes based on their references
// =======================================================
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
  LevelPtr node_level = nodes.at(level);
  
  // Make sure level has nodes before looping through it
  check_level_has_nodes(node_level);
  
  // Loop through each of the nodes,
  for (auto node_it  = node_level->begin(); 
            node_it != node_level->end(); 
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
  return nodes.at(level)->begin()->second;
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


// =======================================================
// Scan through levels and remove all group nodes that have no children. Returns
// the number removed
// =======================================================
int SBM::clean_empty_groups(){
  
  int num_levels = nodes.size();
  int total_deleted = 0;
  
  // Scan through all levels up to final
  for (int level = 1; level < num_levels; ++level) 
  {
    // Grab desired level
    LevelPtr group_level = nodes.at(level);
    
    // Create a vector to store group ids that we want to delete
    vector<string> groups_to_delete;
    
    // Loop through every node at level
    for (auto group_it = group_level->begin(); group_it != group_level->end(); ++group_it)
    {
      NodePtr current_group = group_it->second;
      
      // If there are no children for the current group
      if (current_group->children.size() == 0) 
      {
        // Remove group from children of its parent (if it has one)
        if (level < num_levels - 1) 
        {
          current_group->parent->remove_child(current_group);
        }
        
        // Add current group to the removal list
        groups_to_delete.push_back(current_group->id);
      }
    }
    
    // Remove all the groups in the removal list
    for (auto group_id : groups_to_delete)
    {
      group_level->erase(group_id);
    }
    
    // Increment total groups deleted counter
    total_deleted += groups_to_delete.size();
  }
  
  return total_deleted;
}                     


// ======================================================= 
// Helper to ensure desired id pair is always the same 
// regardless of passed order
// =======================================================
std::pair<string, string> id_pair(string a, string b) {
  return (a < b) ?
  std::pair<string, string>(a,b):
  std::pair<string, string>(b,a);
}


// ======================================================= 
// Builds a id-id paired map of edge counts between nodes of the same level
// =======================================================
EdgeCounts SBM::gather_edge_counts(int level){
  
  // Setup our edge count map: 
  EdgeCounts e_rs;
  
  // Grab current level
  LevelPtr node_level = nodes.at(level);
  
  // Loop through all groups (r)
  for (auto group_it = node_level->begin(); group_it != node_level->end(); ++group_it) 
  {
    NodePtr group_r = group_it->second;
    string group_r_id = group_it->first;
    
    // Get all the edges for group r to its level
    vector<NodePtr> group_r_cons = group_r->get_connections_to_level(level);
    
    // Set total number of edges for group r
    e_rs[id_pair(group_r_id, group_r_id)] = group_r_cons.size();
    
    // Loop over all edges
    for (auto group_s = group_r_cons.begin(); group_s != group_r_cons.end(); ++group_s) 
    {
      // Add connection counts to the map
      e_rs[id_pair(group_r_id, (*group_s)->id)]++;
    }
    
  } // end group r loop
  
  // All the off-diagonal elements will be doubled because they were added for
  // r->s and s->r, so divide them by two
  for (auto node_pair = e_rs.begin(); node_pair != e_rs.end(); ++node_pair)
  {
    // Make sure we're not on a diagonal
    if (node_pair->first.first != node_pair->first.second)
    {
      node_pair->second /= 2;
    }
  }
  
  return e_rs;
}   


// ======================================================= 
// Builds a id-id paired map of edge counts between nodes of the same level
// =======================================================
void SBM::update_edge_counts(
    EdgeCounts& level_counts, 
    int         level, 
    NodePtr     updated_node, 
    NodePtr     old_group, 
    NodePtr     new_group) 
{
  // Get ids of groups moved, at the level of the move
  string old_group_id = old_group->get_parent_at_level(level)->id;
  string new_group_id = new_group->get_parent_at_level(level)->id;
  
  // Gather all connections from the moved node to the level of the groups we're
  // working with
  vector<NodePtr> moved_connections = updated_node->get_connections_to_level(level);
  
  // Setup an edge count map for node
  std::map<string, int> moved_connections_counts;
  
  // Fill out edge count map
  for(auto moved_connection  = moved_connections.begin(); 
           moved_connection != moved_connections.end();
           ++moved_connection )
  {
    moved_connections_counts[(*moved_connection)->id]++;
  }
  
  // Now we can loop through all the groups that the moved node was connected to
  // and subtract their counts from the from-group's edges and add their counts
  // to the to-group's
  // Fill out edge count map
  for(auto changed_group  = moved_connections_counts.begin(); 
           changed_group != moved_connections_counts.end();
           ++changed_group )
  {
    string changed_id = changed_group->first;
    int amount_changed = changed_group->second;
    
    // Subtract from old group...
    level_counts[id_pair(changed_id, old_group_id)] -= amount_changed;
    
    // ...Add to new group
    level_counts[id_pair(changed_id, new_group_id)] += amount_changed;
  }
  
  // Finally update the total counts for the old and new group
  int total_moved = moved_connections.size();
  level_counts[id_pair(old_group_id, old_group_id)] -= total_moved;
  level_counts[id_pair(new_group_id, new_group_id)] += total_moved;
}

