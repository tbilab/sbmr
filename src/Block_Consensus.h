#ifndef __BLOCK_CONSENSUS_INCLUDED__
#define __BLOCK_CONSENSUS_INCLUDED__

#include "Network.h"

struct Pair_Status {
  bool connected;
  int  times_connected;
  Pair_Status(bool c)
      : connected(c)
      , times_connected(0) {};
};

typedef std::unordered_set<std::string> PairSet;

class Block_Consensus {
  public:
  // Holds the pairs of nodes to connection status and counts
  std::unordered_map<std::string, Pair_Status> concensus_pairs;

  // Initialies containers when needed
  void initialize(const LevelPtr node_map);

  // Updates the pair statuses and iterates based on a set of changed pairs
  void update_pair_tracking_map(const PairSet& updated_pairs);

  // Unrolls the pairs map to two passed vectors
  void dump_results(std::vector<std::string>& node_pair,
                    std::vector<int>&         times_connected);

  // Update the set of pairs that need to be updated for a given sweep.
  static void update_changed_pairs(NodePtr   curr_node,
                                   ChildSet& old_connections,
                                   ChildSet& new_connections,
                                   PairSet&  pair_moves);
};

// Helper to build alphabetically string pair of two node ids for pair maps
inline std::string make_pair_key(const std::string& a_node, const std::string& b_node)
{
  return a_node > b_node
      ? a_node + "--" + b_node
      : b_node + "--" + a_node;
}

// Helper to build alphabetically string pair of two node ids for pair maps
inline std::string make_pair_key(const NodePtr& a_node, const NodePtr& b_node)
{
  return make_pair_key(a_node->id, b_node->id);
}

#endif