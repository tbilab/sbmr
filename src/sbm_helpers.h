#ifndef __HELPERS_INCLUDED__
#define __HELPERS_INCLUDED__
// Inline functions to help out with verbosity of sbm code.

#include "Node.h"

inline double partial_entropy(const double& a,
                              const double& b,
                              const double& c)
{

  if (a == 0 | b == 0 | c == 0) {
    return 0;
  }

  return a * std::log(a / (b * c));
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

// Adds an edge type the the edge type tracking map.
using EdgeTypes = std::map<std::string, std::set<std::string>>;

inline void add_edge_type(EdgeTypes&         edge_type_pairs,
                          const std::string& from_type,
                          const std::string& to_type)
{
  edge_type_pairs[from_type].insert(to_type);
  edge_type_pairs[to_type].insert(from_type);
}
#endif
