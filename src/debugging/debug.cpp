// This script serves as a portal into debugging. I use it by pasting in any
// tests that are failing and then stepping through to see what's going on.

#include "../SBM.h"
#include "../cpp_tests/network_builders.cpp"

#include <iostream>

int main(int argc, char** argv)
{
  SBM unipartite_sbm = build_simple_SBM_unipartite();

  // Propose move of n4 to group c
  const NodePtr n4 = unipartite_sbm.get_node_by_id("n4", 0);
  const NodePtr c  = unipartite_sbm.get_node_by_id("c", 1);

  const auto proposal_results = unipartite_sbm.make_proposal_decision(n4, c, false);

  return 0;
}
