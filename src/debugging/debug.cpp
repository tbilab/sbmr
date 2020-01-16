// This script serves as a portal into debugging. I use it by pasting in any
// tests that are failing and then stepping through to see what's going on.

#include <iostream>
#include "../SBM.h"
#include "../cpp_tests/network_builders.cpp"

int main(int argc, char **argv)
{
 int num_sweeps = 2;

  Sampler random(312);

  // Setup a simulated SBM model
  SBM my_SBM = build_unipartite_simulated();

  // Give it some random groupings of the correct number of groups
  my_SBM.initialize_blocks(3, 0);

  auto all_nodes = my_SBM.get_level(0);

  for (int i = 0; i < num_sweeps; i++)
  {
    std::cout << "=================================================" << std::endl;
    std::cout << "Sweep #" << i << std::endl;

    // Loop through all nodes
    for (auto node_to_move_it = all_nodes->begin();
              node_to_move_it != all_nodes->end();
              node_to_move_it++)
    {
      const NodePtr node_to_move = node_to_move_it->second;

      const std::string original_parent = (node_to_move->parent)->id;
      
      // Calculate current model entropy
      const double pre_entropy = my_SBM.compute_entropy(0);

      // Make sure that our entropy value is positive as it should be
      // REQUIRE(pre_entropy > 0);

      // Choose random group for node to join
      auto potential_groups = my_SBM.get_nodes_of_type_at_level(node_to_move->type, 1);
      
      random.sample(potential_groups);
      const NodePtr group_to_move_to = random.sample(potential_groups);
      
      // Get move proposal report for move
      const Proposal_Res proposal_vals = my_SBM.make_proposal_decision(node_to_move, group_to_move_to);

      const double reported_entropy_delta = proposal_vals.entropy_delta;

      // Move node
      node_to_move->set_parent(group_to_move_to);

      // Take new model entropy
      const double true_delta = my_SBM.compute_entropy(0) - pre_entropy;

      std::cout << node_to_move->id << "(" << original_parent <<  ")" << " -> " << group_to_move_to->id << " | "
                << std::to_string(true_delta) << " - " 
                << std::to_string(reported_entropy_delta) << " = " 
                << std::to_string(true_delta - reported_entropy_delta) 
                << std::endl;
      
      // They should be the same
      // REQUIRE(true_delta == Approx(reported_entropy_delta).epsilon(0.1));

    } // End node loop
  } // End iteration loop
  return 0;
}
