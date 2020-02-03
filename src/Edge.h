#include "Node.h"

class Edge {
  public:
  // Encodes an edge between two nodes such that they will always have the same order
  // (alphabetical)
  NodePtr     node_a;
  NodePtr     node_b;
  std::string pair_id;
  Edge(const NodePtr a, const NodePtr b)
      : node_a(a->id < b->id ? a : b), node_b(a->id < b->id ? b : a), pair_id(node_a->id + "--" + node_b->id)
  {
  }
  inline Edge at_level(const int level)
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