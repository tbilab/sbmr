#ifndef __SBM_INCLUDED__
#define __SBM_INCLUDED__

#include "Network.h"
#include "Sampler.h"
#include <math.h>

// =============================================================================
// What this file declares
// =============================================================================
class SBM;

struct Merge_Step
{
  double entropy;
  State_Dump state;
  int num_blocks;
  std::vector<string> from_node;
  std::vector<string> to_node;
  Merge_Step() {}
  Merge_Step(const double e, const State_Dump s, const int n) : entropy(e),
                                              state(s),
                                              num_blocks(n) {}
};

struct Proposal_Res
{
  double entropy_delta;
  double prob_of_accept;
  Proposal_Res(double e, double p) : entropy_delta(e),
                                     prob_of_accept(p){};
};

struct Sweep_Res
{
  std::list<std::string> nodes_moved;
  double entropy_delta = 0;
};

// =============================================================================
// Main node class declaration
// =============================================================================
class SBM : public Network
{
public:
  // Constructors
  // =========================================================================
  // Just sets default epsilon value to computer derived seed
  SBM() {}

  // Sets default seed to specified value
  SBM(int sampler_seed) : sampler(sampler_seed) {}

  // Attributes
  // =========================================================================
  // A random sampler generation class.
  Sampler sampler;

  // Parameters that control the mcmc and merging stuffs
  double EPS = 0.1;
  double SIGMA = 0.5;
  bool GREEDY = true;
  int N_CHECKS_PER_block = 5; // When not greedy


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
  Sweep_Res mcmc_sweep(int level, bool variable_num_blocks);

  // Merge two blocks at a given level based on the probability of doing so
  Merge_Step agglomerative_merge( int level_of_blocks, int n_merges);

  // Run mcmc chain initialization by finding best organization
  // of B' blocks for all B from B = N to B = 1.
  std::vector<Merge_Step> collapse_blocks(
      int node_level,
      int num_mcmc_steps,
      int desired_num_blocks, // Default value which lets model drop to 1 block per type.
      bool report_all_steps);

}; // End SBM class declaration

#endif