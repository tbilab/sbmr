#include "catch.hpp"

#include <iomanip>

#include "../helpers.h"
#include "../SBM.h"

#include "network_builders.cpp"


// Print a state dump for debugging purposes
inline void print_state_dump(State_Dump state)
{
  int n = state.id.size();

  for (int i = 0; i < n; i++)
  {
    std::cout << std::setw(7) << state.id[i] << ", "
              << std::setw(7) << state.parent[i] << ", "
              << std::setw(2) << state.level[i] << ", " << std::endl;
  }
}

TEST_CASE("Generate Node move proposals", "[SBM]")
{
  double tol = 0.01;
  double eps = 0.01;
  SBM my_SBM = build_simple_SBM();

  NodePtr a1 = my_SBM.get_node_by_id("a1");

  // Initialize a sampler to choose group
  Sampler my_sampler;

  int num_trials = 5000;
  int num_times_no_move = 0;
  NodePtr old_group = a1->parent;

  // Run multiple trials and of move and see how often a given node is moved
  for (int i = 0; i < num_trials; ++i)
  {
    // Do move attempt (dry run)
    NodePtr new_group = my_SBM.propose_move(a1, eps);

    if (new_group->id == old_group->id)
      num_times_no_move++;
  }

  double frac_of_time_no_change = double(num_times_no_move) / double(num_trials);

  // Prob of a1 staying in a1_1 should be approximately (2 + eps)/(6 + 4*eps)
  // Make sure model's decisions to move a1 reflects this.
  double two = 2;
  double six = 6;
  double four = 4;

  REQUIRE(
      Approx((two + eps) / (six + four * eps)).epsilon(tol) ==
      frac_of_time_no_change);
}

TEST_CASE("Calculate Model entropy", "[SBM]")
{
  // Setup simple SBM model
  SBM my_SBM = build_simple_SBM();

  EdgeCounts l1_edges = my_SBM.gather_edge_counts(1);

  // Compute full entropy at first level of nodes
  double model_entropy = my_SBM.compute_entropy(0);

  // Test entropy is near a hand-calculated value
  // Should be -8 - ( 2*log(2) + 3*log(6) ) - ( 2*log(2/12) + 4*log(4/30) + 1*log(1/5) + 1*log(1) ) = -1.509004
  REQUIRE(
      model_entropy ==
      Approx(-1.509004).epsilon(0.1));

  // Calculate entropy delta caused by moving a node
  NodePtr node_to_move = my_SBM.get_node_by_id("a1");
  NodePtr from_group = node_to_move->parent;
  NodePtr to_group = my_SBM.get_node_by_id("a12", 1);

  // Calculate the entropy delta along with acceptance prob
  Proposal_Res proposal_results = my_SBM.compute_acceptance_prob(
      l1_edges,
      node_to_move,
      to_group,
      0.1,
      0.1);

  double entropy_delta = proposal_results.entropy_delta;

  // std::cout << "Moving a1 from a11->a12: prob = " << proposal_results.prob_of_accept << std::endl;

  // Now we will actually move the desired node and test to see if entropy has changed
  // Move node
  node_to_move->set_parent(to_group);

  // Recalculate entropy
  double new_entropy = my_SBM.compute_entropy(0);

  // Get difference from original
  double real_entropy_delta = model_entropy - new_entropy;

  REQUIRE(
      real_entropy_delta ==
      Approx(entropy_delta).epsilon(0.1));

  REQUIRE(
      proposal_results.entropy_delta ==
      entropy_delta);
}

TEST_CASE("Basic MCMC sweeps", "[SBM]")
{
  // Setup simple SBM model
  SBM my_SBM = build_simple_SBM();

  State_Dump pre_sweep = my_SBM.get_state();

  // Run a few rounds of sweeps of the MCMC algorithm on network
  int num_sweeps = 1000;

  // Loop over a few different epsilon values
  std::vector<double> epsilons = {0.01, 0.9};
  std::vector<double> avg_num_moves;

  for (double eps : epsilons)
  {
    int total_num_changes = 0;

    for (int i = 0; i < num_sweeps; i++)
    {
      int n_changes = my_SBM.mcmc_sweep(0, false, eps, 1.0);
      total_num_changes += n_changes;
    }

    double avg_num_changes = double(total_num_changes) / double(num_sweeps);

    avg_num_moves.push_back(avg_num_changes);
    // std::cout << std::setw (5) << eps << " -- Avg number of changes = " << avg_num_changes << std::endl;
  }

  // Make sure that we have a more move-prone model when we have a high epsilon value...
  REQUIRE(
      avg_num_moves.at(0) < avg_num_moves.at(epsilons.size() - 1));
}

TEST_CASE("Agglomerative merge steps", "[SBM]")
{
  // Setup simple SBM model
  SBM my_SBM = build_simple_SBM();

  int num_initial_groups = my_SBM.get_level(1)->size();
  double initial_entropy = my_SBM.compute_entropy(0);

  std::cout << "Attempting single merge " << std::endl;
  // Run greedy aglomerative merge with best single merge done
  Merge_Res single_merge = my_SBM.agglomerative_merge(1, true, 5, 1, 0.01);

  // Make sure that we now have one less group than before for each type
  int new_group_num = my_SBM.get_level(1)->size();
  int change_in_groups = num_initial_groups - new_group_num;
  REQUIRE(change_in_groups == 1);

  // Make sure entropy has gone down as we would expect
  REQUIRE(initial_entropy < single_merge.entropy);

  // Run again but this time merging the best 2
  SBM new_SBM = build_simple_SBM();

  std::cout << "Attempting double merge " << std::endl;
  // Run greedy aglomerative merge with best single merge done
  Merge_Res double_merge = new_SBM.agglomerative_merge(1, 2, true, 5, 0.01);

  // Make sure that we now have two fewer groups per type than before
  REQUIRE(
      2 ==
      num_initial_groups - new_SBM.get_level(1)->size());

  // Entropy should go down even more with two merges
  REQUIRE(single_merge.entropy < double_merge.entropy);
}

TEST_CASE("Agglomerative merging algorithm steps", "[SBM]")
{
  // Setup simple SBM model
  SBM my_SBM = build_simple_SBM();

  int num_initial_groups = my_SBM.get_level(1)->size();
  double initial_entropy = my_SBM.compute_entropy(0);

  Merge_Params params;
  // Run full agglomerative merging algorithm till we have just 3 groups left
  auto run_results = my_SBM.agglomerative_run(
    0,  // Run on level 0
    3,  // Reduce to 3 total groups
    params);

  std::cout << "Post-Merging..." << std::endl;
  print_state_dump(my_SBM.get_state());

  // Make sure that we now have just 3 groups left
  REQUIRE(my_SBM.get_level(1)->size() == 3);
}

TEST_CASE("Greedy agglomerative merging on larger network", "[SBM]")
{
  // Setup simple SBM model
  SBM my_SBM = build_simulated_SBM();

  int desired_num_groups = 4;

  // Run full agglomerative merging algorithm till we have just 3 groups left
  auto run_results = my_SBM.agglomerative_run(
      0,                   // Run on level 0
      desired_num_groups); // Reduce to 3 total groups

  // Make sure that we have lumped together at least some groups
  REQUIRE(my_SBM.get_level(1)->size() < my_SBM.get_level(0)->size());

  // Greedy merging should also always return the same results...
  double first_merge_entropy = run_results.end()->entropy;
  for (int i = 0; i < 15; i++)
  {
    SBM new_SBM = build_simulated_SBM();
    auto new_results =
        new_SBM.agglomerative_run(
            0,
            desired_num_groups);

    REQUIRE(
        Approx(run_results.end()->entropy).epsilon(0.5) ==
        first_merge_entropy);
  }
}

TEST_CASE("One merge at a time agglomerative merging on larger network", "[SBM]")
{
  // Setup simple SBM model
  SBM my_SBM = build_simulated_SBM();

  int desired_num_groups = 4;


  Merge_Params params;
  params.sigma = 0.5;
  // Run full agglomerative merging algorithm till we have just 3 groups left
  auto run_results = my_SBM.agglomerative_run(
      0, // Level
      desired_num_groups,
      params);

  int num_groups_removed = my_SBM.get_level(0)->size() - my_SBM.get_level(1)->size();

  // Make sure we have a single step for each group removed.
  REQUIRE(
      num_groups_removed == run_results.size());

}

TEST_CASE("Non-Greedy agglomerative merging on larger network", "[SBM]")
{
  // Setup simple SBM model
  SBM my_SBM = build_simulated_SBM();

  int desired_num_groups = 4;

  Merge_Params params;
  params.greedy = false;
  // Run full agglomerative merging algorithm till we have just 3 groups left
  auto run_results = my_SBM.agglomerative_run(
      0, // Level
      desired_num_groups,
      params);

  // Make sure that we have lumped together at least some groups
  REQUIRE(my_SBM.get_level(1)->size() < my_SBM.get_level(0)->size());
}