// This script serves as a portal into debugging. I use it by pasting in any
// tests that are failing and then stepping through to see what's going on.

#include <iostream>
#include "../SBM.h"
#include "../cpp_tests/network_builders.cpp"

int main(int argc, char **argv)
{

  int num_sweeps = 10;

  // Setup simple SBM model
  SBM my_SBM = build_simple_SBM();

  auto all_nodes = my_SBM.get_level(0);

  for (int i = 0; i < num_sweeps; i++)
  {
    std::cout << "mcmc sweep: " << i << std::endl;
    // Loop through all nodes
    for (auto node_to_move_it = all_nodes->begin();
         node_to_move_it != all_nodes->end();
         node_to_move_it++)
    {
      NodePtr node_to_move = node_to_move_it->second;

      // Calculate current model entropy
      double pre_entropy = my_SBM.compute_entropy(0);

      // Choose random group for node to join
      NodePtr group_to_move_to = my_SBM.sampler.sample(my_SBM.get_nodes_of_type_at_level(node_to_move->type, 1));


      std::cout << "Moving node " << node_to_move->id << " from " <<  
                   (node_to_move->parent)->id << " to " <<
                    group_to_move_to->id << std::endl;

      // Get move proposal report for move
      Proposal_Res proposal_vals = my_SBM.make_proposal_decision(node_to_move, group_to_move_to);

      double reported_entropy_delta = proposal_vals.entropy_delta;

      // Move node
      node_to_move->set_parent(group_to_move_to);

      // Take new model entropy
      double post_entropy = my_SBM.compute_entropy(0);

      std::cout << "(True : Reported) -> ("
                << std::to_string(post_entropy - pre_entropy) << " , " << reported_entropy_delta << ")"
                << std::endl;

    } // End node loop
  }   // End iteration loop

  return 0;
}
