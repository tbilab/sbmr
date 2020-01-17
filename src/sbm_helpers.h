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

// Takes a node and computes its portion of the edge counts entropy
inline double compute_node_edge_entropy_partial(const NodeEdgeMap& node_edge_counts,
                                                const NodePtr      node,
                                                const NodePtr      partner)
{
  // First we collapse the nodes edge counts to all it's neighbors
  // const NodeEdgeMap node_edge_counts = node->gather_edges_to_level(node->level);
  const int node_degree = node->degree;

  double entropy_sum = 0;

  // Next we loop over this edge counts list
  for (auto const& neighbor_group_edges : node_edge_counts) {

    // If the neighbor is a partner or this is a self pair we need to
    // divide the results by two so we don't double count them.
    const bool neighbor_is_partner = neighbor_group_edges.first == partner;
    const bool is_self_pair        = node == neighbor_group_edges.first;
    const int  scalar              = (neighbor_is_partner | is_self_pair) ? 2 : 1;

    entropy_sum += partial_entropy(neighbor_group_edges.second,
                                   neighbor_group_edges.first->degree,
                                   node_degree)
        / scalar;
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

inline double calc_prob_of_move(const NodeEdgeMap& pre_move_cons,
                                const NodeEdgeMap& post_move_cons,
                                const NodeEdgeMap& moved_node_cons,
                                const NodeVec&     possible_neighbors,
                                const double       node_degree,
                                const double       eps)
{
  double       pre_prob   = 0;
  double       post_prob  = 0;
  const double n_possible = possible_neighbors.size();

  for (const NodePtr& neighbor : possible_neighbors) {
    // First check if node being moved has any connections to this block
    const auto node_to_neighbor_it = moved_node_cons.find(neighbor);
    const int  neighbor_degree     = neighbor->degree;

    // If there are connections we can continue, otherwise result will be 0
    if (node_to_neighbor_it != moved_node_cons.end()) {
      const double prop_node_edges_to_neighbor = double(node_to_neighbor_it->second) / node_degree;

      const double fraction_bottom = neighbor_degree + (eps * n_possible);

      pre_prob += prop_node_edges_to_neighbor * (get_edge_counts(pre_move_cons, neighbor) + eps) / fraction_bottom;
      post_prob += prop_node_edges_to_neighbor * (get_edge_counts(post_move_cons, neighbor) + eps) / fraction_bottom;
    }
  }

  return post_prob / pre_prob;
}
