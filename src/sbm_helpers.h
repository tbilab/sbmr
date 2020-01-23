// Inline functions to help out with verbosity of sbm code.

#include "Node.h"

inline double partial_entropy(const double& a,
                              const double& b,
                              const double& c)
{

  if (a == 0 | b == 0 | c == 0) {
    return 0;
  }

  return a * std::log(a / (b * c));;
}

inline int get_edge_counts(const NodeEdgeMap& node_cons, const NodePtr& neighbor)
{
  // Search the node being moved to's connections for the current neighbor
  const auto count_to_neighbor_it = node_cons.find(neighbor);

  // If it wasnt found set count to 0, otherwise set it to its value.
  return count_to_neighbor_it == node_cons.end()
      ? 0
      : count_to_neighbor_it->second;
}
