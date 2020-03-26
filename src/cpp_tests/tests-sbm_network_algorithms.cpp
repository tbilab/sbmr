#include "../calc_move_results.h"
#include "../network.h"
#include "build_testing_networks.h"
// #include "../mcmc_sweep.h"
#include "catch.hpp"


TEST_CASE("Generate Node move proposals - Simple Bipartite", "[SBM]")
{
  double tol = 0.05;
  double eps = 0.01;

  auto my_sbm = simple_bipartite();

  int num_trials        = 200;
  int num_times_no_move = 0;
  Node* a1              = my_sbm.get_node_by_id("a1");
  Node* old_block       = a1->parent();

  // Run multiple trials and of move and see how often a given node is moved
  for (int i = 0; i < num_trials; ++i) {
    // Do move attempt (dry run)
    Node* new_block = my_sbm.propose_move(a1, eps);

    if (new_block == old_block)
      num_times_no_move++;
  }

  double frac_of_time_no_change = double(num_times_no_move) / double(num_trials);
  // Prob of a1 staying in a1_1 should be approximately (2 + eps)/(6 + 4*eps)
  // Make sure model's decisions to move a1 reflects this.
  double two   = 2;
  double six   = 6;
  double three = 3;

  REQUIRE(Approx((two + eps) / (six + (three * eps))).epsilon(tol) == frac_of_time_no_change);
}

TEST_CASE("Generate Node move proposals - Simple Unipartite", "[SBM]")
{
  double tol = 0.1;
  double eps = 0.01;

  auto my_sbm = simple_unipartite();

  int num_trials = 2000;
  Node* n5       = my_sbm.get_node_by_id("n5");
  Node* b        = my_sbm.get_node_by_id("n4")->parent();

  // Sanity check to make sure we've got the right block
  REQUIRE(n5->parent() != b);

  std::map<string, int> times_to_block;

  // Run multiple trials and of move and see how often a given node is moved
  for (int i = 0; i < num_trials; ++i) {
    // Do move attempt (dry run)
    times_to_block[my_sbm.propose_move(n5, eps)->id()]++;
  }

  REQUIRE(double(times_to_block.at("a")) / double(num_trials)
          == Approx(0.3033066).epsilon(tol));

  REQUIRE(double(times_to_block.at("b")) / double(num_trials)
          == Approx(0.4155352).epsilon(tol));

  REQUIRE(double(times_to_block.at("c")) / double(num_trials)
          == Approx(0.2811582).epsilon(tol));

  // Value calculated in R using the following:
  // eps <- 0.01
  // B <- 3
  // e_i <- 5
  // p_to_t <- function(e_it, e_tb, e_t){ (e_it/e_i) * ( (e_tb + eps) / (e_t + eps*B) ) }

  // # To a
  // p_to_t(2, 2, 8) +
  // p_to_t(1, 4, 9) +
  // p_to_t(2, 2, 7)
  // # 0.3033066

  // # To b
  // p_to_t(2, 4, 8) +
  // p_to_t(1, 2, 9) +
  // p_to_t(2, 3, 7)
  // # 0.4155352

  // # To c
  // p_to_t(2, 2, 8) +
  // p_to_t(1, 3, 9) +
  // p_to_t(2, 2, 7)
  // # 0.2811582
}

TEST_CASE("Move results information - Simple Bipartite", "[SBM]")
{
  auto my_sbm = simple_bipartite();

  // Make sure we have correct number of blocks
  REQUIRE(my_sbm.num_nodes_at_level(1) == 6);

  auto a2 = my_sbm.get_node_by_id("a2");
  // propose moving a2 into block with a1
  const auto move_results = get_move_results(a2,
                                             my_sbm.get_node_by_id("a1")->parent(),
                                             my_sbm.num_possible_neighbors_for_node(a2),
                                             0.1);

  REQUIRE(move_results.entropy_delta == Approx(-0.5924696).epsilon(0.1));

  // This was a coincidence
  // Value is (2 + eps)/(6 + eps*3) for both pre and post move probs
  REQUIRE(move_results.prob_ratio == 1);
}

TEST_CASE("Move results information - Simple Unipartite", "[SBM]")
{
  auto my_sbm = simple_unipartite();

  // Group c is n5 and n6's parent
  Node* group_c = my_sbm.get_node_by_id("n6")->parent();

  // Propose move of n4 to group c
  Node* n4                = my_sbm.get_node_by_id("n4");
  const int B             = my_sbm.num_possible_neighbors_for_node(n4);
  const auto move_results = get_move_results(n4, group_c, B, 0.5);

  REQUIRE(move_results.entropy_delta == Approx(-0.1117765).epsilon(0.1));
  REQUIRE(move_results.prob_ratio == Approx(0.6820954).epsilon(0.1));
}

// TEST_CASE("MCMC sweeps - Simple Bipartite", "[SBM]")
// {
//   auto my_sbm = simple_bipartite();
//   mcmc_sweep(my_sbm,
//              5,
//              0.2,
//              false,
//              false,
//              false);
// }

