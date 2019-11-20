// This script serves as a portal into debugging. I use it by pasting in any
// tests that are failing and then stepping through to see what's going on.

#include <iostream>
#include "../helpers.h"
#include "../SBM.h"

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

// Print a state dump for debugging purposes
void print_state_dump(State_Dump state)
{
  int n = state.id.size();

  for (int i = 0; i < n; i++)
  {
    std::cout << state.id[i] << ", "
              << state.parent[i] << ", " 
              << state.level[i] << ", " 
              << std::endl;
  }
}

int main(int argc, char **argv)
{
  // Setup simple SBM model
  SBM my_SBM = build_simple_SBM();

  int num_initial_groups = my_SBM.get_level(1)->size();
  double initial_entropy = my_SBM.compute_entropy(0);

  // Run greedy aglomerative merge with best single merge done
  Merge_Res single_merge = my_SBM.agglomerative_merge(1, true, 5, 1, 0.01);

  // Make sure that we now have one less group than before
  int new_group_num = my_SBM.get_level(1)->size();
  int change_in_groups = num_initial_groups - new_group_num;

  return 0;
}
