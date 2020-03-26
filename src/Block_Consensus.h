#pragma once

#include "network.h"
#include <unordered_map>
#include <unordered_set>

struct Pair_Status {
  bool connected;
  int times_connected;
  Pair_Status(bool c)
      : connected(c)
      , times_connected(0) {};
};

using Pair_Set = std::unordered_set<string>;
using Pair_Map = std::unordered_map<string, Pair_Status>;

// Helper to build alphabetically string pair of two node ids for pair maps
inline string make_pair_key(const string& id_a,
                            const string& id_b)
{
  return id_a > id_b
      ? id_a + "--" + id_b
      : id_b + "--" + id_a;
}

class Block_Consensus {
  private:
  // Holds the pairs of nodes to connection status and counts
  Pair_Map consensus_pairs;

  public:
  int size() const { return consensus_pairs.size(); }
  // Initialies containers when needed
  void initialize(const Type_Vec& node_level)
  {
    // Loop through each type of node and insert unique pairs of members into
    // consensus pairs map. This avoids making pairs of nodes that can't ever
    // be in the same blocks together
    for (const auto& nodes_of_type : node_level) {

      for (auto node_a_it = nodes_of_type.begin();
           node_a_it != nodes_of_type.end();
           node_a_it++) {

        for (auto node_b_it = std::next(node_a_it);
             node_b_it != nodes_of_type.end();
             node_b_it++) {
          // Initialize pair info for group
          consensus_pairs.emplace(
              make_pair_key((*node_a_it)->id(), (*node_b_it)->id()),
              Pair_Status((*node_a_it)->parent() == (*node_b_it)->parent())); // Checks if in same group
        }
      }
    }
  }

  // Updates the pair statuses and iterates based on a set of changed pairs
  void update_pair_tracking_map(const Pair_Set& updated_pairs)
  {
    for (auto& pair : consensus_pairs) {

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

  // Update the set of pairs that need to be updated for a given sweep.
  static void update_changed_pairs(const string& node_id,
                                   const Node_Vec& old_connections,
                                   const Node_Vec& new_connections,
                                   Pair_Set& pair_moves)
  {
    // Loop through all the nodes in the previous group node changes
    for (const auto& lost_pair : old_connections) {
      pair_moves.insert(make_pair_key(node_id, lost_pair->id()));
    }

    // Repeat for the new groups children
    for (const auto& new_pair : new_connections) {
      // Make sure we don't add this node to itself.
      if (new_pair->id() != node_id) {
        pair_moves.insert(make_pair_key(node_id, new_pair->id()));
      }
    }
  }
};
