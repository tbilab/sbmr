// This script serves as a portal into debugging. I use it by pasting in any
// tests that are failing and then stepping through to see what's going on.

#include <iostream>
#include "../helpers.h"
#include "../SBM.h"
#include "../cpp_tests/network_builders.cpp"



int main(int argc, char **argv)
{
  // Setup simple SBM model
 // Node level

  SBM my_SBM;

  NodePtr a1 = my_SBM.add_node("a1", 0, 0);
  NodePtr a2 = my_SBM.add_node("a2", 0, 0);
  NodePtr a3 = my_SBM.add_node("a3", 0, 0);
  NodePtr b1 = my_SBM.add_node("b1", 1, 0);
  NodePtr b2 = my_SBM.add_node("b2", 1, 0);
  NodePtr b3 = my_SBM.add_node("b3", 1, 0);
  NodePtr a11 = my_SBM.add_node("a11", 0, 1);
  NodePtr a12 = my_SBM.add_node("a12", 0, 1);
  NodePtr b11 = my_SBM.add_node("b11", 1, 1);
  NodePtr b12 = my_SBM.add_node("b12", 1, 1);


  my_SBM.add_connection(a1, b1);
  my_SBM.add_connection(a1, b2);
  my_SBM.add_connection(a1, b3);
  my_SBM.add_connection(a2, b3);
  my_SBM.add_connection(a3, b2);


  my_SBM.set_node_parent(a2, a11);
  my_SBM.set_node_parent(a1, a11);
  my_SBM.set_node_parent(b1, b11);
  my_SBM.set_node_parent(a3, a12);
  my_SBM.set_node_parent(b3, b12);
  my_SBM.set_node_parent(b2, b11);



  return 0;
}
