// Inline functions to help out with verbosity of sbm code. 

#include "Node.h"

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