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


  a2->set_parent(a11);
  a1->set_parent(a11);
  b1->set_parent(b11);
  a3->set_parent(a12);
  b3->set_parent(b12);
  b2->set_parent(b11);



  return 0;
}
