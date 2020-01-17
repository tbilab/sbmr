#include "Block_Consensus.h"

void Block_Consensus::initialize(const LevelPtr node_map)
{
  for (auto node_a_it = node_map->begin();
       node_a_it != node_map->end();
       node_a_it++) {
    for (auto node_b_it = std::next(node_a_it);
         node_b_it != node_map->end();
         node_b_it++) {
      bool in_same_group = node_a_it->second->parent == node_b_it->second->parent;

      // Initialize pair info for group
      concensus_pairs.emplace(
          make_pair_key(node_a_it->first, node_b_it->first),
          Pair_Status(in_same_group));
    }
  }
}

void Block_Consensus::update_pair_tracking_map(const std::unordered_set<std::string>& updated_pairs)
{
  for (auto pair_it = concensus_pairs.begin();
       pair_it != concensus_pairs.end();
       pair_it++) {
    // Check if this pair was updated on last sweep
    auto sweep_change_loc   = updated_pairs.find(pair_it->first);
    bool updated_last_sweep = sweep_change_loc != updated_pairs.end();

    if (updated_last_sweep) {
      // Update the pair connection status
      (pair_it->second).connected = !(pair_it->second).connected;
    }

    // Increment the counts if needed
    if ((pair_it->second).connected) {
      (pair_it->second).times_connected++;
    }
  }
}

void Block_Consensus::dump_results(std::vector<std::string>& node_pair,
                                   std::vector<int>&         times_connected)
{
  // Fill out pair tracking vectors with map internals
  node_pair.reserve(concensus_pairs.size());
  times_connected.reserve(concensus_pairs.size());

  for (auto pair_it = concensus_pairs.begin();
       pair_it != concensus_pairs.end();
       pair_it++) {
    node_pair.push_back(pair_it->first);
    times_connected.push_back((pair_it->second).times_connected);
  }
}

void Block_Consensus::update_changed_pairs(NodePtr                          curr_node,
                                           ChildSet&                        old_connections,
                                           ChildSet&                        new_connections,
                                           std::unordered_set<std::string>& pair_moves)
{
  // Loop through all the nodes in the previous group node changes
  for (auto lost_pair_it = old_connections.begin();
       lost_pair_it != old_connections.end();
       lost_pair_it++) {
    pair_moves.insert(make_pair_key(curr_node->id, (*lost_pair_it)->id));
  }

  // Repeat for the new groups children
  for (auto new_pair_it = new_connections.begin();
       new_pair_it != new_connections.end();
       new_pair_it++) {
    // Make sure we don't add this node to itself.
    if ((*new_pair_it)->id != curr_node->id) {
      pair_moves.insert(make_pair_key(curr_node->id, (*new_pair_it)->id));
    }
  }
}
