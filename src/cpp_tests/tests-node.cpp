#include "../Node.h"
#include "../print_helpers.h"
#include "catch.hpp"

// Smart pointer to node instance
typedef std::shared_ptr<Node> NodePtr;

TEST_CASE("Basic Initialization", "[Node]")
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

  Node::connect_nodes(n1, m1);
  Node::connect_nodes(n1, m3);
  Node::connect_nodes(n2, m1);
  Node::connect_nodes(n3, m2);
  Node::connect_nodes(n3, m3);

  // Get basic info out of the nodes
  REQUIRE(n1->id == "n1");
  REQUIRE(n1->get_parent_at_level(1)->id == n1->parent->id);

  // Make sure the edge propigate properly.
  REQUIRE(
      "m1, m3" == print_node_ids(n1->get_edges_to_level(0)));
  REQUIRE(
      "d1, d2" == print_node_ids(n1->get_edges_to_level(1)));
  REQUIRE(
      "d1, d1, d2" == print_node_ids(c1->get_edges_to_level(1)));
  REQUIRE(
      "d2, d2" == print_node_ids(c2->get_edges_to_level(1)));
}

TEST_CASE("Gathering edge counts to a level", "[Node]")
{
  // Node level
  NodePtr a1 = std::make_shared<Node>("a1", 0, 1);
  NodePtr a2 = std::make_shared<Node>("a2", 0, 1);
  NodePtr a3 = std::make_shared<Node>("a3", 0, 1);
  NodePtr b1 = std::make_shared<Node>("b1", 0, 2);
  NodePtr b2 = std::make_shared<Node>("b2", 0, 2);
  NodePtr b3 = std::make_shared<Node>("b3", 0, 2);

  // First level / blocks
  NodePtr a11 = std::make_shared<Node>("a11", 1, 1);
  NodePtr a12 = std::make_shared<Node>("a12", 1, 1);
  NodePtr b11 = std::make_shared<Node>("b11", 1, 2);
  NodePtr b12 = std::make_shared<Node>("b12", 1, 2);

  // Second level / super blocks
  NodePtr a21 = std::make_shared<Node>("a21", 2, 1);
  NodePtr b21 = std::make_shared<Node>("b21", 2, 2);

  a1->set_parent(a11);
  a2->set_parent(a12);
  a3->set_parent(a12);

  b1->set_parent(b11);
  b2->set_parent(b11);
  b3->set_parent(b12);

  a11->set_parent(a21);
  a12->set_parent(a21);

  b11->set_parent(b21);
  b12->set_parent(b21);

  Node::connect_nodes(a1, b1);
  Node::connect_nodes(a1, b2);
  Node::connect_nodes(a2, b1);
  Node::connect_nodes(a2, b2);
  Node::connect_nodes(a3, b2);
  Node::connect_nodes(a3, b3);

  // Gather all the edges from node a1 to level 1
  NodeEdgeMap a1_to_l1 = a1->gather_edges_to_level(1);

  REQUIRE(
      a1_to_l1[b11] == 2);

  REQUIRE(
      a1_to_l1[b12] == 0);

  // now a3 to l1
  NodeEdgeMap a3_to_l1 = a3->gather_edges_to_level(1);

  REQUIRE(
      a3_to_l1[b11] == 1);

  REQUIRE(
      a3_to_l1[b12] == 1);

  // now b2 to l1
  NodeEdgeMap b2_to_l1 = b2->gather_edges_to_level(1);

  REQUIRE(
      b2_to_l1[a11] == 1);

  REQUIRE(
      b2_to_l1[a12] == 2);

  // Last b11 to a21
  NodeEdgeMap b11_to_l2 = b11->gather_edges_to_level(2);

  REQUIRE(
      b11_to_l2[a21] == 5);
}

TEST_CASE("Edge count gathering (unipartite)", "[Node]")
{

  NodePtr n1 = std::make_shared<Node>("n1", 0, 1);
  NodePtr n2 = std::make_shared<Node>("n2", 0, 1);
  NodePtr n3 = std::make_shared<Node>("n3", 0, 1);
  NodePtr n4 = std::make_shared<Node>("n4", 0, 1);
  NodePtr n5 = std::make_shared<Node>("n5", 0, 1);
  NodePtr n6 = std::make_shared<Node>("n6", 0, 1);

  // Add edges
  Node::connect_nodes(n1, n2);
  Node::connect_nodes(n1, n3);
  Node::connect_nodes(n1, n4);
  Node::connect_nodes(n1, n5);
  Node::connect_nodes(n2, n3);
  Node::connect_nodes(n2, n4);
  Node::connect_nodes(n2, n5);
  Node::connect_nodes(n3, n4);
  Node::connect_nodes(n3, n6);
  Node::connect_nodes(n4, n5);
  Node::connect_nodes(n4, n6);
  Node::connect_nodes(n5, n6);

  // Make 3 blocks
  NodePtr a = std::make_shared<Node>("a", 1, 1);
  NodePtr b = std::make_shared<Node>("b", 1, 1);
  NodePtr c = std::make_shared<Node>("c", 1, 1);

  // Assign nodes to their blocks
  n1->set_parent(a);
  n2->set_parent(a);
  n3->set_parent(b);
  n4->set_parent(b);
  n5->set_parent(c);
  n6->set_parent(c);

  // Move node 4 to c block
  n4->set_parent(c);

  const auto a_edges = a->gather_edges_to_level(1);
  REQUIRE(a_edges.size() == 3);
  REQUIRE(a_edges.at(a) == 2 * 1); // self edges will be double
  REQUIRE(a_edges.at(b) == 2);
  REQUIRE(a_edges.at(c) == 4);

  const auto b_edges = b->gather_edges_to_level(1);
  REQUIRE(b_edges.size() == 2);
  REQUIRE(b_edges.at(a) == 2);
  REQUIRE(b_edges.at(c) == 2);

  const auto c_edges = c->gather_edges_to_level(1);
  REQUIRE(c_edges.at(a) == 4);
  REQUIRE(c_edges.at(b) == 2);
  REQUIRE(c_edges.at(c) == 2 * 3);
}

TEST_CASE("Edge count gathering after moving (unipartite)", "[Node]")
{

  NodePtr n1 = std::make_shared<Node>("n1", 0, 1);
  NodePtr n2 = std::make_shared<Node>("n2", 0, 1);
  NodePtr n3 = std::make_shared<Node>("n3", 0, 1);
  NodePtr n4 = std::make_shared<Node>("n4", 0, 1);
  NodePtr n5 = std::make_shared<Node>("n5", 0, 1);
  NodePtr n6 = std::make_shared<Node>("n6", 0, 1);

  // Add edges
  Node::connect_nodes(n1, n2);
  Node::connect_nodes(n1, n3);
  Node::connect_nodes(n1, n4);
  Node::connect_nodes(n1, n5);
  Node::connect_nodes(n2, n3);
  Node::connect_nodes(n2, n4);
  Node::connect_nodes(n2, n5);
  Node::connect_nodes(n3, n4);
  Node::connect_nodes(n3, n6);
  Node::connect_nodes(n4, n5);
  Node::connect_nodes(n4, n6);
  Node::connect_nodes(n5, n6);

  // Make 3 blocks
  NodePtr a = std::make_shared<Node>("a", 1, 1);
  NodePtr b = std::make_shared<Node>("b", 1, 1);
  NodePtr c = std::make_shared<Node>("c", 1, 1);

  // Assign nodes to their blocks
  n1->set_parent(a);
  n2->set_parent(a);
  n3->set_parent(b);
  n4->set_parent(b);
  n5->set_parent(c);
  n6->set_parent(c);

  const auto a_edges = a->gather_edges_to_level(1);
  REQUIRE(a_edges.size() == 3);
  REQUIRE(a_edges.at(a) == 2 * 1); // self edges will be double
  REQUIRE(a_edges.at(b) == 4);
  REQUIRE(a_edges.at(c) == 2);

  const auto b_edges = b->gather_edges_to_level(1);
  REQUIRE(b_edges.size() == 3);
  REQUIRE(b_edges.at(a) == 4);
  REQUIRE(b_edges.at(b) == 1 * 2);
  REQUIRE(b_edges.at(c) == 3);

  const auto c_edges = c->gather_edges_to_level(1);
  REQUIRE(c_edges.at(a) == 2);
  REQUIRE(c_edges.at(b) == 3);
  REQUIRE(c_edges.at(c) == 2 * 1);
}

TEST_CASE("Tracking node degrees", "[Node]")
{
  // Node level
  NodePtr a1 = std::make_shared<Node>("a1", 0, 1);
  NodePtr a2 = std::make_shared<Node>("a2", 0, 1);
  NodePtr a3 = std::make_shared<Node>("a3", 0, 1);
  NodePtr b1 = std::make_shared<Node>("b1", 0, 2);
  NodePtr b2 = std::make_shared<Node>("b2", 0, 2);
  NodePtr b3 = std::make_shared<Node>("b3", 0, 2);

  // First level / blocks
  NodePtr a11 = std::make_shared<Node>("a11", 1, 1);
  NodePtr a12 = std::make_shared<Node>("a12", 1, 1);
  NodePtr b11 = std::make_shared<Node>("b11", 1, 2);
  NodePtr b12 = std::make_shared<Node>("b12", 1, 2);

  // Second level / super blocks
  NodePtr a21 = std::make_shared<Node>("a21", 2, 1);
  NodePtr b21 = std::make_shared<Node>("b21", 2, 2);

  a1->set_parent(a11);
  a2->set_parent(a12);
  a3->set_parent(a12);

  b1->set_parent(b11);
  b2->set_parent(b11);
  b3->set_parent(b12);

  a11->set_parent(a21);
  a12->set_parent(a21);

  b11->set_parent(b21);
  b12->set_parent(b21);

  Node::connect_nodes(a1, b1);
  Node::connect_nodes(a1, b2);
  Node::connect_nodes(a2, b1);
  Node::connect_nodes(a2, b2);
  Node::connect_nodes(a3, b2);
  Node::connect_nodes(a3, b3);

  // Get basic info out of the nodes
  REQUIRE(a1->degree == 2);
  REQUIRE(a2->degree == 2);
  REQUIRE(a3->degree == 2);

  REQUIRE(a1->degree == (a1->edges).size());
  REQUIRE(a2->degree == (a2->edges).size());
  REQUIRE(a3->degree == (a3->edges).size());

  REQUIRE(b1->degree == 2);
  REQUIRE(b2->degree == 3);
  REQUIRE(b3->degree == 1);

  REQUIRE(a11->degree == 2);
  REQUIRE(a12->degree == 4);
  REQUIRE(b11->degree == 5);
  REQUIRE(b12->degree == 1);
  REQUIRE(a21->degree == 6);
  REQUIRE(b21->degree == 6);

  REQUIRE(a11->degree == (a11->edges).size());
  REQUIRE(a12->degree == (a12->edges).size());
  REQUIRE(b11->degree == (b11->edges).size());
  REQUIRE(b12->degree == (b12->edges).size());
  REQUIRE(a21->degree == (a21->edges).size());
  REQUIRE(b21->degree == (b21->edges).size());

  // Swap parents of a2 and b2 nodes
  a2->set_parent(a11);
  b2->set_parent(b12);

  // Make sure that the degrees correctly reflect change
  REQUIRE(a1->degree == 2);
  REQUIRE(a2->degree == 2);
  REQUIRE(a3->degree == 2);

  REQUIRE(b1->degree == 2);
  REQUIRE(b2->degree == 3);
  REQUIRE(b3->degree == 1);

  REQUIRE(a11->degree == 4);
  REQUIRE(a12->degree == 2);
  REQUIRE(b11->degree == 2);
  REQUIRE(b12->degree == 4);
  REQUIRE(a21->degree == 6);
  REQUIRE(b21->degree == 6);

  REQUIRE(a11->degree == (a11->edges).size());
  REQUIRE(a12->degree == (a12->edges).size());
  REQUIRE(b11->degree == (b11->edges).size());
  REQUIRE(b12->degree == (b12->edges).size());
  REQUIRE(a21->degree == (a21->edges).size());
  REQUIRE(b21->degree == (b21->edges).size());
}
