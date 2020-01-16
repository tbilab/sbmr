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
    const std::map<NodePtr, int> node_edge_counts = node->gather_edges_to_level(node->level);
    const int node_degree = node->degree;

    double entropy_sum = 0;

    // Next we loop over this edge counts list
    for (auto neighbor_group_edges = node_edge_counts.begin();
         neighbor_group_edges != node_edge_counts.end();
         neighbor_group_edges++)
    {
        entropy_sum += partial_entropy(neighbor_group_edges->second,
                                       (neighbor_group_edges->first)->degree,
                                       node_degree);
    }

    return entropy_sum;
}


// Takes a node and computes its portion of the edge counts entropy
inline double compute_node_edge_entropy_partial(const std::map<NodePtr, int> &node_edge_counts,
                                                const NodePtr node,
                                                const NodePtr partner)
{
    // First we collapse the nodes edge counts to all it's neighbors
    // const std::map<NodePtr, int> node_edge_counts = node->gather_edges_to_level(node->level);
    const int node_degree = node->degree;

    double entropy_sum = 0;

    // Next we loop over this edge counts list
    for (auto neighbor_group_edges = node_edge_counts.begin();
         neighbor_group_edges != node_edge_counts.end();
         neighbor_group_edges++)
    {
        NodePtr neighbor = neighbor_group_edges->first;

        // If the neighbor is a partner or this is a self pair we need to
        // divide the results by two so we don't double count them.
        const bool neighbor_is_partner = neighbor == partner;
        const bool is_self_pair = node == neighbor;
        const int scalar = (neighbor_is_partner | is_self_pair) ? 2 : 1;

        entropy_sum += partial_entropy(neighbor_group_edges->second,
                                       neighbor->degree,
                                       node_degree) / scalar;
    }

    return entropy_sum;
}


inline double calc_prob_of_move(const std::map<NodePtr, int> &to_node_cons,
                                const std::vector<NodePtr> &possible_neighbors,
                                const double eps)
{
    double prob = 0;
    const double n_possible = possible_neighbors.size();

    for (NodePtr neighbor : possible_neighbors)
    {
        // Search the node being moved to's connections for the current neighbor
        const auto count_to_neighbor_it = to_node_cons.find(neighbor);

        // If it wasnt found set count to 0, otherwise set it to its value.
        const double count_to_neighbor = count_to_neighbor_it == to_node_cons.end() ? 0 : count_to_neighbor_it->second;

        // Add on this pair's contribution to probability sum
        prob += ((count_to_neighbor + eps) / (neighbor->degree + (eps*n_possible)));
    }

    return prob;
}


