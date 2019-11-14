#include<gtest/gtest.h>
#include "../helpers.h"
#include "../Node.h"
#include <memory>

// Smart pointer to node instance
typedef std::shared_ptr<Node> NodePtr;

TEST(testNode, basic){
  
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
  EXPECT_EQ("n1", n1->id);
  EXPECT_EQ(n1->get_parent_at_level(1)->id, n1->parent->id);
  EXPECT_EQ(2, c1->get_children_at_level(0).size());
  EXPECT_EQ(1, d1->get_children_at_level(0).size());

  // Make sure the connection propigate properly.
  EXPECT_EQ(
    "m1, m3",
    print_node_ids(n1->get_connections_to_level(0))
  );
  EXPECT_EQ(
    "d1, d2",
    print_node_ids(n1->get_connections_to_level(1))
  );
  EXPECT_EQ(
    "d1, d1, d2",
    print_node_ids(c1->get_connections_to_level(1))
  );
  EXPECT_EQ(
    "d2, d2",
    print_node_ids(c2->get_connections_to_level(1))
  );
}

TEST(testNode, edge_counts_to_level){
  
  // Node level
  NodePtr a1 = std::make_shared<Node>("a1", 0, 1);
  NodePtr a2 = std::make_shared<Node>("a2", 0, 1);
  NodePtr a3 = std::make_shared<Node>("a3", 0, 1);
  NodePtr b1 = std::make_shared<Node>("b1", 0, 2);
  NodePtr b2 = std::make_shared<Node>("b2", 0, 2);
  NodePtr b3 = std::make_shared<Node>("b3", 0, 2);
  
  // First level / groups
  NodePtr a11 = std::make_shared<Node>("a11", 1, 1);
  NodePtr a12 = std::make_shared<Node>("a12", 1, 1);
  NodePtr b11 = std::make_shared<Node>("b11", 1, 2);
  NodePtr b12 = std::make_shared<Node>("b12", 1, 2);
  
  // Second level / super groups
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
  std::map<NodePtr, int> a1_to_l1 = a1->gather_connections_to_level(1);
  
  EXPECT_EQ(
    a1_to_l1[b11],
    2
  );
  
  EXPECT_EQ(
    a1_to_l1[b12],
    0
  );
  
  // now a3 to l1
  std::map<NodePtr, int> a3_to_l1 = a3->gather_connections_to_level(1);
  
  EXPECT_EQ(
    a3_to_l1[b11],
    1
  );
  
  EXPECT_EQ(
    a3_to_l1[b12],
    1
  );
  
  // now b2 to l1
  std::map<NodePtr, int> b2_to_l1 = b2->gather_connections_to_level(1);
  
  EXPECT_EQ(
    b2_to_l1[a11],
    1
  );
  
  EXPECT_EQ(
    b2_to_l1[a12],
    2
  );
  
  // Last b11 to a21
  std::map<NodePtr, int> b11_to_l2 = b11->gather_connections_to_level(2);
  
  EXPECT_EQ(
    b11_to_l2[a21],
    5
  );
  
  double tol = 0.01;
  // Also make sure fraction of connections to a given node works as well. 
  ASSERT_NEAR(
    a1->frac_of_connections_to_group(b11),
    1.0,
    tol
  );
  
  ASSERT_NEAR(
    a1->frac_of_connections_to_group(b12),
    0.0,
    tol
  );
  
  ASSERT_NEAR(
    a3->frac_of_connections_to_group(b11),
    0.5,
    tol
  );
  
  ASSERT_NEAR(
    a3->frac_of_connections_to_group(a11),
    0.0,
    tol
  );
  
}

TEST(testNode, degree_tracking){
  
  // Node level
  NodePtr a1 = std::make_shared<Node>("a1", 0, 1);
  NodePtr a2 = std::make_shared<Node>("a2", 0, 1);
  NodePtr a3 = std::make_shared<Node>("a3", 0, 1);
  NodePtr b1 = std::make_shared<Node>("b1", 0, 2);
  NodePtr b2 = std::make_shared<Node>("b2", 0, 2);
  NodePtr b3 = std::make_shared<Node>("b3", 0, 2);
  
  // First level / groups
  NodePtr a11 = std::make_shared<Node>("a11", 1, 1);
  NodePtr a12 = std::make_shared<Node>("a12", 1, 1);
  NodePtr b11 = std::make_shared<Node>("b11", 1, 2);
  NodePtr b12 = std::make_shared<Node>("b12", 1, 2);
  
  // Second level / super groups
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
  EXPECT_EQ(a1->degree, 2);
  EXPECT_EQ(a2->degree, 2);
  EXPECT_EQ(a3->degree, 2);
  
  EXPECT_EQ(b1->degree, 2);
  EXPECT_EQ(b2->degree, 3);
  EXPECT_EQ(b3->degree, 1);
  
  EXPECT_EQ(a11->degree, 2);
  EXPECT_EQ(a12->degree, 4);
  
  EXPECT_EQ(b11->degree, 5);
  EXPECT_EQ(b12->degree, 1);
  
  EXPECT_EQ(a21->degree, 6);
  EXPECT_EQ(b21->degree, 6);
  
  
  // Swap parents of a2 and b2 nodes
  a2->set_parent(a11);
  b2->set_parent(b12);
  
  // Make sure that the degrees correctly reflect change
  EXPECT_EQ(a1->degree, 2);
  EXPECT_EQ(a2->degree, 2);
  EXPECT_EQ(a3->degree, 2);
  
  EXPECT_EQ(b1->degree, 2);
  EXPECT_EQ(b2->degree, 3);
  EXPECT_EQ(b3->degree, 1);
  
  EXPECT_EQ(a11->degree, 4);
  EXPECT_EQ(a12->degree, 2);
  
  EXPECT_EQ(b11->degree, 2);
  EXPECT_EQ(b12->degree, 4);
  
  EXPECT_EQ(a21->degree, 6);
  EXPECT_EQ(b21->degree, 6);
}

int main(int argc, char* argv[]){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
