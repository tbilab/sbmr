#include "Block_Consensus.h"

void Block_Consensus::initialize(const LevelPtr& node_map)
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

void Block_Consensus::update_pair_tracking_map(const PairSet& updated_pairs)
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

void Block_Consensus::update_changed_pairs(const std::string& node_id,
                                           const NodeSet&     old_connections,
                                           const NodeSet&     new_connections,
                                           PairSet&           pair_moves)
{
  // Loop through all the nodes in the previous group node changes
  for (const auto& lost_pair : old_connections) {
    pair_moves.insert(make_pair_key(node_id, lost_pair->id));
  }

  // Repeat for the new groups children
  for (const auto& new_pair : new_connections) {
    // Make sure we don't add this node to itself.
    if (new_pair->id != node_id) {
      pair_moves.insert(make_pair_key(node_id, new_pair->id));
    }
  }
}
