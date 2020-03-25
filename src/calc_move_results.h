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
  const int node_level      = node->level();
  const int block_level     = node_level + 1;
  const double& node_degree = node->degree();
  const double epsB         = eps * n_possible_neighbors;

  // These will change before and after move
  double new_degree = new_block->degree();
  double old_degree = old_block->degree();
  bool post_move    = false;
  Node* curr_block = old_block;
  int curr_degree = old_degree;

  // Gather up all the edges for both the node being moved and its old and new blocks
  Edge_Count_Map node_neighbor_counts      = node->gather_neighbors_at_level(block_level);
  Edge_Count_Map new_block_neighbor_counts = new_block->gather_neighbors_at_level(block_level);
  Edge_Count_Map old_block_neighbor_counts = old_block->gather_neighbors_at_level(block_level);

  
  auto add_ent = [&](double ent_sum, const Edge_Count& neighbor_count)
  {
    const Node* block_t = neighbor_count.first;

    const double scalar  = block_t == curr_block ? 2 : 1;
    const int edges_to_t = neighbor_count.second;
    const int t_degree   = block_t == old_block
        ? old_degree
        : block_t == new_block
            ? new_degree
            : block_t->degree();

    return ent_sum + ent(neighbor_count.second, curr_degree, t_degree) / scalar;
  };

  auto add_probs = [&](double sum, const Edge_Count& edge_count) {
    int neighbor_degree = edge_count.first->degree();
    if (post_move) {
      if (edge_count.first == old_block) {
        neighbor_degree -= node_degree;
      } else if (edge_count.first == new_block) {
        neighbor_degree += node_degree;
      }
    }

    const double edges_to_block = post_move
        ? old_block_neighbor_counts[edge_count.first]
        : new_block_neighbor_counts[edge_count.first];

    return sum + edge_count.second / node_degree * (edges_to_block + eps) / (neighbor_degree + epsB);
  };


  const double old_block_ent = std::accumulate(old_block_neighbor_counts.begin(),
                                               old_block_neighbor_counts.end(),
                                               0.0,
                                               add_ent);

  curr_block  = new_block;
  curr_degree = new_degree;

  const double new_block_ent = std::accumulate(new_block_neighbor_counts.begin(),
                                               new_block_neighbor_counts.end(),
                                               0.0,
                                               add_ent);


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

  curr_block  = new_block;
  curr_degree = new_degree + node_degree;

  const double new_block_ent_post = std::accumulate(new_block_neighbor_counts.begin(),
                                                    new_block_neighbor_counts.end(),
                                                    0.0,
                                                    add_ent);

  curr_block  = old_block;
  curr_degree = old_degree - node_degree;

  const double old_block_ent_post = std::accumulate(old_block_neighbor_counts.begin(),
                                                    old_block_neighbor_counts.end(),
                                                    0.0,
                                                    add_ent);


  double post_move_ent = new_block_ent_post + old_block_ent_post;


  // Edge_Map block_pair_counts;
  // sum_edge_counts(block_pair_counts, old_block, new_block);
  // sum_edge_counts(block_pair_counts, new_block);
  // bool post_move = false;

  // auto add_probs = [&](double sum, const Edge_Count_Pair& edge_count) {
  //   int neighbor_degree = edge_count.first->get_degree();
  //   if (post_move) {
  //     if (edge_count.first == old_block) {
  //       neighbor_degree -= node_degree;
  //     } else if (edge_count.first == new_block) {
  //       neighbor_degree += node_degree;
  //     }
  //   }

  //   Node_Edge_Counts& block_counts = post_move ? old_block_neighbor_counts : new_block_neighbor_counts;

  //   const double edges_to_block = post_move ? old_block_neighbor_counts[edge_count.first] : new_block_neighbor_counts[edge_count.first];

  //   return sum + edge_count.second / node_degree * (edges_to_block + eps) / (neighbor_degree + epsB);
  // };

  // const double prob_move_to_new = std::accumulate(node_neighbor_counts.begin(),
  //                                                 node_neighbor_counts.end(),
  //                                                 0.0,
  //                                                 add_probs);

  // post_move = true;

  // // Update edge counts
  // for (const auto& block_count : node_neighbor_counts) {
  //   const int num_edges = block_count.second;

  //   Node* block = block_count.first;

  //   if (block_count.first == old_block) {
  //     // subtract contributions for when node was in old block
  //     block_pair_counts[Edge(old_block, old_block)] -= 2 * num_edges;
  //     old_block_neighbor_counts[old_block] -= 2 * num_edges;

  //     // add edge to new blocks edge counts
  //     block_pair_counts[Edge(new_block, old_block)] += num_edges;
  //     old_block_neighbor_counts[new_block] += num_edges;
  //   } else if (block_count.first == new_block) {
  //     // subtract contributions for when node was in old block
  //     block_pair_counts[Edge(old_block, new_block)] -= num_edges;

  //     // add edge to new blocks edge counts
  //     block_pair_counts[Edge(new_block, new_block)] += 2 * num_edges;
  //   } else {
  //     // subtract contributions for when node was in old block
  //     block_pair_counts[Edge(old_block, block)] -= num_edges;
  //     old_block_neighbor_counts[block] -= num_edges;

  //     // add edge to new blocks edge counts
  //     block_pair_counts[Edge(new_block, block)] += num_edges;
  //   }
  // }

  // const double post_move_ent = std::accumulate(block_pair_counts.begin(),
  //                                              block_pair_counts.end(),
  //                                              0.0,
  //                                              calc_edge_entropy_part);

  // const double prob_return_to_old = std::accumulate(node_neighbor_counts.begin(),
  //                                                   node_neighbor_counts.end(),
  //                                                   0.0,
  //                                                   add_probs);

  return Move_Results(pre_move_ent - post_move_ent, 1);
  // prob_return_to_old / prob_move_to_new);
}
