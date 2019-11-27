#ifndef __NETWORK_INCLUDED__
#define __NETWORK_INCLUDED__

#include "Node.h"
#include "helpers.h"

#include <set>
#include <iostream>

// =============================================================================
// What this file declares
// =============================================================================
class SBM;

struct State_Dump
{
  std::vector<string> id;
  std::vector<string> parent;
  std::vector<int> level;
  std::vector<int> type;
  State_Dump(){};
  State_Dump(
      std::vector<string> i,
      std::vector<string> p,
      std::vector<int> l,
      std::vector<int> t) : id(i), parent(p), level(l), type(t) {}
};

// Some type definitions for cleaning up ugly syntax
typedef std::shared_ptr<Node> NodePtr;
typedef std::map<string, NodePtr> NodeLevel;
typedef std::shared_ptr<NodeLevel> LevelPtr;
typedef std::map<int, LevelPtr> LevelMap;
typedef std::map<std::pair<NodePtr, NodePtr>, int> EdgeCounts;
typedef std::map<int, EdgeCounts> EdgeMap;

using std::string;

// =============================================================================
// Main node class declaration
// =============================================================================
class Network
{

private:
  // Return nodes of a desired type from level can be switched
  // Either matching or not matching the specified type
  std::list<NodePtr> get_nodes_from_level(int type, int level, bool match_type);

public:
  // Attributes
  // =========================================================================
  // A map keyed by level integer of each level of nodes
  LevelMap nodes;

  // A map keyed by level with the node to node edge counts for that level
  EdgeMap edge_counts;

  // Vector storing all the unique types of nodes seen.
  // Used to make sure the correct move proposals are made
  std::set<int> unique_node_types;

  // Methods
  // =========================================================================
  // Export current state of nodes in model
  State_Dump get_state();

  // Load a level grouping from a state dump
  void load_from_state(State_Dump state);

  // Setup a new Node level
  void add_level(int level_index);

  // Grabs pointer to level of nodes
  LevelPtr get_level(int level);

  // Adds a node of specified id of a type at desired level. 
  NodePtr add_node(string id, int type, int level = 0);

  // Creates a new group node and adds it to its neccesary level
  NodePtr create_group_node(int type, int level);

  // Grabs and returns node of specified id, at desired level. 
  NodePtr get_node_by_id(string id, int level = 0);

  // Attempts to find node in network. If node doesn't exist, it will add it.
  NodePtr find_or_add_node(string id, int level, int type);

  // Grabs the first node found at a given level, used in testing.
  NodePtr get_node_from_level(int level);

  // Return nodes of a desired type from level matching type
  std::list<NodePtr> get_nodes_of_type_at_level(int type, int level);

  // Return all nodes not of a specified type from level
  std::list<NodePtr> get_nodes_not_of_type_at_level(int type, int level);

  // Adds a connection between two nodes based on their references
  void add_connection(NodePtr node_a, NodePtr node_b);

  // Adds a connection between two nodes based on their ids
  void add_connection(string id_a, string id_b);

  // Builds and assigns a group node for every node in a given level
  void give_every_node_at_level_own_group(int level);

  // Scan through levels and remove all group nodes that have no children.
  // Returns number of groups removed
  int clean_empty_groups();

  // Builds a id-id paired map of edge counts between nodes of the same level
  EdgeCounts gather_edge_counts(int level);

  // Get reference to edge count map so it can be updated
  EdgeCounts *get_edge_counts(int level);

  // Update network's internal edge counts map after structure change
  void update_edge_counts(NodePtr updated_node, NodePtr new_group);

  // Builds a group id from a scaffold for generated new groups
  static string build_group_id(int type, int level, int index);
};

#endif