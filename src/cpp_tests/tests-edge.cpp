#include "../Edge.h"
#include "catch.hpp"

TEST_CASE("Edge attributes filled in properly", "[Edge]")
{
  Node_UPtr n1 = Node_UPtr( new Node{"n1", 0, 2});
  Node_UPtr n2 = Node_UPtr( new Node{"n2", 0, 2});
  Node_UPtr a1 = Node_UPtr( new Node{"a1", 1, 2});
  Node_UPtr b1 = Node_UPtr( new Node{"b1", 1, 2});
 
  n1->set_parent(b1.get());
  n2->set_parent(a1.get());

  // Build basic edge between two nodes
  const Edge n1_to_n2 = Edge(n1.get(), n2.get());

  // Make sure that n1 is chosen as node_a (aka the first in alphabetical order)
  REQUIRE(n1_to_n2.node_a->id() == "n1");
  REQUIRE(n1_to_n2.node_b->id() == "n2");

  // Make sure that the construction of the string for the edge id went okay
  REQUIRE(n1_to_n2.pair_id == "n1--n2");

  // Make sure that we can project the edge to the next level and it works properly
  const Edge edge_at_l1 = n1_to_n2.at_level(1);
  REQUIRE(edge_at_l1.node_a->id() == "a1");
  REQUIRE(edge_at_l1.node_b->id() == "b1");
  REQUIRE(edge_at_l1.pair_id == "a1--b1");
}
