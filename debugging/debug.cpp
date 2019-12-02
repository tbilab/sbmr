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
  NodePtr a1 = std::make_shared<Node>("a1", 0, 1);
  NodePtr a2 = std::make_shared<Node>("a2", 0, 1);
  NodePtr a3 = std::make_shared<Node>("a3", 0, 1);
  NodePtr b1 = std::make_shared<Node>("b1", 0, 2);
  NodePtr b2 = std::make_shared<Node>("b2", 0, 2);
  NodePtr b3 = std::make_shared<Node>("b3", 0, 2);
  
  // First level / groups
  NodePtr a11 = std::make_shared<Node>("a11", 1, 1);
  NodePtr a12 = std::make_shared<Node>("a12", 1, 1);
  NodePtr b11 = std::make_shared<Node>("b11", 1, 2);
  NodePtr b12 = std::make_shared<Node>("b12", 1, 2);
  
  // Second level / super groups
  NodePtr a21 = std::make_shared<Node>("a21", 2, 1);
  NodePtr b21 = std::make_shared<Node>("b21", 2, 2);
  

  a1->set_parent(a11);
  a2->set_parent(a12);
  a3->set_parent(a12);
  
  b1->set_parent(b11);
  b2->set_parent(b11);
  b3->set_parent(b12);
  
  a11->set_parent(a21);
  a12->set_parent(a21);
  
  b11->set_parent(b21);
  b12->set_parent(b21);
  
  Node::connect_nodes(a1, b1);
  Node::connect_nodes(a1, b2);
  Node::connect_nodes(a2, b1);
  Node::connect_nodes(a2, b2);
  Node::connect_nodes(a3, b2);
  Node::connect_nodes(a3, b3);
  
  
  // Swap parents of a2 and b2 nodes
  a2->set_parent(a11);
  b2->set_parent(b12);
      
  return 0;
}
