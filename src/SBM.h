#ifndef __SBM_INCLUDED__
#define __SBM_INCLUDED__

#include "Block_Consensus.h"
#include "Network.h"
#include "Sampler.h"

#include <math.h>

// =============================================================================
// What this file declares
// =============================================================================
class SBM;

struct Merge_Step {
  double              entropy_delta;
  State_Dump          state;
  int                 num_blocks;
  std::vector<string> from_node;
  std::vector<string> to_node;
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
  std::unordered_set<std::string> pair_moves;
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

// =============================================================================
// Main node class declaration
// =============================================================================
class SBM : public Network {
  public:
  // Constructors
  // =========================================================================
  // Just sets default epsilon value to computer derived seed
  SBM()
  {
  }

  // Sets default seed to specified value
  SBM(int sampler_seed)
      : sampler(sampler_seed)
  {
  }

  // Attributes
  // =========================================================================
  // A random sampler generation class.
  Sampler sampler;

  // Parameters that control the mcmc and merging stuffs
  double EPS = 0.1;

  // Methods
  // =========================================================================

  // Compute microcononical entropy of current model state at a level
  double compute_entropy(int level);

  // Merge two blocks, placing all nodes that were under block_b under
  // block_a and deleting from model.
  void merge_blocks(NodePtr block_a, NodePtr block_b);

  // Use model state to propose a potential block move for a node.
  NodePtr propose_move(NodePtr node);

  // Make a decision on the proposed new block for node
  Proposal_Res make_proposal_decision(NodePtr node, NodePtr new_block);

  // Runs efficient MCMC sweep algorithm on desired node level
  MCMC_Sweeps mcmc_sweep(int  level,
                         int  num_sweeps,
                         bool variable_num_blocks,
                         bool track_pairs,
                         bool verbose = false);

  // Merge two blocks at a given level based on the probability of doing so
  Merge_Step agglomerative_merge(int level_of_blocks, int n_merges, int num_checks_per_block);

  // Run mcmc chain initialization by finding best organization
  // of B' blocks for all B from B = N to B = 1.
  std::vector<Merge_Step> collapse_blocks(int    node_level,
                                          int    num_mcmc_steps,
                                          int    desired_num_blocks,
                                          int    num_checks_per_block,
                                          double sigma,
                                          bool   report_all_steps);

}; // End SBM class declaration

#endif