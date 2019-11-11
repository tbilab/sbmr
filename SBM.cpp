#include "SBM.h" 

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

Trans_Probs SBM::get_transition_probs_for_groups(NodePtr node_to_move, EdgeCounts group_edge_counts) 
{
  // Ergodicity tuning parameter
  double epsilon = 0.01;
  
  int group_level = node_to_move->level + 1;
  
  // If we have a polypartite network we want to avoid that type when finding
  // neighbor nodes to look at. Otherwise we want to get all types, which we do
  // by supplying the 'null' type of -1.
  int type_to_ignore = unique_node_types.size() > 1 ? node_to_move->type : -1;
  
  // Grab all groups that could belong to connections
  list<NodePtr> neighboring_groups = get_nodes_not_of_type_at_level(
    type_to_ignore,
    group_level);
  
  // Map for precalculating the connections between node and all neighbor groups
  //map<NodePtr, connection_info> node_outward_connections;
  map<string, int> node_outward_connections;
  
  // Gather all connections node has to the next level up (its group level)
  vector<NodePtr> neighbor_group_connections = node_to_move->get_connections_to_level(group_level);
  
  // First we gather info on how the node to move is connected in terms of its
  // neighbors's groups
  for (auto neighbor_group_it  = neighbor_group_connections.begin();
       neighbor_group_it != neighbor_group_connections.end();
       ++neighbor_group_it)
  {
    // Count edges to current group
    node_outward_connections[(*neighbor_group_it)->id]++;
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
      
      // How many connections does this node have to group of interest? 
      double e_si = node_outward_connections[neighbor_group->id];
      
      // How many connections there are between our neighbor group and the
      // potential group
      double e_sr = group_edge_counts[find_edges(potential_group, neighbor_group)];
      
      // How many total connections the neighbor node has
      double e_s = group_edge_counts[find_edges(neighbor_group)];
      
      // Finally calculate partial probability and add to cummulative sum
      cummulative_prob += e_si * ( (e_sr + epsilon) / (e_s + epsilon*(B + 1)) );
    }
    
    // Add the final cumulative probabiltiy sum to potential group's element in
    // probability vector
    probabilities.push_back(cummulative_prob);
  }
  
  // Our sampling algorithm just needs unormalized weights so we don't actually have to normalize. 
  // Normalize vector to sum to 1
  double total_of_probs = std::accumulate(probabilities.begin(), probabilities.end(), double(0));
  for (auto prob = probabilities.begin(); prob != probabilities.end(); ++prob)
  {
    *prob = *prob/total_of_probs;
  }
  
  return Trans_Probs(probabilities, groups);
}

Trans_Probs SBM::get_transition_probs_for_groups(NodePtr node_to_move) {

  // Gather all the group connection counts at the group level
  EdgeCounts level_counts = gather_edge_counts(node_to_move->level + 1);
  
  return get_transition_probs_for_groups(node_to_move, level_counts);
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

    // Get all the edges for group r to its level
    vector<NodePtr> group_r_cons = group_r->get_connections_to_level(level);
    
    // Set total number of edges for group r
    e_rs[find_edges(group_r)] = group_r_cons.size();
    
    // Loop over all edges
    for (auto group_s = group_r_cons.begin(); group_s != group_r_cons.end(); ++group_s) 
    {
      // Add connection counts to the map
      e_rs[find_edges(group_r, *group_s)]++;
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
    level_counts[find_edges(changed_id, old_group_id)] -= amount_changed;
    
    // ...Add to new group
    level_counts[find_edges(changed_id, new_group_id)] += amount_changed;
  }
  
  // Finally update the total counts for the old and new group
  int total_moved = moved_connections.size();
  level_counts[find_edges(old_group_id, old_group_id)] -= total_moved;
  level_counts[find_edges(new_group_id, new_group_id)] += total_moved;
}



// ======================================================= 
// Attempts to move a node to new group, returns true if node moved, false if it stays.
// ======================================================= 
NodePtr SBM::attempt_move(
    NodePtr            node_to_move, 
    EdgeCounts &       group_edge_counts, 
    Weighted_Sampler & sampler) 
{
  int level_of_move = node_to_move->level + 1;

  // Calculate the transition probabilities for all possible groups node could join
  Trans_Probs move_probs = get_transition_probs_for_groups(node_to_move, group_edge_counts);

  // Initialize a sampler to choose group
  Weighted_Sampler my_sampler;

  // Sample probabilies to choose index of new group
  int chosen_group_index = sampler.sample(move_probs.probability);

  // Extract new group
  NodePtr new_group = move_probs.group[chosen_group_index];
  
  return new_group;
}; 



// ======================================================= 
// Run through all nodes in a given level and attempt a group move on each one
// in turn.
// ======================================================= 
int SBM::run_move_sweep(int level) 
{
  // Grab level map
  LevelPtr node_map = get_level(level);
  
  // Get all the nodes at the given level in a shuffleable vector format
  // Initialize vector to hold nodes
  vector<NodePtr> node_vec;
  node_vec.reserve(node_map->size());
  
  // Fill in vector with map elements
  for (auto node_it = node_map->begin(); node_it != node_map->end(); ++node_it)
  {
    node_vec.push_back(node_it->second);
  }
  
  // Shuffle node order
  std::random_shuffle(node_vec.begin(), node_vec.end());
  
  // Build starting edge counts
  int group_level = level + 1;
  EdgeCounts group_edges = gather_edge_counts(group_level);
  
  // Setup random sampler
  Weighted_Sampler my_sampler;
  
  // Keep track of how many moves were made
  int num_moves_made = 0;
  
  // Loop through randomly ordered nodes
  for (auto node_it = node_vec.begin(); node_it != node_vec.end(); ++node_it)
  {
    // Get direct pointer to current node
    NodePtr node_to_move = *node_it;
    
    // Note the current group of the node
    NodePtr old_group = node_to_move->parent;
    
    // Attempt group move
    NodePtr new_group = attempt_move(node_to_move, group_edges, my_sampler);
    
    // Check if chosen group is different than the current group for the node.
    // If group has changed, Update the node's parent and update counts map
    if (new_group->id != old_group->id)
    {
      // Swap parent for newly chosen group
      node_to_move->set_parent(new_group);

      // Update edge counts with this move
      update_edge_counts(group_edges,
                         group_level,
                         node_to_move,
                         old_group,
                         new_group);
      
      // Add to moves made counter
      num_moves_made++;
    }
  } // Ends current sweep loop
  
  // Cleanup any now empty groups
  clean_empty_groups();
  
  // Return number of nodes that were moved
  return num_moves_made;
}  


// ======================================================= 
// Export current state of nodes in model
// ======================================================= 
State_Dump SBM::get_sbm_state(){
  // Initialize the return struct
  State_Dump state; 
  
  // Keep track of how many nodes we've seen so we can preallocate vector sizes
  int n_nodes_seen = 0;
  
  // Loop through all the levels present in SBM
  for (auto level_it = nodes.begin(); level_it != nodes.end(); ++level_it) 
  {
    int level = level_it->first;
    LevelPtr node_level = level_it->second;
    
    // Add level's nodes to current total
    n_nodes_seen += node_level->size();
    
    // Update sizes of the state vectors
    state.id.reserve(n_nodes_seen);
    state.level.reserve(n_nodes_seen);
    state.parent.reserve(n_nodes_seen);
    state.type.reserve(n_nodes_seen);
    
    // Loop through each node in level
    for (auto node_it = node_level->begin(); node_it != node_level->end(); ++node_it )
    {
      // Get currrent node
      NodePtr current_node = node_it->second;

      // Dump all its desired info into its element in the state vectors
      state.id.push_back(current_node->id);
      state.level.push_back(level);
      state.type.push_back(current_node->type);
      
      // Record parent if node has one
      state.parent.push_back(current_node->parent ? current_node->parent->id: "none");
      
    } // End node loop
  } // End level loop
  
  return state;
}                          


