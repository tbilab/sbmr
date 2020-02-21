#ifndef __NETWORK_INCLUDED__
#define __NETWORK_INCLUDED__

#include "Edge.h"
#include "Node.h"
#include "Sampler.h"
#include "Block_Consensus.h"
#include "sbm_helpers.h"

#include <math.h>


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
      std::vector<std::string>      i,
      std::vector<std::string>      p,
      std::vector<int>         l,
      std::vector<std::string> t)
      : id(i)
      , parent(p)
      , level(l)
      , type(t)
  {
  }
};

struct Merge_Step {
  double     entropy_delta;
  double     entropy;
  State_Dump state;
  int        num_blocks;
  Merge_Step()
      : entropy_delta(0)
  {
  }
  Merge_Step(const double e, const State_Dump s, const int n)
      : entropy_delta(e)
      , state(s)
      , num_blocks(n)
  {
  }
};

struct Proposal_Res {
  double entropy_delta;
  double prob_of_accept;
  Proposal_Res(const double e, const double p)
      : entropy_delta(e)
      , prob_of_accept(p) {};
};

struct Sweep_Res {
  std::list<std::string>          nodes_moved;
  std::list<std::string>          new_groups;
  double                          entropy_delta = 0;
  std::set<std::string> pair_moves;
};

struct MCMC_Sweeps {
  std::vector<double>    sweep_entropy_delta;
  std::vector<int>       sweep_num_nodes_moved;
  Block_Consensus        block_consensus;
  std::list<std::string> nodes_moved;
  MCMC_Sweeps(const int n)
  {
    // Preallocate the entropy change and num groups moved in sweep vectors and
    sweep_entropy_delta.reserve(n);
    sweep_num_nodes_moved.reserve(n);
  }
};

// Some type definitions for cleaning up ugly syntax
using CollapseResults = std::vector<Merge_Step>;
using BlockEdgeCounts = std::map<Edge, int>;

// =============================================================================
// Main node class declaration
// =============================================================================
class SBM {

  private:

 
  public:
  // Attributes
  // =========================================================================
  LevelMap                                  nodes;            // A kmap keyed by level integer of each level of nodes
  std::map<std::string, std::map<int, int>> node_type_counts; // A map keyed by type to a map keyed by level of node counts
  std::list<Edge>                           edges;            // Each pair of edges in the network

  // Map keyed by a node type. Value is the types of nodes the key type is allowed to connect to.
  std::map<std::string, std::set<std::string>> edge_type_pairs;

  // Do we have an explicitely set list of allowed edges or should we build this list ourselves?
  bool specified_allowed_edges = false;

  // A random sampler generation class.
  Sampler sampler;

  // Methods
  // =========================================================================
  // Adds a node of specified id of a type at desired level.
  SBM()
  {
  }

  // Sets default seed to specified value
  SBM(int sampler_seed)
      : sampler(sampler_seed)
  {
  }

  NodePtr add_node(const std::string& id,
                   const std::string& type  = "a",
                   const int          level = 0);

  void add_edge(const std::string& id_a, const std::string& id_b); // based on their ids

  // Add an alowed pairing of node types for edges
  void add_edge_types(const std::vector<std::string>& from_types, const std::vector<std::string>& to_types);

  // Creates a new block node and adds it to its neccesary level
  NodePtr create_block_node(const std::string& type, const int level);

  // Grabs pointer to level of nodes
  LevelPtr get_level(const int& level);
  // const version that wont append new level if it doesn't exist
  LevelPtr get_level(const int& level) const; 

  // Export current state of nodes in model
  State_Dump get_state() const;

  // Grabs and returns node of specified id, at desired level.
  NodePtr get_node_by_id(const std::string& id,
                         const int          level = 0) const;

 // Return nodes of a desired type from level matching type
  NodeVec get_nodes_of_type_at_level(const std::string& type, const int& level) const; 

  // Gathers counts of edges between any two blocks in network
  BlockEdgeCounts get_block_edge_counts(const int& level) const;

  // Get a node's block connections map to a desired level
  NodeEdgeMap get_node_to_block_edge_counts(const std::string& id,
                                            const int&         node_level        = 0,
                                            const int&         connections_level = 1) const;
  
  // Load a level blocking from a state dump
  void set_state(const std::vector<std::string>& id,
                       const std::vector<std::string>& parent,
                       const std::vector<int>&         level,
                       const std::vector<std::string>& types);

 
  // Adds a num_blocks to model and randomly assigns them for a given level (-1 means every node gets their own block)
  void initialize_blocks(int level, int num_blocks = -1);

  // Scan through levels and remove all block nodes that have no children. Returns # of blocks removed
  NodeVec clean_empty_blocks();


  // Compute microcononical entropy of current model state at a level
  double get_entropy(int level) const;


  // Use model state to propose a potential block move for a node.
  NodePtr propose_move(const NodePtr& node,
                       const double&  eps,
                       Sampler&       node_chooser) const;

  // Make a decision on the proposed new block for node
  Proposal_Res make_proposal_decision(const NodePtr& node,
                                      const NodePtr& new_block,
                                      const double&  eps);

  // Runs efficient MCMC sweep algorithm on desired node level
  MCMC_Sweeps mcmc_sweep(const int&    level,
                         const int&    num_sweeps,
                         const double& eps,
                         const bool&   variable_num_blocks,
                         const bool&   track_pairs,
                         const bool&   verbose = false);

  // Merge two blocks, placing all nodes that were under block_b under
  // block_a and deleting from model.
  void merge_blocks(const NodePtr& block_a, const NodePtr& block_b);

  // Merge two blocks at a given level based on the probability of doing so
  Merge_Step agglomerative_merge(const int&    level_of_blocks,
                                 const int&    n_merges,
                                 const int&    num_checks_per_block,
                                 const double& eps);

  // Run mcmc chain initialization by finding best organization
  // of B' blocks for all B from B = N to B = 1.
  CollapseResults collapse_blocks(const int&    node_level,
                                  const int&    num_mcmc_steps,
                                  const int&    desired_num_blocks,
                                  const int&    num_checks_per_block,
                                  const double& sigma,
                                  const double& eps,
                                  const bool&   report_all_steps);

  CollapseResults collapse_run(const int&              node_level,
                               const int&              num_mcmc_steps,
                               const int&              num_checks_per_block,
                               const double&           sigma,
                               const double&           eps,
                               const std::vector<int>& block_nums);
};

#endif
