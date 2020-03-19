#include "../network.h"
#include "../swap_blocks.h"
#include "catch.hpp"
#include <set>

TEST_CASE("Basic initialization of network", "[Network]")
{
  SBM_Network my_net({ "m", "n" }, 42);

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

TEST_CASE("Default block initialization", "[Network]")
{
  SBM_Network my_net({ "a", "b" }, 42);

  // Start with a few nodes in the network
  my_net.add_node("a1", "a");
  my_net.add_node("a2", "a");
  my_net.add_node("a3", "a");
  my_net.add_node("b1", "b");
  my_net.add_node("b2", "b");
  my_net.add_node("b3", "b");

  // Give every node its own block
  my_net.initialize_blocks();

  // There should be same number of nodes at block level as data level
  REQUIRE(my_net.num_nodes_at_level(0) == my_net.num_nodes_at_level(1));

  // Make sure every node has a parent node
  REQUIRE(my_net.get_node_by_id("a1")->has_parent());
  REQUIRE(my_net.get_node_by_id("a2")->has_parent());
  REQUIRE(my_net.get_node_by_id("a3")->has_parent());
  REQUIRE(my_net.get_node_by_id("b1")->has_parent());
  REQUIRE(my_net.get_node_by_id("b2")->has_parent());
  REQUIRE(my_net.get_node_by_id("b3")->has_parent());

  // All parents should be unique
  std::set<string> unique_blocks { my_net.get_node_by_id("a1")->parent()->id(),
                                   my_net.get_node_by_id("a2")->parent()->id(),
                                   my_net.get_node_by_id("a3")->parent()->id(),
                                   my_net.get_node_by_id("b1")->parent()->id(),
                                   my_net.get_node_by_id("b2")->parent()->id(),
                                   my_net.get_node_by_id("b3")->parent()->id() };

  REQUIRE(unique_blocks.size() == my_net.num_nodes_at_level(1));
}

TEST_CASE("Initializing a block for every node", "[Network]")
{
  SBM_Network my_net({ "a", "b" }, 42);

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
  SBM_Network my_net({ "a", "b" }, 42);

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
  SBM_Network my_net({ "m", "n" }, 42);

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
  my_net.delete_block_level();

  // Should be back to two levels
  REQUIRE(my_net.num_levels() == 2);

  // Remove the blocks
  my_net.delete_block_level();
  REQUIRE(my_net.num_levels() == 1);

  // Cant remove blocks when no levels are left
  REQUIRE_THROWS(my_net.delete_block_level());
}

TEST_CASE("Swapping of blocks", "[Network]")
{
  SBM_Network my_net({ "m", "n" }, 42);

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
  Node* node2  = my_net.get_node_by_id("n2");
  Node* block1 = my_net.get_node_by_id("n1")->parent();

  swap_blocks(node2,
              block1,
              my_net.get_nodes_of_type("n", 1),
              true);

  // There should now be one less block of type n
  REQUIRE(my_net.num_nodes_of_type("n", 1) == 2);

  // Now do the same for the m type nodes but don't delete the empty block
  swap_blocks(my_net.get_nodes_of_type("m")[1].get(),
              my_net.get_nodes_of_type("m")[0]->parent(),
              my_net.get_nodes_of_type("m", 1),
              false);

  // There should be no change in the number of blocks
  REQUIRE(my_net.num_nodes_of_type("m", 1) == 3);
}

bool state_has_entry(const State_Dump& state,
                     const string& id,
                     const string& type)
{
  for (int i = 0; i < state.size(); i++) {
    if (state.ids[i] == id && state.types[i] == type) {
      return true;
    }
  }
  return false;
}

void print_state(const State_Dump& state)
{
  for (int i = 0; i < state.size(); i++) {
    OUT_MSG << "id: " << state.ids[i]
            << ", type: " << state.types[i]
            << ", parent: " << state.parents[i]
            << ", level: " << state.levels[i]
            << std::endl;
  }
}

int index_of_node(const State_Dump& state, const string& node_id)
{
  for (int i = 0; i < state.size(); i++) {
    if (state.ids[i] == node_id)
      return i;
  }
  RANGE_ERROR("Could not find node.");
  return -1;
}

string parent_from_state(const State_Dump& state, const string& node_id)
{
  return state.parents[index_of_node(state, node_id)];
}

TEST_CASE("State dumping and restoring", "[Network")
{
  SBM_Network my_net({ "a", "b" }, 42);

  // Start with a few nodes in the network
  my_net.add_node("a1", "a");
  my_net.add_node("a2", "a");
  my_net.add_node("a3", "a");

  my_net.add_node("b1", "b");
  my_net.add_node("b2", "b");
  my_net.add_node("b3", "b");

  // Give every node its own block
  my_net.initialize_blocks();

  // Dump model state
  State_Dump state1 = my_net.state();

  // Test state dump is in correct form
  REQUIRE(state_has_entry(state1, "a1", "a"));
  REQUIRE(state_has_entry(state1, "a2", "a"));
  REQUIRE(state_has_entry(state1, "a3", "a"));
  REQUIRE(state_has_entry(state1, "b1", "b"));
  REQUIRE(state_has_entry(state1, "b2", "b"));
  REQUIRE(state_has_entry(state1, "b3", "b"));

  Node* a1 = my_net.get_node_by_id("a1");
  Node* a2 = my_net.get_node_by_id("a2");

  // Now give merge a1 and a2 to same parent and remove a1s old parent
  swap_blocks(a1,
              a2->parent(),
              my_net.get_nodes_of_type("a", 1),
              true);

  // Dump model state again
  State_Dump state2 = my_net.state();

  // a1's parent changed
  REQUIRE(parent_from_state(state1, "a1") != parent_from_state(state2, "a1"));

  // a2's parent did not change
  REQUIRE(parent_from_state(state1, "a2") == parent_from_state(state2, "a2"));

  // a1 and a2 now share a parent
  REQUIRE(parent_from_state(state2, "a1") == parent_from_state(state2, "a2"));

  // Restore to original state
  my_net.update_state(state1);
  REQUIRE(my_net.num_levels() == 2);

  State_Dump state3 = my_net.state();

  // a1's parent is same as original state
  REQUIRE(parent_from_state(state3, "a1") == parent_from_state(state1, "a1"));

  // Which is not the same as a2's parent anymore
  REQUIRE(parent_from_state(state3, "a1") != parent_from_state(state3, "a2"));

  // The size of the network is correct...
  REQUIRE(my_net.num_nodes_at_level(1) == 6);

  // We can also build a brand new network without any previous groups and have it assume
  // the state from before

  SBM_Network my_net2({ "a", "b" }, 42);

  // Start with a few nodes in the network
  my_net2.add_node("a1", "a");
  my_net2.add_node("a2", "a");
  my_net2.add_node("a3", "a");
  my_net2.add_node("b1", "b");
  my_net2.add_node("b3", "b");
  my_net2.add_node("b2", "b");

  REQUIRE(my_net2.num_levels() == 1);

  my_net2.update_state(state2);
  REQUIRE(my_net2.num_levels() == 2);
  REQUIRE(my_net2.num_nodes_at_level(1) == 5);

  REQUIRE(my_net2.get_node_by_id("a1")->parent()
          == my_net2.get_node_by_id("a2")->parent());
}

TEST_CASE("State dumping and restoring: w/ metablocks", "[Network")
{
  SBM_Network my_net({ "a", "b" }, 42);

  // Start with a few nodes in the network
  my_net.add_node("a1", "a");
  my_net.add_node("a2", "a");
  my_net.add_node("a3", "a");
  my_net.add_node("a4", "a");

  my_net.add_node("b1", "b");
  my_net.add_node("b2", "b");
  my_net.add_node("b3", "b");
  my_net.add_node("b4", "b");

  // Give every node its own block
  my_net.initialize_blocks();

  // Distribute two metablocks per type accross the blocks
  my_net.initialize_blocks(2);

  REQUIRE(my_net.num_levels() == 3);
  REQUIRE(my_net.num_nodes_at_level(0) == 8);
  REQUIRE(my_net.num_nodes_at_level(1) == 8);
  REQUIRE(my_net.num_nodes_at_level(2) == 4);

  // Dump model state
  State_Dump state1 = my_net.state();

  // Build a new model that doesn't have any state
  SBM_Network my_net2({ "a", "b" }, 42);

  // Start with a few nodes in the network
  my_net2.add_node("a1", "a");
  my_net2.add_node("a2", "a");
  my_net2.add_node("a3", "a");
  my_net2.add_node("a4", "a");
  my_net2.add_node("b1", "b");
  my_net2.add_node("b2", "b");
  my_net2.add_node("b3", "b");
  my_net2.add_node("b4", "b");

  REQUIRE(my_net2.num_levels() == 1);
  REQUIRE(my_net2.num_nodes_at_level(0) == 8);

  // Load dumped state from first model to this new model
  my_net2.update_state(state1);

  // Now check to make sure the size matches the previous model
  REQUIRE(my_net2.num_levels() == 3);
  REQUIRE(my_net2.num_nodes_at_level(0) == 8);
  REQUIRE(my_net2.num_nodes_at_level(1) == 8);
  REQUIRE(my_net2.num_nodes_at_level(2) == 4);
}

TEST_CASE("Building with vectors -- Unipartite", "[Network]")
{
  const std::vector<string> nodes_id{"a1", "a2", "a3"};
  const std::vector<string> nodes_type{"a", "a", "a"};
  const std::vector<string> types_name{"a"};

  // Fully connected network (Except a3, which is not connected to itself)
  const std::vector<string> edges_from{"a1", "a1", "a1", "a2", "a2"};
  const std::vector<string>   edges_to{"a1", "a2", "a3", "a2", "a3"};

  SBM_Network my_net { nodes_id, nodes_type,
                       edges_from, edges_to,
                       types_name };

  REQUIRE(my_net.num_nodes() == 3);
  REQUIRE(my_net.num_types() == 1);
  REQUIRE(my_net.num_nodes_of_type("a") == 3);
}

TEST_CASE("Building with vectors -- Bipartite", "[Network]")
{
  const std::vector<string> nodes_id{"a1", "a2", "b1", "b2"};
  const std::vector<string> nodes_type{"a", "a", "b", "b"};
  const std::vector<string> types_name{"a", "b"};

  // Fully connected bipartite network
  const std::vector<string> edges_from{"a1", "a1", "a2", "a2"};
  const std::vector<string>   edges_to{"b1", "b2", "b1", "b2"};

  SBM_Network my_net { nodes_id, nodes_type,
                       edges_from, edges_to,
                       types_name };

  REQUIRE(my_net.num_nodes() == 4);
  REQUIRE(my_net.num_types() == 2);
  REQUIRE(my_net.num_nodes_of_type("a") == 2);
  REQUIRE(my_net.num_nodes_of_type("b") == 2);
}

TEST_CASE("Building with vectors -- Tripartite", "[Network]")
{
  const std::vector<string> nodes_id{"a1", "a2", "b1", "c1", "c2"};
  const std::vector<string> nodes_type{"a", "a", "b", "c", "c"};
  const std::vector<string> types_name{"a", "b", "c"};

  // Only has connections from a-b and a-c
  const std::vector<string> edges_from{"a1", "a1", "a2", "a2"};
  const std::vector<string>   edges_to{"b1", "c1", "b1", "c2"};

  SBM_Network my_net { nodes_id, nodes_type,
                       edges_from, edges_to,
                       types_name };

  REQUIRE(my_net.num_nodes() == 5);
  REQUIRE(my_net.num_types() == 3);
  REQUIRE(my_net.num_nodes_of_type("a") == 2);
  REQUIRE(my_net.num_nodes_of_type("b") == 1);
  REQUIRE(my_net.num_nodes_of_type("c") == 2);
}

TEST_CASE("Building with vectors -- Restricted tripartite", "[Network]")
{
  const std::vector<string> nodes_id{"a1", "a2", "b1", "b2", "c1", "c2"};
  const std::vector<string> nodes_type{"a", "a", "b", "b", "c", "c"};
  const std::vector<string> types_name{"a", "b", "c"};

  const std::vector<string> edges_from{"a1", "a1", "a2", "b1", "b1", "b2"};
  const std::vector<string>   edges_to{"b1", "b2", "b1", "c1", "c2", "c1"};

  // Has connections from a-b and b-c
  const std::vector<string> type_from{"a", "b"};
  const std::vector<string>   type_to{"b", "c"};

  SBM_Network my_net { nodes_id, nodes_type,
                       edges_from, edges_to,
                       types_name,
                       42,
                       type_from, type_to };

  REQUIRE(my_net.num_nodes() == 6);
  REQUIRE(my_net.num_types() == 3);
  REQUIRE(my_net.num_nodes_of_type("a") == 2);
  REQUIRE(my_net.num_nodes_of_type("b") == 2);
  REQUIRE(my_net.num_nodes_of_type("c") == 2);

  // Try adding an edges with unallowed types
  const std::vector<string> bad_edges_from { "b1", "a1", "a1", "a2", "a2", "b1", "b1" };
  const std::vector<string> bad_edges_to { "b2", "b1", "c1", "b1", "c2", "c2", "c1" };
  
  // Will complain
  REQUIRE_THROWS(
      SBM_Network { nodes_id, nodes_type,
                    bad_edges_from, bad_edges_to,
                    types_name,
                    42,
                    type_from, type_to});
}

TEST_CASE("Counting edges", "[Network]")
{
  SBM_Network my_net{{ "a", "b" }, 42};

  // Base-level nodes
  Node* a1 = my_net.add_node("a1", "a");
  Node* a2 = my_net.add_node("a2", "a");
  Node* a3 = my_net.add_node("a3", "a");
  Node* a4 = my_net.add_node("a4", "a");
  Node* a5 = my_net.add_node("a5", "a");

  Node* b1 = my_net.add_node("b1", "b");
  Node* b2 = my_net.add_node("b2", "b");
  Node* b3 = my_net.add_node("b3", "b");
  Node* b4 = my_net.add_node("b4", "b");
  Node* b5 = my_net.add_node("b5", "b");

  // level one blocks
  my_net.build_level();
  Node* a11 = my_net.add_node("a11", "a", 1);
  Node* a12 = my_net.add_node("a12", "a", 1);
  Node* a13 = my_net.add_node("a13", "a", 1);
  Node* b11 = my_net.add_node("b11", "b", 1);
  Node* b12 = my_net.add_node("b12", "b", 1);
  Node* b13 = my_net.add_node("b13", "b", 1);

  // level two blocks
  my_net.build_level();
  Node* a21 = my_net.add_node("a21", "a", 2);
  Node* a22 = my_net.add_node("a22", "a", 2);
  Node* b21 = my_net.add_node("b21", "b", 2);
  Node* b22 = my_net.add_node("b22", "b", 2);

  // Add edges
  my_net.add_edge("a1", "b1");
  my_net.add_edge("a1", "b2");
  my_net.add_edge("a2", "b1");
  my_net.add_edge("a2", "b3");
  my_net.add_edge("a2", "b5");
  my_net.add_edge("a3", "b2");
  my_net.add_edge("a4", "b4");
  my_net.add_edge("a4", "b5");
  my_net.add_edge("a5", "b3");

  // Set hierarchy

  // Nodes -> level 1
  a1->set_parent(a11);

  a2->set_parent(a12);
  a3->set_parent(a12);

  a4->set_parent(a13);
  a5->set_parent(a13);

  b1->set_parent(b11);
  b2->set_parent(b11);

  b3->set_parent(b12);

  b4->set_parent(b13);
  b5->set_parent(b13);

  // level 1 -> level 2
  a11->set_parent(a21);
  a12->set_parent(a21);
  a13->set_parent(a22);

  b11->set_parent(b21);
  b12->set_parent(b21);
  b13->set_parent(b22);

  // Make sure our SBM is the proper size

  // Make sure node degrees are correct
  REQUIRE(a11->degree() == 2);
  REQUIRE(a12->degree() == 4);
  REQUIRE(a13->degree() == 3);
  REQUIRE(b11->degree() == 4);
  REQUIRE(b12->degree() == 2);
  REQUIRE(b13->degree() == 3);

  REQUIRE(a21->degree() == 6);
  REQUIRE(a22->degree() == 3);
  REQUIRE(b21->degree() == 6);
  REQUIRE(b22->degree() == 3);

  // Gather edges between all blocks at level one
  // Check num edges between blocks
  auto a11_edges = a11->gather_neighbors_at_level(1);
  REQUIRE(a11_edges[b11] == 2);
  REQUIRE(a11_edges[b12] == 0);
  REQUIRE(a11_edges[b13] == 0);

  auto a12_edges = a12->gather_neighbors_at_level(1);
  REQUIRE(a12_edges[b11] == 2);

  REQUIRE(a12_edges[b12] == 1);

  REQUIRE(a12_edges[b13] == 1);

  auto a13_edges = a13->gather_neighbors_at_level(1);
  REQUIRE(a13_edges[b11] == 0);
  REQUIRE(a13_edges[b12] == 1);
  REQUIRE(a13_edges[b13] == 2);

  // Direction shouldn't matter
  REQUIRE(a11->gather_neighbors_at_level(1)[b11] == b11->gather_neighbors_at_level(1)[a11]);

  // Direction shouldn't matter
  REQUIRE(a11->gather_neighbors_at_level(1)[b12] == b12->gather_neighbors_at_level(1)[a11]);

  // Repeat for level 2
  auto a21_edges = a21->gather_neighbors_at_level(2);
  REQUIRE(a21_edges[b21] == 5);
  REQUIRE(a21_edges[b22] == 1);

  auto a22_edges = a22->gather_neighbors_at_level(2);
  REQUIRE(a22_edges[b21] == 1);
  REQUIRE(a22_edges[b22] == 2);

  // Now we will change the block for a node and make sure the changes are properly reflected

  // Update the level 1 edge counts
  a3->set_parent(a13);

  // Make sure node degrees are correct
  REQUIRE(a11->degree() == 2);
  REQUIRE(a12->degree() == 3);
  REQUIRE(a13->degree() == 4);

  REQUIRE(b11->degree() == 4);
  REQUIRE(b12->degree() == 2);
  REQUIRE(b13->degree() == 3);


  // Check num edges between blocks
  auto a11_edges_new = a11->gather_neighbors_at_level(1);
  REQUIRE(a11_edges_new[b11] == 2);
  REQUIRE(a11_edges_new[b12] == 0);
  REQUIRE(a11_edges_new[b13] == 0);

  auto a12_edges_new = a12->gather_neighbors_at_level(1);
  REQUIRE(a12_edges_new[b11] == 1);
  REQUIRE(a12_edges_new[b12] == 1);
  REQUIRE(a12_edges_new[b13] == 1);

  auto a13_edges_new = a13->gather_neighbors_at_level(1);
  REQUIRE(a13_edges_new[b11] == 1);
  REQUIRE(a13_edges_new[b12] == 1);
  REQUIRE(a13_edges_new[b13] == 2);
}
