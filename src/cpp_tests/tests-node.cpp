#include "../Node.h"
#include "print_helpers.h"
#include "catch.hpp"

TEST_CASE("Basic Initialization", "[Node]")
{
  Node_UPtr n1 = Node_UPtr(new Node { "n1", 0, 0, 2 });
  Node_UPtr n2 = Node_UPtr(new Node { "n2", 0, 0, 2 });
  Node_UPtr n3 = Node_UPtr(new Node { "n3", 0, 0, 2 });
  Node_UPtr m1 = Node_UPtr(new Node { "m1", 0, 1, 2 });
  Node_UPtr m2 = Node_UPtr(new Node { "m2", 0, 1, 2 });
  Node_UPtr m3 = Node_UPtr(new Node { "m3", 0, 1, 2 });
  Node_UPtr c1 = Node_UPtr(new Node { "c1", 1, 1, 2 });
  Node_UPtr c2 = Node_UPtr(new Node { "c2", 1, 1, 2 });
  Node_UPtr d1 = Node_UPtr(new Node { "d1", 1, 1, 2 });
  Node_UPtr d2 = Node_UPtr(new Node { "d2", 1, 1, 2 });

  n1->set_parent(c1.get());
  n2->set_parent(c1.get());
  n3->set_parent(c2.get());

  m1->set_parent(d1.get());
  m2->set_parent(d2.get());
  m3->set_parent(d2.get());

  connect_nodes(n1.get(), m1.get());
  connect_nodes(n1.get(), m3.get());
  connect_nodes(n2.get(), m1.get());
  connect_nodes(n3.get(), m2.get());
  connect_nodes(n3.get(), m3.get());

  // Get basic info out of the nodes
  REQUIRE(n1->id() == "n1");
  REQUIRE(n1->get_parent_at_level(1)->id() == n1->parent()->id());

  // // Make sure the edge propigate properly.
  // REQUIRE("m1, m3" == print_node_ids(n1->get_edges_of_type(1,0)));
  // REQUIRE("d1, d2" == print_node_ids(n1->get_edges_of_type(1,1)));
  // REQUIRE("d1, d1, d2" == print_node_ids(c1->get_edges_of_type(1,1)));
  // REQUIRE("d2, d2" == print_node_ids(c2->get_edges_of_type(1,1)));
}

TEST_CASE("Block id construction", "[Node]")
{
  Node_UPtr b1 = Node_UPtr(new Node {0, 0, 0, 2});
  Node_UPtr b2 = Node_UPtr(new Node {1, 0, 0, 2});

  REQUIRE(b1->id() == "b_0");
  REQUIRE(b2->id() == "b_1");
}

TEST_CASE("Child addition and deletion", "[Node]")
{
  Node_UPtr n1  = Node_UPtr(new Node { "n1", 0, 0 });
  Node_UPtr n2  = Node_UPtr(new Node { "n2", 0, 0 });
  Node_UPtr n3  = Node_UPtr(new Node { "n3", 0, 0 });
  Node_UPtr n11 = Node_UPtr(new Node { "n11", 1, 0 });
  Node_UPtr n12 = Node_UPtr(new Node { "n12", 1, 0 });

  n1->set_parent(n11.get());
  n2->set_parent(n11.get());
  n3->set_parent(n12.get());

  // n11 should have 2 children
  REQUIRE(n11->num_children() == 2);
  // One of those children should be n2
  REQUIRE(n11->has_child(n2.get()));

  // n12 should have 1 child
  REQUIRE(n12->num_children() == 1);

  // Set the parent of n2 to be n12 and results should flip
  n2->set_parent(n12.get());

  // n11 should have 1 child
  REQUIRE(n11->num_children() == 1);

  // n12 should have 2 children
  REQUIRE(n12->num_children() == 2);
  // One of those children should be n2
  REQUIRE(n12->has_child(n2.get()));
}

TEST_CASE("Gathering edge counts to a level", "[Node]")
{
  // Node level
  Node_UPtr a1 = Node_UPtr(new Node { "a1", 0, 0, 2 });
  Node_UPtr a2 = Node_UPtr(new Node { "a2", 0, 0, 2 });
  Node_UPtr a3 = Node_UPtr(new Node { "a3", 0, 0, 2 });
  Node_UPtr b1 = Node_UPtr(new Node { "b1", 0, 1, 2 });
  Node_UPtr b2 = Node_UPtr(new Node { "b2", 0, 1, 2 });
  Node_UPtr b3 = Node_UPtr(new Node { "b3", 0, 1, 2 });

  // First level / blocks
  Node_UPtr a11 = Node_UPtr(new Node { "a11", 1, 0, 2 });
  Node_UPtr a12 = Node_UPtr(new Node { "a12", 1, 0, 2 });
  Node_UPtr b11 = Node_UPtr(new Node { "b11", 1, 1, 2 });
  Node_UPtr b12 = Node_UPtr(new Node { "b12", 1, 1, 2 });

  // Second level / super blocks
  Node_UPtr a21 = Node_UPtr(new Node { "a21", 2, 0, 2 });
  Node_UPtr b21 = Node_UPtr(new Node { "b21", 2, 1, 2 });

  connect_nodes(a1.get(), b1.get());
  connect_nodes(a1.get(), b2.get());
  connect_nodes(a2.get(), b1.get());
  connect_nodes(a2.get(), b2.get());
  connect_nodes(a3.get(), b2.get());
  connect_nodes(a3.get(), b3.get());

  a1->set_parent(a11.get());
  a2->set_parent(a12.get());
  a3->set_parent(a12.get());

  b1->set_parent(b11.get());
  b2->set_parent(b11.get());
  b3->set_parent(b12.get());

  a11->set_parent(a21.get());
  a12->set_parent(a21.get());

  b11->set_parent(b21.get());
  b12->set_parent(b21.get());

  // Gather all the edges from node a1 to level 1
  Edge_Count_Map a1_to_l1 = a1->gather_edges_to_level(1);

  REQUIRE(a1_to_l1[b11.get()] == 2);

  REQUIRE(a1_to_l1[b12.get()] == 0);

  // now a3 to l1
  Edge_Count_Map a3_to_l1 = a3->gather_edges_to_level(1);

  REQUIRE(a3_to_l1[b11.get()] == 1);

  REQUIRE(a3_to_l1[b12.get()] == 1);

  // now b2 to l1
  Edge_Count_Map b2_to_l1 = b2->gather_edges_to_level(1);

  REQUIRE(b2_to_l1[a11.get()] == 1);

  REQUIRE(b2_to_l1[a12.get()] == 2);

  // // Last b11 to a21
  // Edge_Count_Map b11_to_l2 = b11->gather_edges_to_level(2);

  // REQUIRE(b11_to_l2[a21.get()] == 5);
}

TEST_CASE("Edge count gathering (unipartite)", "[Node]")
{

  Node_UPtr n1 = Node_UPtr(new Node { "n1", 0, 0 });
  Node_UPtr n2 = Node_UPtr(new Node { "n2", 0, 0 });
  Node_UPtr n3 = Node_UPtr(new Node { "n3", 0, 0 });
  Node_UPtr n4 = Node_UPtr(new Node { "n4", 0, 0 });
  Node_UPtr n5 = Node_UPtr(new Node { "n5", 0, 0 });
  Node_UPtr n6 = Node_UPtr(new Node { "n6", 0, 0 });

  // Add edges
  connect_nodes(n1.get(), n2.get());
  connect_nodes(n1.get(), n3.get());
  connect_nodes(n1.get(), n4.get());
  connect_nodes(n1.get(), n5.get());
  connect_nodes(n2.get(), n3.get());
  connect_nodes(n2.get(), n4.get());
  connect_nodes(n2.get(), n5.get());
  connect_nodes(n3.get(), n4.get());
  connect_nodes(n3.get(), n6.get());
  connect_nodes(n4.get(), n5.get());
  connect_nodes(n4.get(), n6.get());
  connect_nodes(n5.get(), n6.get());

  // Make 3 blocks
  Node_UPtr a = Node_UPtr(new Node {0, 0, 1 });
  Node_UPtr b = Node_UPtr(new Node {1, 1, 1 });
  Node_UPtr c = Node_UPtr(new Node {2, 2, 1 });

  // Assign nodes to their blocks
  n1->set_parent(a.get());
  n2->set_parent(a.get());
  n3->set_parent(b.get());
  n4->set_parent(b.get());
  n5->set_parent(c.get());
  n6->set_parent(c.get());

  // Move node 4 to c block
  n4->set_parent(c.get());

  const auto a_edges = a->gather_edges_to_level(1);
  REQUIRE(a_edges.size() == 3);
  REQUIRE(a_edges.at(a.get()) == 2 * 1); // self edges will be double
  REQUIRE(a_edges.at(b.get()) == 2);
  REQUIRE(a_edges.at(c.get()) == 4);

  const auto b_edges = b->gather_edges_to_level(1);
  REQUIRE(b_edges.size() == 2);
  REQUIRE(b_edges.at(a.get()) == 2);
  REQUIRE(b_edges.at(c.get()) == 2);

  const auto c_edges = c->gather_edges_to_level(1);
  REQUIRE(c_edges.at(a.get()) == 4);
  REQUIRE(c_edges.at(b.get()) == 2);
  REQUIRE(c_edges.at(c.get()) == 2 * 3);
}

// TEST_CASE("Edge count gathering after moving (unipartite)", "[Node]")
// {
//   Node_UPtr n1 = Node_UPtr(new Node { "n1", 0, 0 });
//   Node_UPtr n2 = Node_UPtr(new Node { "n2", 0, 0 });
//   Node_UPtr n3 = Node_UPtr(new Node { "n3", 0, 0 });
//   Node_UPtr n4 = Node_UPtr(new Node { "n4", 0, 0 });
//   Node_UPtr n5 = Node_UPtr(new Node { "n5", 0, 0 });
//   Node_UPtr n6 = Node_UPtr(new Node { "n6", 0, 0 });

//   // Add edges
//   connect_nodes(n1.get(), n2.get());
//   connect_nodes(n1.get(), n3.get());
//   connect_nodes(n1.get(), n4.get());
//   connect_nodes(n1.get(), n5.get());
//   connect_nodes(n2.get(), n3.get());
//   connect_nodes(n2.get(), n4.get());
//   connect_nodes(n2.get(), n5.get());
//   connect_nodes(n3.get(), n4.get());
//   connect_nodes(n3.get(), n6.get());
//   connect_nodes(n4.get(), n5.get());
//   connect_nodes(n4.get(), n6.get());
//   connect_nodes(n5.get(), n6.get());

//   // Make 3 blocks
//   Node_UPtr a = Node_UPtr(new Node { 0, 1 });
//   Node_UPtr b = Node_UPtr(new Node { 1, 1 });
//   Node_UPtr c = Node_UPtr(new Node { 2, 1 });

//   // Assign nodes to their blocks
//   n1->set_parent(a.get());
//   n2->set_parent(a.get());
//   n3->set_parent(b.get());
//   n4->set_parent(b.get());
//   n5->set_parent(c.get());
//   n6->set_parent(c.get());

//   const auto a_edges = a->gather_edges_to_level(1);
//   REQUIRE(a_edges.size() == 3);
//   REQUIRE(a_edges.at(a.get()) == 2 * 1); // self edges will be double
//   REQUIRE(a_edges.at(b.get()) == 4);
//   REQUIRE(a_edges.at(c.get()) == 2);

//   const auto b_edges = b->gather_edges_to_level(1);
//   REQUIRE(b_edges.size() == 3);
//   REQUIRE(b_edges.at(a.get()) == 4);
//   REQUIRE(b_edges.at(b.get()) == 1 * 2);
//   REQUIRE(b_edges.at(c.get()) == 3);

//   const auto c_edges = c->gather_edges_to_level(1);
//   REQUIRE(c_edges.at(a.get()) == 2);
//   REQUIRE(c_edges.at(b.get()) == 3);
//   REQUIRE(c_edges.at(c.get()) == 2 * 1);
// }

// TEST_CASE("Tracking node degrees", "[Node]")
// {
//   // Node level
//   Node_UPtr a1 = Node_UPtr(new Node { "a1", 0, 0, 2 });
//   Node_UPtr a2 = Node_UPtr(new Node { "a2", 0, 0, 2 });
//   Node_UPtr a3 = Node_UPtr(new Node { "a3", 0, 0, 2 });
//   Node_UPtr b1 = Node_UPtr(new Node { "b1", 0, 1, 2 });
//   Node_UPtr b2 = Node_UPtr(new Node { "b2", 0, 1, 2 });
//   Node_UPtr b3 = Node_UPtr(new Node { "b3", 0, 1, 2 });

//   // First level / blocks
//   Node_UPtr a11 = Node_UPtr(new Node { "a11", 1, 0, 2 });
//   Node_UPtr a12 = Node_UPtr(new Node { "a12", 1, 0, 2 });
//   Node_UPtr b11 = Node_UPtr(new Node { "b11", 1, 1, 2 });
//   Node_UPtr b12 = Node_UPtr(new Node { "b12", 1, 1, 2 });

//   // Second level / super blocks
//   Node_UPtr a21 = Node_UPtr(new Node { "a21", 2, 0, 2 });
//   Node_UPtr b21 = Node_UPtr(new Node { "b21", 2, 1, 2 });

//   connect_nodes(a1.get(), b1.get());
//   connect_nodes(a1.get(), b2.get());
//   connect_nodes(a2.get(), b1.get());
//   connect_nodes(a2.get(), b2.get());
//   connect_nodes(a3.get(), b2.get());
//   connect_nodes(a3.get(), b3.get());

//   a1->set_parent(a11.get());
//   a2->set_parent(a12.get());
//   a3->set_parent(a12.get());

//   b1->set_parent(b11.get());
//   b2->set_parent(b11.get());
//   b3->set_parent(b12.get());

//   a11->set_parent(a21.get());
//   a12->set_parent(a21.get());
//   b11->set_parent(b21.get());
//   b12->set_parent(b21.get());

//   // Get basic info out of the nodes
//   REQUIRE(a1->get_degree() == 2);
//   REQUIRE(a2->get_degree() == 2);
//   REQUIRE(a3->get_degree() == 2);


//   REQUIRE(b1->get_degree() == 2);
//   REQUIRE(b2->get_degree() == 3);
//   REQUIRE(b3->get_degree() == 1);

//   REQUIRE(a11->get_degree() == 2);
//   REQUIRE(a12->get_degree() == 4);
//   REQUIRE(b11->get_degree() == 5);
//   REQUIRE(b12->get_degree() == 1);
//   REQUIRE(a21->get_degree() == 6);
//   REQUIRE(b21->get_degree() == 6);

//   // Swap parents of a2 and b2 nodes
//   a2->set_parent(a11.get());
//   b2->set_parent(b12.get());

//   // Make sure that the degrees correctly reflect change
//   REQUIRE(a1->get_degree() == 2);
//   REQUIRE(a2->get_degree() == 2);
//   REQUIRE(a3->get_degree() == 2);

//   REQUIRE(b1->get_degree() == 2);
//   REQUIRE(b2->get_degree() == 3);
//   REQUIRE(b3->get_degree() == 1);

//   REQUIRE(a11->get_degree() == 4);
//   REQUIRE(a12->get_degree() == 2);
//   REQUIRE(b11->get_degree() == 2);
//   REQUIRE(b12->get_degree() == 4);
//   REQUIRE(a21->get_degree() == 6);
//   REQUIRE(b21->get_degree() == 6);
// }
