#include "../network.h"
#include "../swap_blocks.h"
#include "catch.hpp"

TEST_CASE("Basic initialization of network", "[Network]")
{
  SBM_Network my_net(42);

  // Add some nodes to Network
  my_net.add_node("n1", "n");
  my_net.add_node("n2", "n");
  my_net.add_node("n3", "n");
  my_net.add_node("m1", "m");
  my_net.add_node("m2", "m");
  my_net.add_node("m3", "m");
  my_net.add_node("m4", "m");


  REQUIRE(my_net.num_nodes() == 7);
  REQUIRE(my_net.num_nodes_at_level(0) == 7);
  REQUIRE(my_net.num_nodes_of_type("m", 0) == 4);
  REQUIRE(my_net.num_nodes_of_type("n", 0) == 3);

  my_net.initialize_blocks(2);

  // We should now have added four new nodes at the block level (1)
  REQUIRE(my_net.num_nodes() == 11);
  REQUIRE(my_net.num_nodes_at_level(0) == 7);
  REQUIRE(my_net.num_nodes_at_level(1) == 4);
  REQUIRE(my_net.num_nodes_of_type("m", 1) == 2);
  REQUIRE(my_net.num_nodes_of_type("n", 1) == 2);

  // We should have two levels
  REQUIRE(my_net.num_levels() == 2);
}

TEST_CASE("Tracking node types", "[Network]")
{
  SBM_Network my_net;

  // Add some nodes to Network
  my_net.add_node("n1", "n");
  my_net.add_node("n2", "n");

  // There should only be one type of node so far
  REQUIRE(my_net.num_types() == 1);

  my_net.add_node("m1", "m");

  // There should now be two types of nodes
  REQUIRE(my_net.num_types() == 2);

  my_net.add_node("m2", "m");
  my_net.add_node("n3", "n");

  // There should still just be two types of nodes
  REQUIRE(my_net.num_types() == 2);

  my_net.add_node("m3", "m");
  my_net.add_node("o1", "o");
  my_net.add_node("o2", "o");

  // There should now be three types of nodes
  REQUIRE(my_net.num_types() == 3);
}

TEST_CASE("Initializing a block for every node", "[Network]")
{
  SBM_Network my_net;

  my_net.add_node("a1", "a");
  my_net.add_node("a2", "a");
  my_net.add_node("a3", "a");
  my_net.add_node("a4", "a");
  my_net.add_node("a5", "a");
  my_net.add_node("a10", "a");
  my_net.add_node("a11", "a");
  my_net.add_node("a13", "a");
  my_net.add_node("a14", "a");
  my_net.add_node("a6", "a");
  my_net.add_node("a7", "a");
  my_net.add_node("a8", "a");
  my_net.add_node("a9", "a");
  my_net.add_node("a12", "a");
  my_net.add_node("b1", "b");
  my_net.add_node("b2", "b");
  my_net.add_node("b3", "b");
  my_net.add_node("b4", "b");

  // There should be a total of 18 nodes at base level
  REQUIRE(my_net.num_nodes_at_level(0) == 18);

  // And zero nodes at the block level
  REQUIRE(my_net.num_levels() == 1);

  // Now assiging every node their own parent block
  my_net.initialize_blocks();

  // There should now be a second level with 18 nodes
  REQUIRE(my_net.num_levels() == 2);
  REQUIRE(my_net.num_nodes_at_level(1) == 18);
}

TEST_CASE("Randomly assigning a given number of blocks", "[Network]")
{
  SBM_Network my_net;

  my_net.add_node("a1", "a");
  my_net.add_node("a2", "a");
  my_net.add_node("a3", "a");
  my_net.add_node("a4", "a");
  my_net.add_node("a5", "a");
  my_net.add_node("a6", "a");
  my_net.add_node("a7", "a");
  my_net.add_node("a8", "a");
  my_net.add_node("a9", "a");
  my_net.add_node("a10", "a");

  my_net.add_node("b1", "b");
  my_net.add_node("b2", "b");
  my_net.add_node("b3", "b");
  my_net.add_node("b4", "b");
  my_net.add_node("b5", "b");
  my_net.add_node("b6", "b");
  my_net.add_node("b7", "b");
  my_net.add_node("b8", "b");
  my_net.add_node("b9", "b");
  my_net.add_node("b10", "b");

  // Distribute 3 total blocks for each type across nodes randomly
  my_net.initialize_blocks(3);

  // There should now be three block nodes of each type
  REQUIRE(my_net.num_nodes_of_type("a", 1) == 3);
  REQUIRE(my_net.num_nodes_of_type("b", 1) == 3);
}

TEST_CASE("Metablock initialization", "[Network]")
{
  SBM_Network my_net(42);

  // Add some nodes to Network
  my_net.add_node("n1", "n");
  my_net.add_node("n2", "n");
  my_net.add_node("n3", "n");
  my_net.add_node("m1", "m");
  my_net.add_node("m2", "m");
  my_net.add_node("m3", "m");
  my_net.add_node("m4", "m");

  my_net.initialize_blocks(3);

  // We should now have added six new blocks
  REQUIRE(my_net.num_levels() == 2);
  REQUIRE(my_net.num_nodes_at_level(1) == 6);

  // Now we can initialize metablocks for those blocks
  my_net.initialize_blocks(2);

  // We should now have three levels with the third having 4 blocks
  REQUIRE(my_net.num_levels() == 3);
  REQUIRE(my_net.num_nodes_at_level(2) == 4);

  // Now remove the metablocks
  my_net.delete_blocks();

  // Should be back to two levels
  REQUIRE(my_net.num_levels() == 2);

  // Remove the blocks
  my_net.delete_blocks();
  REQUIRE(my_net.num_levels() == 1);

  // Cant remove blocks when no levels are left
  REQUIRE_THROWS(my_net.delete_blocks());
}


TEST_CASE("Swapping of blocks", "[Network]")
{
  SBM_Network my_net(42);

  // Add some nodes to Network
  my_net.add_node("n1", "n");
  my_net.add_node("n2", "n");
  my_net.add_node("n3", "n");
  my_net.add_node("m1", "m");
  my_net.add_node("m2", "m");
  my_net.add_node("m3", "m");

  my_net.initialize_blocks();

  // Make sure network is how we desired it to be
  REQUIRE(my_net.num_nodes_of_type("n", 1) == 3);
  REQUIRE(my_net.num_nodes_of_type("m", 1) == 3);

  // Merge second node into first nodes block
  Node* node2 = my_net.get_nodes_of_type("n")[1].get();
  Node* block1 = my_net.get_nodes_of_type("n")[0]->get_parent();

  swap_blocks(node2,
              block1,
              my_net.get_nodes_of_type("n", 1),
              true);

  // There should now be one less block of type n
  REQUIRE(my_net.num_nodes_of_type("n", 1) == 2);

  // Now do the same for the m type nodes but don't delete the empty block
  swap_blocks(my_net.get_nodes_of_type("m")[1].get(),
              my_net.get_nodes_of_type("m")[0]->get_parent(),
              my_net.get_nodes_of_type("m", 1),
              false);

  // There should be no change in the number of blocks
  REQUIRE(my_net.num_nodes_of_type("m", 1) == 3);
}




// TEST_CASE("Cleaning up empty blocks", "[Network]")
// {
//   SBM my_net;

//   // Start with a few nodes in the network
//   NodePtr n1 = my_net.add_node("n1", "a");
//   NodePtr n2 = my_net.add_node("n2", "a");
//   NodePtr n3 = my_net.add_node("n3", "a");
//   NodePtr n4 = my_net.add_node("n4", "a");

//   // Create a few block nodes at first level
//   NodePtr g1_1 = my_net.create_block_node("a", 1);
//   NodePtr g1_2 = my_net.create_block_node("a", 1);
//   NodePtr g1_3 = my_net.create_block_node("a", 1);
//   NodePtr g1_4 = my_net.create_block_node("a", 1);

//   // Create two blocks for second level
//   NodePtr g2_1 = my_net.create_block_node("a", 2);
//   NodePtr g2_2 = my_net.create_block_node("a", 2);

//   // Add children to blocks 1 and two at first level
//   n1->set_parent(g1_1);
//   n2->set_parent(g1_1);
//   n3->set_parent(g1_2);
//   n4->set_parent(g1_2);

//   // Add children to both level two blocks
//   g1_1->set_parent(g2_1);
//   g1_2->set_parent(g2_1);
//   g1_3->set_parent(g2_1);
//   g1_4->set_parent(g2_2);

//   // Make sure our SBM is the proper size
//   REQUIRE(3 == my_net.nodes.size());
//   REQUIRE(4 == my_net.nodes.at(0)->size());
//   REQUIRE(4 == my_net.nodes.at(1)->size());
//   REQUIRE(2 == my_net.nodes.at(2)->size());

//   // Run block cleanup
//   my_net.clean_empty_blocks();

//   // Two should have been taken from the first block level
//   REQUIRE(2 == my_net.nodes.at(1)->size());

//   // And 1 should have been taken from the second block level
//   REQUIRE(1 == my_net.nodes.at(2)->size());
// }

// TEST_CASE("Counting edges", "[Network]")
// {
//   SBM my_net;

//   // Base-level nodes
//   NodePtr a1 = my_net.add_node("a1", "a");
//   NodePtr a2 = my_net.add_node("a2", "a");
//   NodePtr a3 = my_net.add_node("a3", "a");
//   NodePtr a4 = my_net.add_node("a4", "a");
//   NodePtr a5 = my_net.add_node("a5", "a");

//   NodePtr b1 = my_net.add_node("b1", "b");
//   NodePtr b2 = my_net.add_node("b2", "b");
//   NodePtr b3 = my_net.add_node("b3", "b");
//   NodePtr b4 = my_net.add_node("b4", "b");
//   NodePtr b5 = my_net.add_node("b5", "b");

//   // level one blocks
//   NodePtr a11 = my_net.add_node("a11", "a", 1);
//   NodePtr a12 = my_net.add_node("a12", "a", 1);
//   NodePtr a13 = my_net.add_node("a13", "a", 1);
//   NodePtr b11 = my_net.add_node("b11", "b", 1);
//   NodePtr b12 = my_net.add_node("b12", "b", 1);
//   NodePtr b13 = my_net.add_node("b13", "b", 1);

//   // level two blocks
//   NodePtr a21 = my_net.add_node("a21", "a", 2);
//   NodePtr a22 = my_net.add_node("a22", "a", 2);
//   NodePtr b21 = my_net.add_node("b21", "b", 2);
//   NodePtr b22 = my_net.add_node("b22", "b", 2);

//   // Add edges
//   my_net.add_edge("a1", "b1");
//   my_net.add_edge("a1", "b2");
//   my_net.add_edge("a2", "b1");
//   my_net.add_edge("a2", "b3");
//   my_net.add_edge("a2", "b5");
//   my_net.add_edge("a3", "b2");
//   my_net.add_edge("a4", "b4");
//   my_net.add_edge("a4", "b5");
//   my_net.add_edge("a5", "b3");

//   // Set hierarchy

//   // Nodes -> level 1
//   a1->set_parent(a11);

//   a2->set_parent(a12);
//   a3->set_parent(a12);

//   a4->set_parent(a13);
//   a5->set_parent(a13);

//   b1->set_parent(b11);
//   b2->set_parent(b11);

//   b3->set_parent(b12);

//   b4->set_parent(b13);
//   b5->set_parent(b13);

//   // level 1 -> level 2
//   a11->set_parent(a21);
//   a12->set_parent(a21);
//   a13->set_parent(a22);

//   b11->set_parent(b21);
//   b12->set_parent(b21);
//   b13->set_parent(b22);

//   // Make sure our SBM is the proper size

//   // There should be three total layers...
//   REQUIRE(3 == my_net.nodes.size());
//   REQUIRE(3 == my_net.node_type_counts.at("a").size());

//   // 10 nodes at first level...
//   REQUIRE(10 == my_net.nodes.at(0)->size());
//   REQUIRE(6 == my_net.nodes.at(1)->size());
//   REQUIRE(4 == my_net.nodes.at(2)->size());

//   // Make sure node degrees are correct
//   REQUIRE(a11->degree == 2);
//   REQUIRE(a12->degree == 4);
//   REQUIRE(a13->degree == 3);
//   REQUIRE(b11->degree == 4);
//   REQUIRE(b12->degree == 2);
//   REQUIRE(b13->degree == 3);

//   REQUIRE(a21->degree == 6);
//   REQUIRE(a22->degree == 3);
//   REQUIRE(b21->degree == 6);
//   REQUIRE(b22->degree == 3);

//   // Gather edges between all blocks at level one
//   const auto level_1_counts = my_net.get_block_edge_counts(1);

//   REQUIRE(level_1_counts.size() > 1);

//   // Check num edges between blocks
//   auto a11_edges = a11->gather_edges_to_level(1);
//   REQUIRE(a11_edges[b11] == 2);
//   REQUIRE(level_1_counts.at(Edge(a11, b11)) == 2);
//   REQUIRE(a11_edges[b12] == 0);
//   REQUIRE(a11_edges[b13] == 0);

//   auto a12_edges = a12->gather_edges_to_level(1);
//   REQUIRE(a12_edges[b11] == 2);
//   REQUIRE(level_1_counts.at(Edge(a12, b11)) == 2);

//   REQUIRE(a12_edges[b12] == 1);
//   REQUIRE(level_1_counts.at(Edge(a12, b12)) == 1);

//   REQUIRE(a12_edges[b13] == 1);
//   REQUIRE(level_1_counts.at(Edge(a12, b13)) == 1);

//   auto a13_edges = a13->gather_edges_to_level(1);
//   REQUIRE(a13_edges[b11] == 0);
//   REQUIRE(a13_edges[b12] == 1);
//   REQUIRE(level_1_counts.at(Edge(a13, b12)) == 1);

//   REQUIRE(a13_edges[b13] == 2);
//   REQUIRE(level_1_counts.at(Edge(a13, b13)) == 2);

//   // Direction shouldn't matter
//   REQUIRE(a11->gather_edges_to_level(1)[b11] == b11->gather_edges_to_level(1)[a11]);
//   REQUIRE(level_1_counts.at(Edge(a13, b13)) == level_1_counts.at(Edge(b13, a13)));

//   // Direction shouldn't matter
//   REQUIRE(a11->gather_edges_to_level(1)[b12] == b12->gather_edges_to_level(1)[a11]);

//   // Repeat for level 2
//   auto a21_edges = a21->gather_edges_to_level(2);
//   REQUIRE(a21_edges[b21] == 5);
//   REQUIRE(a21_edges[b22] == 1);

//   auto a22_edges = a22->gather_edges_to_level(2);
//   REQUIRE(a22_edges[b21] == 1);
//   REQUIRE(a22_edges[b22] == 2);

//   // Now we will change the block for a node and make sure the changes are properly detected

//   // Update the level 1 edge counts
//   a3->set_parent(a13);

//   // Make sure node degrees are correct
//   REQUIRE(a11->degree == 2);
//   REQUIRE(a12->degree == 3);
//   REQUIRE(a13->degree == 4);
//   REQUIRE(b11->degree == 4);
//   REQUIRE(b12->degree == 2);
//   REQUIRE(b13->degree == 3);

//   REQUIRE(a21->degree == 5);
//   REQUIRE(a22->degree == 4);
//   REQUIRE(b21->degree == 6);
//   REQUIRE(b22->degree == 3);

//   // Check num edges between blocks
//   auto a11_edges_new = a11->gather_edges_to_level(1);
//   REQUIRE(a11_edges_new[b11] == 2);
//   REQUIRE(a11_edges_new[b12] == 0);
//   REQUIRE(a11_edges_new[b13] == 0);

//   auto a12_edges_new = a12->gather_edges_to_level(1);
//   REQUIRE(a12_edges_new[b11] == 1);
//   REQUIRE(a12_edges_new[b12] == 1);
//   REQUIRE(a12_edges_new[b13] == 1);

//   auto a13_edges_new = a13->gather_edges_to_level(1);
//   REQUIRE(a13_edges_new[b11] == 1);
//   REQUIRE(a13_edges_new[b12] == 1);
//   REQUIRE(a13_edges_new[b13] == 2);

//   // Repeat for level 2
//   auto a21_edges_new = a21->gather_edges_to_level(2);
//   REQUIRE(a21_edges_new[b21] == 4);
//   REQUIRE(a21_edges_new[b22] == 1);

//   auto a22_edges_new = a22->gather_edges_to_level(2);
//   REQUIRE(a22_edges_new[b21] == 2);
//   REQUIRE(a22_edges_new[b22] == 2);
// }

// TEST_CASE("State dumping and restoring", "[Network")
// {
//   SBM my_net;

//   // Start with a few nodes in the network
//   NodePtr a1 = my_net.add_node("a1", "a");
//   NodePtr a2 = my_net.add_node("a2", "a");
//   NodePtr a3 = my_net.add_node("a3", "a");

//   NodePtr b1 = my_net.add_node("b1", "b");
//   NodePtr b2 = my_net.add_node("b2", "b");
//   NodePtr b3 = my_net.add_node("b3", "b");

//   NodePtr a11 = my_net.add_node("a11", "a", 1);
//   NodePtr a12 = my_net.add_node("a12", "a", 1);
//   NodePtr a13 = my_net.add_node("a13", "a", 1);

//   NodePtr b11 = my_net.add_node("b11", "b", 1);
//   NodePtr b12 = my_net.add_node("b12", "b", 1);
//   NodePtr b13 = my_net.add_node("b13", "b", 1);

//   // Assign simple block structure
//   a1->set_parent(a11);
//   a2->set_parent(a12);
//   a3->set_parent(a13);

//   b1->set_parent(b11);
//   b2->set_parent(b12);
//   b3->set_parent(b13);

//   // Dump model state
//   State_Dump state1 = my_net.get_state();

//   // Test state dump is in correct form
//   REQUIRE(
//       print_ids_to_string(state1.id) == "a1, a11, a12, a13, a2, a3, b1, b11, b12, b13, b2, b3");

//   REQUIRE(
//       print_ids_to_string(state1.parent) == "a11, a12, a13, b11, b12, b13, none, none, none, none, none, none");

//   // Now give node a1 a different parent
//   NodePtr a14 = my_net.add_node("a14", "a", 1);
//   a1->set_parent(a14);

//   // Dump model state again
//   State_Dump state2 = my_net.get_state();

//   // Make sure new parent for a1 is reflected in new state dump
//   REQUIRE(
//       print_ids_to_string(state2.id) == "a1, a11, a12, a13, a14, a2, a3, b1, b11, b12, b13, b2, b3");

//   REQUIRE(
//       print_ids_to_string(state2.parent) == "a12, a13, a14, b11, b12, b13, none, none, none, none, none, none, none");

//   // Now restore model to pre a1->a14 move state
//   my_net.set_state(state1.id, state1.parent, state1.level, state1.type);
 
//   State_Dump state3 = my_net.get_state();

//   // Make sure state dumps 1 and 3 match state dump is in correct form
//   REQUIRE(
//       print_ids_to_string(state1.id) == print_ids_to_string(state3.id));

//   REQUIRE(
//       print_ids_to_string(state1.parent) == print_ids_to_string(state3.parent));
// }
