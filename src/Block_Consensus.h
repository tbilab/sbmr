#ifndef __BLOCK_CONSENSUS_INCLUDED__
#define __BLOCK_CONSENSUS_INCLUDED__

#include "Network.h"

struct Pair_Status
{
    bool connected;
    int times_connected;
    Pair_Status(bool c) : connected(c),
                          times_connected(0){};
};

class Block_Consensus
{
public:

    std::unordered_map<std::string, Pair_Status> concensus_pairs;

    void initialize(const LevelPtr node_map);
    
    void update_pair_tracking_map(const std::unordered_set<std::string> &updated_pairs);

    // Update the set of pairs that need to be updated for a given sweep.
    static void update_changed_pairs(NodePtr curr_node,
                                     ChildSet &old_connections,
                                     ChildSet &new_connections,
                                     std::unordered_set<std::string> &pair_moves);
};

#endif