#include "../network.h"
#include "catch.hpp"


SBM_Network simple_bipartite() 
{
  SBM_Network my_sbm{{ "a", "b" }, 42};

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

TEST_CASE("Generate Node move proposals - Simple Bipartite", "[SBM]")
{
  double tol  = 0.05;
  double eps  = 0.01;
  
  auto my_sbm = simple_bipartite();
  // SBM_Network my_sbm{{ "a", "b" }, 42};

  // // Add nodes to graph first
  // Node* a1 = my_sbm.add_node("a1", "a");
  // Node* a2 = my_sbm.add_node("a2", "a");
  // Node* a3 = my_sbm.add_node("a3", "a");
  // Node* a4 = my_sbm.add_node("a4", "a");
  // Node* b1 = my_sbm.add_node("b1", "b");
  // Node* b2 = my_sbm.add_node("b2", "b");
  // Node* b3 = my_sbm.add_node("b3", "b");
  // Node* b4 = my_sbm.add_node("b4", "b");

  // // Make blocks
  // my_sbm.build_level();
  // Node* a11 = my_sbm.add_node("a11", "a", 1);
  // Node* a12 = my_sbm.add_node("a12", "a", 1);
  // Node* a13 = my_sbm.add_node("a13", "a", 1);
  // Node* b11 = my_sbm.add_node("b11", "b", 1);
  // Node* b12 = my_sbm.add_node("b12", "b", 1);
  // Node* b13 = my_sbm.add_node("b13", "b", 1);

  // // Add edges
  // my_sbm.add_edge("a1", "b2");
  // my_sbm.add_edge("a2", "b1");
  // my_sbm.add_edge("a2", "b2");
  // my_sbm.add_edge("a3", "b1");
  // my_sbm.add_edge("a3", "b2");
  // my_sbm.add_edge("a3", "b4");
  // my_sbm.add_edge("a4", "b3");

  // // Assign nodes to their blocks
  // a1->set_parent(a11);
  // a2->set_parent(a12);
  // a3->set_parent(a12);
  // a4->set_parent(a13);
  // b1->set_parent(b11);
  // b2->set_parent(b11);
  // b3->set_parent(b12);
  // b4->set_parent(b13);
 
  // int num_trials        = 100;
  // int num_times_no_move = 0;
  // Node* old_block       = a1->parent();

  // // Run multiple trials and of move and see how often a given node is moved
  // for (int i = 0; i < num_trials; ++i) {
  //   // Do move attempt (dry run)
  //   Node* new_block = my_sbm.propose_move(a1, eps);

  //   if (new_block == old_block)
  //     num_times_no_move++;
  // }

  // double frac_of_time_no_change = double(num_times_no_move) / double(num_trials);

  // // Prob of a1 staying in a1_1 should be approximately (2 + eps)/(6 + 4*eps)
  // // Make sure model's decisions to move a1 reflects this.
  // double two  = 2;
  // double six  = 6;
  // double four = 4;

  // REQUIRE(
  //     Approx((two + eps) / (six + four * eps)).epsilon(tol) == frac_of_time_no_change);
}