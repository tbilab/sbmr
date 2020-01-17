#include "Block_Consensus.h"

void Block_Consensus::initialize(const LevelPtr node_map)
{

  for (auto node_a_it = node_map->begin();
       node_a_it != node_map->end();
       node_a_it++) {

    for (auto node_b_it = std::next(node_a_it);
         node_b_it != node_map->end();
         node_b_it++) {

      // Initialize pair info for group
      concensus_pairs.emplace(
          make_pair_key(node_a_it->first, node_b_it->first),
          Pair_Status(node_a_it->second->parent == node_b_it->second->parent)); // Checks if in same group
    
    }
  }
}

void Block_Consensus::update_pair_tracking_map(const std::unordered_set<std::string>& updated_pairs)
{
  for (auto& pair : concensus_pairs) {

    // Check if this pair was updated on last sweep
    auto sweep_change_loc   = updated_pairs.find(pair.first);
    bool updated_last_sweep = sweep_change_loc != updated_pairs.end();

    if (updated_last_sweep) {
      // Update the pair connection status
      (pair.second).connected = !(pair.second).connected;
    }

    // Increment the counts if needed
    if ((pair.second).connected) {
      (pair.second).times_connected++;
    }
  }
}

void Block_Consensus::dump_results(std::vector<std::string>& node_pair,
                                   std::vector<int>&         times_connected)
{
  // Fill out pair tracking vectors with map internals
  node_pair.reserve(concensus_pairs.size());
  times_connected.reserve(concensus_pairs.size());

  for (auto const& pair : concensus_pairs) {
    node_pair.push_back(pair.first);
    times_connected.push_back((pair.second).times_connected);
  }
}

void Block_Consensus::update_changed_pairs(NodePtr                          curr_node,
                                           ChildSet&                        old_connections,
                                           ChildSet&                        new_connections,
                                           std::unordered_set<std::string>& pair_moves)
{
  // Loop through all the nodes in the previous group node changes
  for (auto const& lost_pair : old_connections) {
    pair_moves.insert(make_pair_key(curr_node->id, lost_pair->id));
  }

  // Repeat for the new groups children
  for (auto const& new_pair : new_connections) {
    // Make sure we don't add this node to itself.
    if (new_pair->id != curr_node->id) {
      pair_moves.insert(make_pair_key(curr_node->id, new_pair->id));
    }
  }
}
