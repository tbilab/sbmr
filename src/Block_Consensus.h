#ifndef __BLOCK_CONSENSUS_INCLUDED__
#define __BLOCK_CONSENSUS_INCLUDED__

#include "Node.h"

struct Pair_Status {
  bool connected;
  int  times_connected;
  Pair_Status(bool c)
      : connected(c)
      , times_connected(0) {};
};

using PairSet = std::set<std::string>;

class Block_Consensus {
  public:
  // Holds the pairs of nodes to connection status and counts
  std::map<std::string, Pair_Status> concensus_pairs;

  // Initialies containers when needed
  void initialize(const LevelPtr& node_map);

  // Updates the pair statuses and iterates based on a set of changed pairs
  void update_pair_tracking_map(const PairSet& updated_pairs);

  // Update the set of pairs that need to be updated for a given sweep.
  static void update_changed_pairs(const std::string& node_id,
                                   const ChildSet&    old_connections,
                                   const ChildSet&    new_connections,
                                   PairSet&           pair_moves);
};

// Helper to build alphabetically string pair of two node ids for pair maps
inline std::string make_pair_key(const std::string& a_node, const std::string& b_node)
{
  return a_node > b_node
      ? a_node + "--" + b_node
      : b_node + "--" + a_node;
}

#endif