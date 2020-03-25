#include "../calc_move_results.h"
#include "../network.h"
#include "../swap_blocks.h"
#include "catch.hpp"

SBM_Network simple_bipartite()
{
  SBM_Network my_sbm { { "a", "b" }, 42 };

  // Add nodes to graph first
  Node* a1 = my_sbm.add_node("a1", "a");
  Node* a2 = my_sbm.add_node("a2", "a");
  Node* a3 = my_sbm.add_node("a3", "a");
  Node* a4 = my_sbm.add_node("a4", "a");

  Node* b1 = my_sbm.add_node("b1", "b");
  Node* b2 = my_sbm.add_node("b2", "b");
  Node* b3 = my_sbm.add_node("b3", "b");
  Node* b4 = my_sbm.add_node("b4", "b");

  // Make blocks
  my_sbm.build_level();
  Node* a11 = my_sbm.add_node("a11", "a", 1);
  Node* a12 = my_sbm.add_node("a12", "a", 1);
  Node* a13 = my_sbm.add_node("a13", "a", 1);
  Node* b11 = my_sbm.add_node("b11", "b", 1);
  Node* b12 = my_sbm.add_node("b12", "b", 1);
  Node* b13 = my_sbm.add_node("b13", "b", 1);

  // Add edges
  my_sbm.add_edge("a1", "b1");
  my_sbm.add_edge("a1", "b2");

  my_sbm.add_edge("a2", "b1");
  my_sbm.add_edge("a2", "b2");

  my_sbm.add_edge("a3", "b1");
  my_sbm.add_edge("a3", "b2");
  my_sbm.add_edge("a3", "b4");

  my_sbm.add_edge("a4", "b3");

  // Assign nodes to their blocks
  a1->set_parent(a11);
  a2->set_parent(a12);
  a3->set_parent(a12);
  a4->set_parent(a13);
  b1->set_parent(b11);
  b2->set_parent(b11);
  b3->set_parent(b12);
  b4->set_parent(b13);

  return my_sbm;
}

SBM_Network simple_unipartite()
{
  SBM_Network my_SBM { { "a" }, 42 };

  // Add nodes to graph first
  Node* n1 = my_SBM.add_node("n1", "a");
  Node* n2 = my_SBM.add_node("n2", "a");
  Node* n3 = my_SBM.add_node("n3", "a");
  Node* n4 = my_SBM.add_node("n4", "a");
  Node* n5 = my_SBM.add_node("n5", "a");
  Node* n6 = my_SBM.add_node("n6", "a");

  // Add edges
  my_SBM.add_edge("n1", "n2");
  my_SBM.add_edge("n1", "n3");
  my_SBM.add_edge("n1", "n4");
  my_SBM.add_edge("n1", "n5");
  my_SBM.add_edge("n2", "n3");
  my_SBM.add_edge("n2", "n4");
  my_SBM.add_edge("n2", "n5");
  my_SBM.add_edge("n3", "n4");
  my_SBM.add_edge("n3", "n6");
  my_SBM.add_edge("n4", "n5");
  my_SBM.add_edge("n4", "n6");
  my_SBM.add_edge("n5", "n6");

  // Make 3 blocks
  my_SBM.build_level();
  Node* a = my_SBM.add_node("a", "a", 1);
  Node* b = my_SBM.add_node("b", "a", 1);
  Node* c = my_SBM.add_node("c", "a", 1);

  // Assign nodes to their blocks
  n1->set_parent(a);
  n2->set_parent(a);

  n3->set_parent(b);
  n4->set_parent(b);

  n5->set_parent(c);
  n6->set_parent(c);

  return my_SBM;
}

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
