// Inline functions to help out with verbosity of sbm code. 

#include "Node.h"

inline void process_block_pair(
    const std::map<NodePtr, int>::iterator con_block_it,
    const double edges_from_node,
    const double moved_degree_pre,
    const double moved_degree_post,
    double & entropy_pre,
    double & entropy_post)
{

  const double neighbor_deg = con_block_it->first->degree;
  const double edge_count_pre = con_block_it->second;

  // The old and new edge counts we need to compute entropy
  // If we're looking at the neighbor blocks for the old block we need to
  // subtract the edges the node contributed, otherwise we need to add.
  double edge_count_post = edge_count_pre + edges_from_node;

  // Calculate entropy contribution pre move
  double entropy_pre_delta = edge_count_pre > 0
                                 ? edge_count_pre * log(edge_count_pre / (moved_degree_pre * neighbor_deg))
                                 : 0;

  // Calculate entropy contribution post move
  double entropy_post_delta = edge_count_post > 0
                                  ? edge_count_post * log(edge_count_post / (moved_degree_post * neighbor_deg))
                                  : 0;

  entropy_pre += entropy_pre_delta;
  entropy_post += entropy_post_delta;
}

inline double partial_entropy(const double a, const double b, const double c)
{
    // std::cout << a <<"ln(" << a << "/" << b << "*" << c << ")";

    if (a == 0 | b == 0 | c == 0) {
        return 0;
    }

    return a * std::log(a / (b * c));
}

inline double prob_move_r_to_s(const double e_ts, const double e_t, const double B, const double eps)
{
    return (e_ts + eps) / (e_t + eps * B);
}

inline double calc_edge_entropy_for_blocks(const std::vector<NodePtr> nodes_to_check)
{
    double edge_entropy = 0.0;

    // Now initiate a double for loop over the nodes in level to calculate the edge entropy
    for (auto block_r_ptr = nodes_to_check.begin();
         block_r_ptr != nodes_to_check.end();
         block_r_ptr++)
    {
        const NodePtr block_r = *block_r_ptr;

        const int deg_block_r = block_r->degree;

        // Gather all of block r's edges to its own level
        auto block_r_edge_counts = block_r->gather_edges_to_level(block_r->level);

        // Second loop over remaining pairs of nodes
        for (auto block_s_ptr = block_r_ptr;
             block_s_ptr != nodes_to_check.end();
             block_s_ptr++)
        {
            const NodePtr block_s = *block_s_ptr;
            const int e_rs = block_r_edge_counts[block_s];
            // Compute this iteration's controbution to sum
            edge_entropy += partial_entropy(e_rs, block_s->degree, deg_block_r);
        }
    }

    return edge_entropy;
}

inline void process_block_pair(
    const std::map<NodePtr, int>::iterator con_block_it,
    const double edges_from_node,
    const double moved_degree_pre,
    const double moved_degree_post,
    double * entropy_pre,
    double * entropy_post)
{

  const double neighbor_deg = con_block_it->first->degree;
  const double edge_count_pre = con_block_it->second;

  // The old and new edge counts we need to compute entropy
  // If we're looking at the neighbor blocks for the old block we need to
  // subtract the edges the node contributed, otherwise we need to add.
  double edge_count_post = edge_count_pre + edges_from_node;

  // Calculate entropy contribution pre move
  double entropy_pre_delta = edge_count_pre > 0 ? edge_count_pre *
                                                      log(edge_count_pre / (moved_degree_pre * neighbor_deg))
                                                : 0;

  // Calculate entropy contribution post move
  double entropy_post_delta = edge_count_post > 0 ? edge_count_post *
                                                        log(edge_count_post / (moved_degree_post * neighbor_deg))
                                                  : 0;

  (*entropy_pre) += entropy_pre_delta;
  (*entropy_post) += entropy_post_delta;
}

inline double calculate_partial_prob(NodePtr old_block, NodePtr new_block, std::vector<NodePtr> neighbor_blocks)
{
    const int block_level = old_block->level;
    std::map<NodePtr, int> old_block_cons = old_block->gather_edges_to_level(block_level);
    std::map<NodePtr, int> new_block_cons = new_block->gather_edges_to_level(block_level);

    // Get degrees of the two blocks pre-move
    const double deg_old_block = old_block->degree;
    const double deg_new_block = new_block->degree;

    double entropy_portion = 0;

    const int n = neighbor_blocks.size();
    for (int i = 0; i < n; i++)
    {
        NodePtr neighbor_block = neighbor_blocks[i];
        const int e_new_to_neighbor = new_block_cons[neighbor_block];
        const int e_old_to_neighbor = old_block_cons[neighbor_block];

        entropy_portion += partial_entropy(e_old_to_neighbor, neighbor_block->degree, deg_old_block);

        if (i != 0)
        {
            entropy_portion += partial_entropy(e_new_to_neighbor, neighbor_block->degree, deg_new_block);
        }
    }

    return entropy_portion;
}