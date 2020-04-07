#pragma once

#include "../SBM.h"


inline SBM simple_bipartite()
{
  SBM my_sbm { { "a", "b" }, 42 };

  // Add nodes to graph first
  Node* a1 = my_sbm.add_node("a1", "a");
  Node* a2 = my_sbm.add_node("a2", "a");
  Node* a3 = my_sbm.add_node("a3", "a");
  Node* a4 = my_sbm.add_node("a4", "a");

  Node* b1 = my_sbm.add_node("b1", "b");
  Node* b2 = my_sbm.add_node("b2", "b");
  Node* b3 = my_sbm.add_node("b3", "b");
  Node* b4 = my_sbm.add_node("b4", "b");

  // Add edges
  my_sbm.add_edge("a1", "b1");
  my_sbm.add_edge("a1", "b2");
  my_sbm.add_edge("a2", "b1");
  my_sbm.add_edge("a2", "b2");
  my_sbm.add_edge("a3", "b1");
  my_sbm.add_edge("a3", "b2");
  my_sbm.add_edge("a3", "b4");
  my_sbm.add_edge("a4", "b3");

  // Make blocks
  my_sbm.build_block_level();
  Node* a11 = my_sbm.add_node("a11", "a", 1);
  Node* a12 = my_sbm.add_node("a12", "a", 1);
  Node* a13 = my_sbm.add_node("a13", "a", 1);
  Node* b11 = my_sbm.add_node("b11", "b", 1);
  Node* b12 = my_sbm.add_node("b12", "b", 1);
  Node* b13 = my_sbm.add_node("b13", "b", 1);


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

inline SBM simple_unipartite()
{
  SBM my_SBM { { "a" }, 42 };

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
  my_SBM.build_block_level();
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