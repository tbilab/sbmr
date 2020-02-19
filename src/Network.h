#ifndef __NETWORK_INCLUDED__
#define __NETWORK_INCLUDED__

#include "Edge.h"
#include "Node.h"
#include "Sampler.h"

#include <unordered_map>

// =============================================================================
// What this file declares
// =============================================================================
class SBM;

struct State_Dump {
  std::vector<std::string> id;
  std::vector<std::string> parent;
  std::vector<int>         level;
  std::vector<std::string> type;
  State_Dump() {};
  State_Dump(
      std::vector<string>      i,
      std::vector<string>      p,
      std::vector<int>         l,
      std::vector<std::string> t)
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
  NodeVec get_nodes_from_level(const std::string& type,
                               const int          level,
                               const bool         match_type);

  public:
  // Attributes
  // =========================================================================
  LevelMap                                  nodes;            // A kmap keyed by level integer of each level of nodes
  std::map<std::string, std::map<int, int>> node_type_counts; // A map keyed by type to a map keyed by level of node counts
  std::list<Edge>                           edges;            // Each pair of edges in the network

  // Map keyed by a node type. Value is the types of nodes the key type is allowed to connect to.
  std::unordered_map<std::string, std::unordered_set<std::string>> edge_type_pairs;

  // Do we have an explicitely set list of allowed edges or should we build this list ourselves?
  bool specified_allowed_edges = false;

  // Methods
  // =========================================================================
  // Adds a node of specified id of a type at desired level.
  NodePtr add_node(const std::string& id,
                   const std::string& type = "a",
                   const int          level = 0);

  // Adds a edge between two nodes
  void add_edge(NodePtr node_a, NodePtr node_b);                   // based on their references
  void add_edge(const std::string& id_a, const std::string& id_b); // based on their ids

  // Add an alowed pairing of node types for edges
  void add_edge_types(const std::string& from_type, const std::string& to_type);

  // Setup a new Node level
  void add_level(int level_index);

  // Creates a new block node and adds it to its neccesary level
  NodePtr create_block_node(const std::string& type, const int level);

  // Grabs pointer to level of nodes
  LevelPtr get_level(int level);

  // Export current state of nodes in model
  State_Dump get_state();

  // Grabs and returns node of specified id, at desired level.
  NodePtr get_node_by_id(const std::string& id,
                         const int          level = 0);

  // Grabs the first node found at a given level, used in testing.
  NodePtr get_node_from_level(int level);
  NodeVec get_nodes_of_type_at_level(const std::string& type, const int level); // Return nodes of a desired type from level matching type

  // Gathers counts of edges between any two blocks in network
  std::map<Edge, int> get_block_counts_at_level(int level);

  // Load a level blocking from a state dump
  void load_from_state(State_Dump state);

  // Builds and assigns a block node for every node in a given level
  void give_every_node_at_level_own_block(int level);

  // Adds a num_blocks to model and randomly assigns them for a given level (-1 means every node gets their own block)
  void initialize_blocks(int num_blocks,
                         int level);

  // Scan through levels and remove all block nodes that have no children. Returns # of blocks removed
  NodeVec clean_empty_blocks();

  // Builds a block id from a scaffold for generated new blocks
  static string build_block_id(const std::string& type,
                               const int          level,
                               const int          index);
};

#endif