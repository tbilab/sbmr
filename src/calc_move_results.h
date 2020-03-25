#pragma once
// Takes a node and a new block along with the containers for the blocks and nodes and
// calculates both the entropy delta of the SBM before and after the proposed move and
// the ratio of the probabilities of moving to the proposed block before the move and
// moving back to the original block after the move.
// #include "calc_edge_entropy.h"

#include "Node.h"

using Node_Edge_Counts = std::map<Node*, int>;
using Edge_Count       = std::pair<Node*, int>;

struct Move_Results {
  double entropy_delta = 0.0;
  double prob_ratio    = 1.0;
  Move_Results(const double& e, const double& p)
      : entropy_delta(e)
      , prob_ratio(p)
  {
  }
};

double ent(const double e_rs, const double e_r, const double e_s)
{
  return e_rs * std::log(e_rs / (e_r * e_s));
}

Move_Results
get_move_results(Node* node,
                 Node* new_block,
                 const int n_possible_neighbors,
                 const double eps = 0.1)
{
  Node* old_block = node->parent();

  // No need to go on if we're "swapping" to the same group
  if (new_block == old_block)
    return Move_Results(0, 1);

  // These stay constant before and after move
  const int block_level     = node->level() + 1;
  const double& node_degree = node->degree();
  const double epsB         = eps * n_possible_neighbors;

  // These will change before and after move
  double new_block_degree = new_block->degree();
  double old_block_degree = old_block->degree();
  Node* curr_block        = old_block;
  int curr_degree         = old_block_degree;

  // Gather up all the edges for both the node being moved and its old and new blocks
  Edge_Count_Map node_neighbor_counts      = node->gather_neighbors_at_level(block_level);
  Edge_Count_Map new_block_neighbor_counts = new_block->gather_neighbors_at_level(block_level);
  Edge_Count_Map old_block_neighbor_counts = old_block->gather_neighbors_at_level(block_level);

  auto get_block_degree = [&](const Node* block_t) {
    return block_t == old_block
        ? old_block_degree
        : block_t == new_block
            ? new_block_degree
            : block_t->degree();
  };

  auto add_ent = [&](double ent_sum, const Edge_Count& neighbor_count) {
    const Node* block_t = neighbor_count.first;

    const double scalar  = block_t == curr_block ? 2 : 1;
    const int edges_to_t = neighbor_count.second;
    const int t_degree   = get_block_degree(block_t);

    return ent_sum + ent(neighbor_count.second, curr_degree, t_degree) / scalar;
  };

  const double old_block_ent = std::accumulate(old_block_neighbor_counts.begin(),
                                               old_block_neighbor_counts.end(),
                                               0.0,
                                               add_ent);

  curr_block  = new_block;
  curr_degree = new_block_degree;

  const double new_block_ent = std::accumulate(new_block_neighbor_counts.begin(),
                                               new_block_neighbor_counts.end(),
                                               0.0,
                                               add_ent);

  auto calc_p_move_to_new = [&](double sum, const Edge_Count& edge_count) {
    const double t_degree   = get_block_degree(edge_count.first);
    const double edges_to_t = new_block_neighbor_counts[edge_count.first];

    return sum + edge_count.second / node_degree * (edges_to_t + eps) / (t_degree + epsB);
  };

  const double prob_move_to_new = std::accumulate(node_neighbor_counts.begin(),
                                                  node_neighbor_counts.end(),
                                                  0.0,
                                                  calc_p_move_to_new);

  // Keeps track of the entropy sum for the pre-move portion
  const double pre_move_ent = new_block_ent + old_block_ent;

  // Update edge count maps for post move
  for (const auto& node_block_count : node_neighbor_counts) {
    Node* block          = node_block_count.first;
    const int e_to_block = node_block_count.second;

    if (block == new_block) {
      new_block_neighbor_counts[old_block] -= e_to_block;
      new_block_neighbor_counts[new_block] += 2 * e_to_block;

      old_block_neighbor_counts[new_block] -= e_to_block;
    } else if (block == old_block) {
      new_block_neighbor_counts[old_block] += e_to_block;

      old_block_neighbor_counts[new_block] += e_to_block;
      old_block_neighbor_counts[old_block] -= 2 * e_to_block;
    } else {
      new_block_neighbor_counts[block] += e_to_block;
      old_block_neighbor_counts[block] -= e_to_block;
    }
  }

  new_block_degree += node_degree;
  old_block_degree -= node_degree;
  curr_block  = new_block;
  curr_degree = new_block_degree;

  const double new_block_ent_post = std::accumulate(new_block_neighbor_counts.begin(),
                                                    new_block_neighbor_counts.end(),
                                                    0.0,
                                                    add_ent);

  curr_block  = old_block;
  curr_degree = old_block_degree;

  const double old_block_ent_post = std::accumulate(old_block_neighbor_counts.begin(),
                                                    old_block_neighbor_counts.end(),
                                                    0.0,
                                                    add_ent);

  double post_move_ent = new_block_ent_post + old_block_ent_post;

  auto calc_p_return_to_old = [&](double sum, const Edge_Count& edge_count) {
    const double t_degree   = get_block_degree(edge_count.first);
    const double edges_to_t = old_block_neighbor_counts[edge_count.first];

    return sum + edge_count.second / node_degree * (edges_to_t + eps) / (t_degree + epsB);
  };

  const double prob_return_to_old = std::accumulate(node_neighbor_counts.begin(),
                                                    node_neighbor_counts.end(),
                                                    0.0,
                                                    calc_p_return_to_old);

  return Move_Results(pre_move_ent - post_move_ent,
                      prob_return_to_old / prob_move_to_new);
}
