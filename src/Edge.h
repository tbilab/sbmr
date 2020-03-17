#pragma once

#include "Node.h"

class Edge {
  public:
  // Encodes an edge between two nodes such that they will always have the same order
  // (alphabetical)
  Node* node_a;
  Node* node_b;
  std::string pair_id;
  Edge(Node* a, Node* b)
      : node_a(a->get_id() < b->get_id() ? a : b)
      , node_b(a->get_id() < b->get_id() ? b : a)
      , pair_id(node_a->get_id() + "--" + node_b->get_id())
  {
  }
  inline Edge at_level(const int level) const
  {
    // Project edge to desired level
    return Edge(node_a->get_parent_at_level(level),
                node_b->get_parent_at_level(level));
  }
  bool operator==(const Edge& edge_2) const
  {
    return pair_id == edge_2.pair_id;
  }
  bool operator<(const Edge& edge_2) const
  {
    return pair_id < edge_2.pair_id;
  }
};