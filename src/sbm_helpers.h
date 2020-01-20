// Inline functions to help out with verbosity of sbm code.

#include "Node.h"

inline double partial_entropy(const double a, const double b, const double c)
{

  if (a == 0 | b == 0 | c == 0) {
    return 0;
  }

  return a * std::log(a / (b * c));
}

// Takes a node and computes its portion of the edge counts entropy
inline double compute_node_edge_entropy(const NodePtr node)
{
  // First we collapse the nodes edge counts to all it's neighbors
  const NodeEdgeMap node_edge_counts = node->gather_edges_to_level(node->level);
  const int         node_degree      = node->degree;

  double entropy_sum = 0;

  // Next we loop over this edge counts list
  for (auto const& neighbor_group_edges : node_edge_counts) {
    entropy_sum += partial_entropy(neighbor_group_edges.second,
                                   (neighbor_group_edges.first)->degree,
                                   node_degree);
  }

  return entropy_sum;
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

