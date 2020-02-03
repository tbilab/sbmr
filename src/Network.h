#ifndef __NETWORK_INCLUDED__
#define __NETWORK_INCLUDED__

#include "Node.h"
#include "Edge.h"
#include "Sampler.h"

#include <unordered_map>

// =============================================================================
// What this file declares
// =============================================================================
class SBM;

struct State_Dump {
  std::vector<string> id;
  std::vector<string> parent;
  std::vector<int>    level;
  std::vector<int>    type;
  State_Dump() {};
  State_Dump(
      std::vector<string> i,
      std::vector<string> p,
      std::vector<int>    l,
      std::vector<int>    t)
      : id(i)
      , parent(p)
      , level(l)
      , type(t)
  {
  }
};


// Some type definitions for cleaning up ugly syntax
typedef std::shared_ptr<Node>      NodePtr;
typedef std::map<string, NodePtr>  NodeLevel;
typedef std::shared_ptr<NodeLevel> LevelPtr;
typedef std::map<int, LevelPtr>    LevelMap;

using std::string;

// =============================================================================
// Main node class declaration
// =============================================================================
class Network {

  private:
  // Return nodes of a desired type from level can be switched
  // Either matching or not matching the specified type
  NodeVec get_nodes_from_level(int type, int level, bool match_type);

  public:
  // Attributes
  // =========================================================================
  LevelMap                          nodes;            // A map keyed by level integer of each level of nodes
  std::map<int, std::map<int, int>> node_type_counts; // A map keyed by type to a map keyed by level of node counts
  std::list<Edge>                   edges;            // Each pair of edges in the network

  // Methods
  // =========================================================================
  State_Dump          get_state();                                     // Export current state of nodes in model
  void                load_from_state(State_Dump state);               // Load a level blocking from a state dump
  void                add_level(int level_index);                      // Setup a new Node level
  LevelPtr            get_level(int level);                            // Grabs pointer to level of nodes
  NodePtr             add_node(string id, int type, int level = 0);    // Adds a node of specified id of a type at desired level.
  NodePtr             create_block_node(int type, int level);          // Creates a new block node and adds it to its neccesary level
  NodePtr             get_node_by_id(string id, int level = 0);        // Grabs and returns node of specified id, at desired level.
  NodePtr             get_node_from_level(int level);                  // Grabs the first node found at a given level, used in testing.
  NodeVec             get_nodes_of_type_at_level(int type, int level); // Return nodes of a desired type from level matching type
  void                add_edge(NodePtr node_a, NodePtr node_b);        // Adds a edge between two nodes based on their references
  void                add_edge(string id_a, string id_b);              // Adds a edge between two nodes based on their ids
  void                give_every_node_at_level_own_block(int level);   // Builds and assigns a block node for every node in a given level
  void                initialize_blocks(int num_blocks, int level);    // Adds a num_blocks to model and randomly assigns them for a given level (-1 means every node gets their own block)
  NodeVec             clean_empty_blocks();                            // Scan through levels and remove all block nodes that have no children. Returns # of blocks removed
  std::map<Edge, int> gather_block_counts_at_level(int level);         // Gathers counts of edges between any two blocks in network
  static string       build_block_id(int type, int level, int index);  // Builds a block id from a scaffold for generated new blocks
};

#endif