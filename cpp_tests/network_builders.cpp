#include "../SBM.h"

SBM build_simulated_SBM()
{
  // This function implements a network built using the bisbmsim package
  // produced for the TBI lab and is built using the R script at
  // /tools/simulate_test_network.R
  SBM my_SBM;

  my_SBM.add_node("a1", 1);
  my_SBM.add_node("a2", 1);
  my_SBM.add_node("a3", 1);
  my_SBM.add_node("a4", 1);
  my_SBM.add_node("a5", 1);
  my_SBM.add_node("a10", 1);
  my_SBM.add_node("a11", 1);
  my_SBM.add_node("a12", 1);
  my_SBM.add_node("a13", 1);
  my_SBM.add_node("a14", 1);
  my_SBM.add_node("a6", 1);
  my_SBM.add_node("a7", 1);
  my_SBM.add_node("a8", 1);
  my_SBM.add_node("a9", 1);
  my_SBM.add_node("b1", 0);
  my_SBM.add_node("b2", 0);
  my_SBM.add_node("b3", 0);
  my_SBM.add_node("b4", 0);

  my_SBM.add_connection("a1", "b1");
  my_SBM.add_connection("a2", "b1");
  my_SBM.add_connection("a3", "b1");
  my_SBM.add_connection("a4", "b1");
  my_SBM.add_connection("a5", "b1");
  my_SBM.add_connection("a1", "b2");
  my_SBM.add_connection("a2", "b2");
  my_SBM.add_connection("a3", "b2");
  my_SBM.add_connection("a4", "b2");
  my_SBM.add_connection("a5", "b2");
  my_SBM.add_connection("a10", "b2");
  my_SBM.add_connection("a11", "b2");
  my_SBM.add_connection("a12", "b2");
  my_SBM.add_connection("a13", "b2");
  my_SBM.add_connection("a14", "b2");
  my_SBM.add_connection("a6", "b3");
  my_SBM.add_connection("a7", "b3");
  my_SBM.add_connection("a8", "b3");
  my_SBM.add_connection("a9", "b3");
  my_SBM.add_connection("a10", "b3");
  my_SBM.add_connection("a11", "b3");
  my_SBM.add_connection("a12", "b3");
  my_SBM.add_connection("a13", "b3");
  my_SBM.add_connection("a14", "b3");
  my_SBM.add_connection("a10", "b4");
  my_SBM.add_connection("a11", "b4");
  my_SBM.add_connection("a12", "b4");
  my_SBM.add_connection("a13", "b4");
  my_SBM.add_connection("a14", "b4");

  return my_SBM;
}

// Loads a simple bipartite sbm model with optional hierarchy added
SBM build_simple_SBM()
{

  // This function builds a network with the following structure
  /*
   +----+               +----+
   +----+   |    |---------------|    |
   |    |   | a1 |-----      ----| b1 |   +----+
   |a11 |---|    |      \  /    -|    |---|    |
   |    |   +----+       \/   /  +----+   |b11 |
   +----+   +----+       /\  /   +----+   |    |
   |    |----- /  \ ----|    |---|    |
   +----+   | a2 |---------------| b2 |   +----+      
   |    |---|    |       /    ---|    |   
   |a12 |   +----+      /   /    +----+   +----+
   |    |   +----+     /   /     +----+   |    |
   |    |---|    |----    /      |    |   |b12 |
   +----+   | a3 |-------        | b3 |---|    |
   |    |------    -----|    |   +----+      
   +----+   +----+      \  /     +----+   +----+
   |    |   +----+       \/      +----+   |    |
   |a13 |---|    |       /\      |    |   |b13 |
   |    |   | a4 |------/  \-----| b4 |---|    |
   +----+   |    |               |    |   +----+
   +----+               +----+
   */
  SBM my_SBM;

  // Add nodes to graph first
  NodePtr a1 = my_SBM.add_node("a1", 0);
  NodePtr a2 = my_SBM.add_node("a2", 0);
  NodePtr a3 = my_SBM.add_node("a3", 0);
  NodePtr a4 = my_SBM.add_node("a4", 0);
  NodePtr b1 = my_SBM.add_node("b1", 1);
  NodePtr b2 = my_SBM.add_node("b2", 1);
  NodePtr b3 = my_SBM.add_node("b3", 1);
  NodePtr b4 = my_SBM.add_node("b4", 1);

  // Add connections
  my_SBM.add_connection(a1, b1);
  my_SBM.add_connection(a1, b2);
  my_SBM.add_connection(a2, b1);
  my_SBM.add_connection(a2, b2);
  my_SBM.add_connection(a3, b1);
  my_SBM.add_connection(a3, b2);
  my_SBM.add_connection(a3, b4);
  my_SBM.add_connection(a4, b3);

  // Make 2 type 0/a groups
  NodePtr a11 = my_SBM.add_node("a11", 0, 1);
  NodePtr a12 = my_SBM.add_node("a12", 0, 1);
  NodePtr a13 = my_SBM.add_node("a13", 0, 1);
  NodePtr b11 = my_SBM.add_node("b11", 1, 1);
  NodePtr b12 = my_SBM.add_node("b12", 1, 1);
  NodePtr b13 = my_SBM.add_node("b13", 1, 1);

  // Assign nodes to their groups
  a1->set_parent(a11);
  a2->set_parent(a12);
  a3->set_parent(a12);
  a4->set_parent(a13);
  b1->set_parent(b11);
  b2->set_parent(b11);
  b3->set_parent(b12);
  b4->set_parent(b13);

  return my_SBM;
}