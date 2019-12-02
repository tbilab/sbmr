#include "catch.hpp"

#include <iomanip>

#include "../helpers.h"
#include "../Network.h"


TEST_CASE("Basic initialization of network", "[Network]")
{
    Network my_net;

    // Add some nodes to Network
    my_net.add_node("n1", 0);
    my_net.add_node("n2", 0);
    my_net.add_node("n3", 0);
    my_net.add_node("m1", 1);
    my_net.add_node("m2", 1);
    my_net.add_node("m3", 1);
    my_net.add_node("m4", 1);

    // Create a group node
    my_net.create_group_node(0, 1);
    my_net.create_group_node(1, 1);

    // How many nodes at the 'data' level do we have?
    REQUIRE(my_net.nodes.at(0)->size() == 7);

    REQUIRE(
        print_node_ids(*my_net.nodes.at(0)) ==
        "m1, m2, m3, m4, n1, n2, n3" 
    );

    // We should have two levels
    REQUIRE(my_net.nodes.size() == 2);

    // Group name convention <type>-<level>_<id>
    REQUIRE(
        print_node_ids(*my_net.nodes.at(1)) ==
        "0-1_0, 1-1_1"
    );

    // Filter to a given node type
    REQUIRE(
        "n1, n2, n3" == 
        print_node_ids(my_net.get_nodes_of_type_at_level(0,0))
    );
    REQUIRE(
        "m1, m2, m3, m4" == 
        print_node_ids(my_net.get_nodes_of_type_at_level(1,0))
    );
    REQUIRE(
        "1-1_1" == 
        print_node_ids(my_net.get_nodes_of_type_at_level(1,1))
    );
  
  // Get number of levels
  REQUIRE(my_net.nodes.size() == 2);
  
  // There should be two types of print_node_ids(*my_net.nodes.at(0))nodes
  REQUIRE(my_net.unique_node_types.size() == 2);
}

TEST_CASE("Tracking node types", "[Network]")
{
  Network my_net;
  
  // Add some nodes to Network
  my_net.add_node("n1", 0);
  my_net.add_node("n2", 0);
  
  // There should only be one type of node so far
  REQUIRE(1 ==  my_net.unique_node_types.size());
  
  my_net.add_node("m1", 1);
  
  // There should now be two types of nodes
  REQUIRE(2 == my_net.unique_node_types.size());
  
  my_net.add_node("m2", 1);
  my_net.add_node("n3", 0);
  
  // There should still just be two types of nodes
  REQUIRE(2 == my_net.unique_node_types.size());
  
  my_net.add_node("m3", 1);
  my_net.add_node("o1", 2);
  my_net.add_node("o2", 2);
  
  // There should now be three types of nodes
  REQUIRE(3 == my_net.unique_node_types.size());
}


TEST_CASE("Initializing a group for every node", "[Network]")
{
    Network my_net;

    my_net.add_node("a1", 1);
    my_net.add_node("a2", 1);
    my_net.add_node("a3", 1);
    my_net.add_node("a4", 1);
    my_net.add_node("a5", 1);
    my_net.add_node("a10", 1);
    my_net.add_node("a11", 1);
    my_net.add_node("a13", 1);
    my_net.add_node("a14", 1);
    my_net.add_node("a6", 1);
    my_net.add_node("a7", 1);
    my_net.add_node("a8", 1);
    my_net.add_node("a9", 1);
    my_net.add_node("a12", 1);
    my_net.add_node("b1", 0);
    my_net.add_node("b2", 0);
    my_net.add_node("b3", 0);
    my_net.add_node("b4", 0);

    // There should be a total of 18 nodes at base level
    REQUIRE(18 == my_net.get_level(0)->size());

    // And zero nodes at the group level
    REQUIRE(0 == my_net.get_level(1)->size());

    // Now assignin every node their own parent group
    my_net.give_every_node_at_level_own_group(0);

    // There should now be a total of 18 nodes at level 1
    REQUIRE(18 == my_net.get_level(1)->size());
  
    // And a node from this new level should have a single child
    REQUIRE(1 == my_net.get_node_from_level(1)->children.size());
}


TEST_CASE("Cleaning up empty groups", "[Network]")
{
    Network my_net;

    // Start with a few nodes in the network
    NodePtr n1 = my_net.add_node("n1", 0);
    NodePtr n2 = my_net.add_node("n2", 0);
    NodePtr n3 = my_net.add_node("n3", 0);
    NodePtr n4 = my_net.add_node("n4", 0);

    // Create a few group nodes at first level
    NodePtr g1_1 = my_net.create_group_node(0, 1);
    NodePtr g1_2 = my_net.create_group_node(0, 1);
    NodePtr g1_3 = my_net.create_group_node(0, 1);
    NodePtr g1_4 = my_net.create_group_node(0, 1);

    // Create two groups for second level
    NodePtr g2_1 = my_net.create_group_node(0, 2);  
    NodePtr g2_2 = my_net.create_group_node(0, 2);

    // Add children to groups 1 and two at first level
    n1->set_parent(g1_1);
    n2->set_parent(g1_1);
    n3->set_parent(g1_2);
    n4->set_parent(g1_2);

    // Add children to both level two groups
    g1_1->set_parent(g2_1);
    g1_2->set_parent(g2_1);
    g1_3->set_parent(g2_1);
    g1_4->set_parent(g2_2);


    // Make sure our network is the proper size
    REQUIRE(3 == my_net.nodes.size());
    REQUIRE(4 == my_net.nodes.at(0)->size());
    REQUIRE(4 == my_net.nodes.at(1)->size());
    REQUIRE(2 == my_net.nodes.at(2)->size());


    // Run group cleanup
    int num_culled = my_net.clean_empty_groups();

    // Three groups should have been cleaned
    REQUIRE(3 == num_culled);

    // Two should have been taken from the first group level
    REQUIRE(2 == my_net.nodes.at(1)->size());

    // And 1 should have been taken from the second group level
    REQUIRE(1 == my_net.nodes.at(2)->size());

    // Run group cleanup again
    int num_culled_clean = my_net.clean_empty_groups();

    // No groups should have been culled
    REQUIRE(0 == num_culled_clean);
}


TEST_CASE("Building an edge count map", "[Network]")
{
    Network my_net;

    // Base-level nodes
    NodePtr a1 = my_net.add_node("a1", 0);
    NodePtr a2 = my_net.add_node("a2", 0);
    NodePtr a3 = my_net.add_node("a3", 0);
    NodePtr a4 = my_net.add_node("a4", 0);
    NodePtr b1 = my_net.add_node("b1", 1);
    NodePtr b2 = my_net.add_node("b2", 1);
    NodePtr b3 = my_net.add_node("b3", 1);
    NodePtr b4 = my_net.add_node("b4", 1);

    // level one groups
    NodePtr a11 = my_net.add_node("a11", 0, 1);
    NodePtr a12 = my_net.add_node("a12", 0, 1);
    NodePtr a13 = my_net.add_node("a13", 0, 1);
    NodePtr b11 = my_net.add_node("b11", 1, 1);
    NodePtr b12 = my_net.add_node("b12", 1, 1);
    NodePtr b13 = my_net.add_node("b13", 1, 1);

    // level two groups
    NodePtr a21 = my_net.add_node("a21", 0, 2);
    NodePtr a22 = my_net.add_node("a22", 0, 2);
    NodePtr b21 = my_net.add_node("b21", 1, 2);

    // Add connections
    my_net.add_connection(a1, b1);
    my_net.add_connection(a1, b2);

    my_net.add_connection(a2, b1);
    my_net.add_connection(a2, b2);
    my_net.add_connection(a2, b3);
    my_net.add_connection(a2, b4);

    my_net.add_connection(a3, b1);
    my_net.add_connection(a3, b2);
    my_net.add_connection(a3, b3);

    my_net.add_connection(a4, b1);
    my_net.add_connection(a4, b3);

    // Set hierarchy

    // Nodes -> level 1
    a1->set_parent(a11);
    a2->set_parent(a12);
    a3->set_parent(a12);
    a4->set_parent(a13);
    b1->set_parent(b11);
    b2->set_parent(b11);
    b3->set_parent(b12);
    b4->set_parent(b13);

    // level 1 -> level 2
    a11->set_parent(a21);
    a12->set_parent(a22);
    a13->set_parent(a22);
    b11->set_parent(b21);
    b12->set_parent(b21);
    b13->set_parent(b21);

    // Make sure our network is the proper size
    REQUIRE(3 == my_net.nodes.size());
    REQUIRE(8 == my_net.nodes.at(0)->size());
    REQUIRE(6 == my_net.nodes.at(1)->size());
    REQUIRE(3 == my_net.nodes.at(2)->size());

    // Build the network connection map for first level
    auto l1_edges = my_net.get_edge_counts(1);

    // The edge count map should have 6 non-empty entries 
    REQUIRE(6 == l1_edges->size());

    // Check num edges between groups
    REQUIRE((*l1_edges)[find_edges(a11, b11)] == 2);
    REQUIRE((*l1_edges)[find_edges(a11, b12)] == 0);
    REQUIRE((*l1_edges)[find_edges(a11, b13)] == 0);
    REQUIRE((*l1_edges)[find_edges(a12, b11)] == 4);
    REQUIRE((*l1_edges)[find_edges(a12, b12)] == 2);
    REQUIRE((*l1_edges)[find_edges(a12, b13)] == 1);
    REQUIRE((*l1_edges)[find_edges(a13, b11)] == 1);
    REQUIRE((*l1_edges)[find_edges(a13, b12)] == 1);
    REQUIRE((*l1_edges)[find_edges(a13, b13)] == 0);

    // Direction shouldn't matter
    REQUIRE(
        (*l1_edges)[find_edges(a11, b11)] == 
        (*l1_edges)[find_edges(b11, a11)]);

    REQUIRE(
        (*l1_edges)[find_edges(a11, b12)] ==
        (*l1_edges)[find_edges(b12, a11)]);

    REQUIRE(
        (*l1_edges)[find_edges(a11, b13)] ==
        (*l1_edges)[find_edges(b13, a11)]);

    REQUIRE(
        (*l1_edges)[find_edges(a12, b11)] ==
        (*l1_edges)[find_edges(b11, a12)]);

    REQUIRE(
        (*l1_edges)[find_edges(a12, b12)] ==
        (*l1_edges)[find_edges(b12, a12)]);

    REQUIRE(
        (*l1_edges)[find_edges(a12, b13)] ==
        (*l1_edges)[find_edges(b13, a12)]);

    REQUIRE(
        (*l1_edges)[find_edges(a13, b11)] ==
        (*l1_edges)[find_edges(b11, a13)]);

    REQUIRE(
        (*l1_edges)[find_edges(a13, b12)] ==
        (*l1_edges)[find_edges(b12, a13)]);

    REQUIRE(
        (*l1_edges)[find_edges(a13, b13)] ==
        (*l1_edges)[find_edges(b13, a13)]);

    // Repeat for level 2
    auto l2_edges = my_net.get_edge_counts(2);

    // Check num edges between groups
    REQUIRE((*l2_edges)[find_edges(a22, b21)] == 9);


    // Now we will change the group for a node and make sure the changes are
    // updated properly with the update_edge_counts() function

    // Change a3's parent from a12 to a13.

    // Update the level 1 edge counts
    my_net.update_edge_counts(a3, a13);
    a3->set_parent(a13);

    // Make sure that the needed change were made to a12's connections:
    REQUIRE((*l1_edges)[find_edges(a12, b11)] == 2);
    REQUIRE((*l1_edges)[find_edges(a12, b12)] == 1);
    REQUIRE((*l1_edges)[find_edges(a12, b13)] == 1);

    // And to a13's as well...
    REQUIRE((*l1_edges)[find_edges(a13, b11)] == 3);
    REQUIRE((*l1_edges)[find_edges(a13, b12)] == 2);
    REQUIRE((*l1_edges)[find_edges(a13, b13)] == 0);


    // Nothing should have changed for the level 2 connections
    // Check num edges between groups
    REQUIRE((*l2_edges)[find_edges(a21, b21)] == 2);
    REQUIRE((*l2_edges)[find_edges(a22, b21)] == 9);
}


TEST_CASE("State dumping and restoring", "[Network")
{
  Network my_net;

  // Start with a few nodes in the network
  NodePtr a1 = my_net.add_node("a1", 0);
  NodePtr a2 = my_net.add_node("a2", 0);
  NodePtr a3 = my_net.add_node("a3", 0);
  
  NodePtr b1 = my_net.add_node("b1", 1);
  NodePtr b2 = my_net.add_node("b2", 1);
  NodePtr b3 = my_net.add_node("b3", 1);

  NodePtr a11 = my_net.add_node("a11", 0, 1);
  NodePtr a12 = my_net.add_node("a12", 0, 1);
  NodePtr a13 = my_net.add_node("a13", 0, 1);
  
  NodePtr b11 = my_net.add_node("b11", 1, 1);
  NodePtr b12 = my_net.add_node("b12", 1, 1);
  NodePtr b13 = my_net.add_node("b13", 1, 1);

  // Assign simple group structure
  a1->set_parent(a11);
  a2->set_parent(a12);
  a3->set_parent(a13);

  b1->set_parent(b11);
  b2->set_parent(b12);
  b3->set_parent(b13);

  // Dump model state
  State_Dump state1 = my_net.get_state();

  // Test state dump is in correct form
  REQUIRE(
    print_ids_to_string(state1.id) == "a1, a11, a12, a13, a2, a3, b1, b11, b12, b13, b2, b3"
  );

  REQUIRE(
    print_ids_to_string(state1.parent) == "a11, a12, a13, b11, b12, b13, none, none, none, none, none, none"
  );

  // REQUIRE(state1.type[0] == 0);
  // REQUIRE(state1.type[1] == 0);
  // REQUIRE(state1.type[2] == 0);
  // REQUIRE(state1.type[3] == 1);
  // REQUIRE(state1.type[4] == 1);
  // REQUIRE(state1.type[5] == 1);

  // Now give node a1 a different parent
  a1->set_parent(a12);

  // Dump model state again
  State_Dump state2 = my_net.get_state();

  // Make sure new parent for a1 is reflected in new state dump
  REQUIRE(
    print_ids_to_string(state2.id) == "a1, a11, a12, a13, a2, a3, b1, b11, b12, b13, b2, b3"
  );

  REQUIRE(
    print_ids_to_string(state2.parent) == "a12, a12, a13, b11, b12, b13, none, none, none, none, none, none"
  );

  // Now restore model to pre a1->a12 move state
  my_net.load_from_state(state1);

  State_Dump state3 = my_net.get_state();

  // Make sure state dumps 1 and 3 match state dump is in correct form
  REQUIRE(
    print_ids_to_string(state1.id) == print_ids_to_string(state3.id)
  );

  REQUIRE(
    print_ids_to_string(state1.parent) == print_ids_to_string(state3.parent)
  );

}
