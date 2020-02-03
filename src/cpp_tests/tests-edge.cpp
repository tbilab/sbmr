#include "../Edge.h"
#include "catch.hpp"

// Smart pointer to node instance
typedef std::shared_ptr<Node> NodePtr;

TEST_CASE("Edge attributes filled in properly", "[Edge]")
{
  NodePtr n1 = std::make_shared<Node>("n1", 0, 1);
  NodePtr n2 = std::make_shared<Node>("n2", 0, 1);
  NodePtr n3 = std::make_shared<Node>("n3", 0, 1);
  NodePtr m1 = std::make_shared<Node>("m1", 0, 2);
  NodePtr m2 = std::make_shared<Node>("m2", 0, 2);
  NodePtr m3 = std::make_shared<Node>("m3", 0, 2);
  NodePtr c1 = std::make_shared<Node>("c1", 1, 1);
  NodePtr c2 = std::make_shared<Node>("c2", 1, 1);
  NodePtr d1 = std::make_shared<Node>("d1", 1, 2);
  NodePtr d2 = std::make_shared<Node>("d2", 1, 2);

  n1->set_parent(c1);
  n2->set_parent(c1);
  n3->set_parent(c2);

  m1->set_parent(d1);
  m2->set_parent(d2);
  m3->set_parent(d2);

  // Build basic edge between two nodes
  const Edge n1_to_n2 = Edge(n1, n2);

  // Make sure that n1 is chosen as node_a (aka the first in alphabetical order)
  REQUIRE(n1_to_n2.node_a->id == "n1");
  REQUIRE(n1_to_n2.node_b->id == "n2");

  // Make sure that the construction of the string for the edge id went okay
  REQUIRE(n1_to_n2.pair_id == "n1--n2");
}
