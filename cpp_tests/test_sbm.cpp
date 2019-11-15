#include <gtest/gtest.h>
#include <iostream>
#include "../helpers.h"
#include "../SBM.h"

// Loads a simple bipartite sbm model with optional hierarchy added
SBM build_simple_SBM(){
  
  // This function builds a network with the following structure
  /*
   +----+               +----+
   +----+   |    |---------------|    |
   |    |   | a1 |-----      ----| b1 |   +----+
   |a11 |---|    |      \  /    -|    |---|    |
   |    |   +----+       \/   /  +----+   |b11 |
   +----+   +----+       /\  /   +----+   |    |
   |    |----- /  \ ----|    |---|    |
   +----+   | a2 |---------------| b2 |   +----+      
   |    |---|    |       /    ---|    |   
   |a12 |   +----+      /   /    +----+   +----+
   |    |   +----+     /   /     +----+   |    |
   |    |---|    |----    /      |    |   |b12 |
   +----+   | a3 |-------        | b3 |---|    |
   |    |------    -----|    |   +----+      
   +----+   +----+      \  /     +----+   +----+
   |    |   +----+       \/      +----+   |    |
   |a13 |---|    |       /\      |    |   |b13 |
   |    |   | a4 |------/  \-----| b4 |---|    |
   +----+   |    |               |    |   +----+
   +----+               +----+
   */   
  SBM my_SBM;
  
  // Add nodes to graph first
  NodePtr a1 = my_SBM.add_node("a1", 0);
  NodePtr a2 = my_SBM.add_node("a2", 0);
  NodePtr a3 = my_SBM.add_node("a3", 0);
  NodePtr a4 = my_SBM.add_node("a4", 0);
  NodePtr b1 = my_SBM.add_node("b1", 1);
  NodePtr b2 = my_SBM.add_node("b2", 1);
  NodePtr b3 = my_SBM.add_node("b3", 1);
  NodePtr b4 = my_SBM.add_node("b4", 1);
  
  // Add connections
  my_SBM.add_connection(a1, b1);
  my_SBM.add_connection(a1, b2);
  my_SBM.add_connection(a2, b1);
  my_SBM.add_connection(a2, b2);
  my_SBM.add_connection(a3, b1);
  my_SBM.add_connection(a3, b2);
  my_SBM.add_connection(a3, b4);
  my_SBM.add_connection(a4, b3);
  
  // Make 2 type 0/a groups
  NodePtr a11 = my_SBM.add_node("a11", 0, 1);
  NodePtr a12 = my_SBM.add_node("a12", 0, 1);
  NodePtr a13 = my_SBM.add_node("a13", 0, 1);
  NodePtr b11 = my_SBM.add_node("b11", 1, 1);
  NodePtr b12 = my_SBM.add_node("b12", 1, 1);
  NodePtr b13 = my_SBM.add_node("b13", 1, 1);
  
  // Assign nodes to their groups
  a1->set_parent(a11);
  a2->set_parent(a12);
  a3->set_parent(a12);
  a4->set_parent(a13);
  b1->set_parent(b11);
  b2->set_parent(b11);
  b3->set_parent(b12);
  b4->set_parent(b13);
  
  
  return my_SBM;
}

// Gets average of the last n elements for a paseed vector of integers
inline float avg_last_n(vector<int> vec, int n){
  return std::accumulate(vec.end() - n, 
                         vec.end(), 
                         0.0 ) / float(n);
}


TEST(testSBM, basic){
  SBM my_SBM;
  
  // Add some nodes to SBM
  my_SBM.add_node("n1", 0);
  my_SBM.add_node("n2", 0);
  my_SBM.add_node("n3", 0);
  my_SBM.add_node("m1", 1);
  my_SBM.add_node("m2", 1);
  my_SBM.add_node("m3", 1);
  my_SBM.add_node("m4", 1);
  
  // Create a group node
  my_SBM.create_group_node(0, 1);
  my_SBM.create_group_node(1, 1);
  
  // How many nodes at the 'data' level do we have?
  EXPECT_EQ(7, my_SBM.nodes.at(0)->size());
  
  EXPECT_EQ("m1, m2, m3, m4, n1, n2, n3", print_node_ids(*my_SBM.nodes.at(0)));
  
  // We should have two levels
  EXPECT_EQ(2, my_SBM.nodes.size());
  
  // Group name convention <type>-<level>_<id>
  EXPECT_EQ("0-1_0, 1-1_1", print_node_ids(*my_SBM.nodes.at(1)));

  // Filter to a given node type
  EXPECT_EQ("n1, n2, n3", print_node_ids(my_SBM.get_nodes_of_type_at_level(0,0)));
  EXPECT_EQ("m1, m2, m3, m4", print_node_ids(my_SBM.get_nodes_of_type_at_level(1,0)));
  EXPECT_EQ("1-1_1", print_node_ids(my_SBM.get_nodes_of_type_at_level(1,1)));
  
  // Get number of levels
  EXPECT_EQ(2, my_SBM.nodes.size());
  
  // There should be two types of print_node_ids(*my_SBM.nodes.at(0))nodes
  EXPECT_EQ(2, my_SBM.unique_node_types.size());
  
}


TEST(testSBM, tracking_node_types){
  SBM my_SBM;
  
  // Add some nodes to SBM
  my_SBM.add_node("n1", 0);
  my_SBM.add_node("n2", 0);
  
  // There should only be one type of node so far
  EXPECT_EQ(1, my_SBM.unique_node_types.size());
  
  my_SBM.add_node("m1", 1);
  
  // There should now be two types of nodes
  EXPECT_EQ(2, my_SBM.unique_node_types.size());
  
  my_SBM.add_node("m2", 1);
  my_SBM.add_node("n3", 0);
  
  // There should still just be two types of nodes
  EXPECT_EQ(2, my_SBM.unique_node_types.size());
  
  my_SBM.add_node("m3", 1);
  my_SBM.add_node("o1", 2);
  my_SBM.add_node("o2", 2);
  
  // There should now be three types of nodes
  EXPECT_EQ(3, my_SBM.unique_node_types.size());
}


TEST(testSBM, state_dumping){
  SBM my_SBM;
  
  // Add nodes to graph first
  NodePtr a1 = my_SBM.add_node("a1", 0);
  NodePtr a2 = my_SBM.add_node("a2", 0);
  NodePtr a3 = my_SBM.add_node("a3", 0);
  
  NodePtr b1 = my_SBM.add_node("b1", 1);
  NodePtr b2 = my_SBM.add_node("b2", 1);
  NodePtr b3 = my_SBM.add_node("b3", 1);
  
  // Make some first level parents
  NodePtr a11 = my_SBM.add_node("a11", 0, 1);
  NodePtr a12 = my_SBM.add_node("a12", 0, 1);
  
  NodePtr b11 = my_SBM.add_node("b11", 1, 1);
  NodePtr b12 = my_SBM.add_node("b12", 1, 1);
  
  // Assign nodes to their groups
  a1->set_parent(a11);
  a2->set_parent(a11);
  a3->set_parent(a12);
  
  b1->set_parent(b11);
  b2->set_parent(b12);
  b3->set_parent(b12);
   
  // Dump state
  State_Dump first_state = my_SBM.get_sbm_state();
  
  // Make sure that there is one entry for each node in the data
  EXPECT_EQ(
    print_ids_to_string(first_state.id),
    "a1, a11, a12, a2, a3, b1, b11, b12, b2, b3"
  );
  
  // Grab location of node a1 in state
  int index_of_a1 = std::distance(
    first_state.id.begin(), 
    std::find(first_state.id.begin(), first_state.id.end(), "a1")
  );
  
  // Make sure a1 has parent node of a11
  EXPECT_EQ(
    first_state.parent[index_of_a1],
    "a11"
  );
  
  // Highest level should be 1
  EXPECT_EQ(
    *max_element(first_state.level.begin(), first_state.level.end()),
    1
  );
  
  // Now update model by swapping in a12 as parent of a1
  a1->set_parent(a12);
  
  // See if state dump adjust accordingly
  State_Dump second_state = my_SBM.get_sbm_state();
  
  // Grab location of node a1 in state and Make sure a1 has parent node of a11
  EXPECT_EQ(
    second_state.parent[std::distance(
                        second_state.id.begin(), 
                        std::find(second_state.id.begin(), second_state.id.end(), "a1"))],
    "a12"
  );
  
  // Make sure a11 has parent of "none"
  EXPECT_EQ(
    second_state.parent[std::distance(
                        second_state.id.begin(), 
                        std::find(second_state.id.begin(), second_state.id.end(), "a11"))],
    "none"
  );
  
  // Add a new level to SBM by making a second level of a node
  NodePtr a21 = my_SBM.add_node("a21", 1, 2);
  a11->set_parent(a21);
  a12->set_parent(a21);
  
  // See if state dump adds node accordingly
  State_Dump third_state = my_SBM.get_sbm_state();

  EXPECT_EQ(
    print_ids_to_string(third_state.id),
    "a1, a11, a12, a2, a21, a3, b1, b11, b12, b2, b3"
  );

  // Highest level should be 2 now
  EXPECT_EQ(
    *max_element(third_state.level.begin(), third_state.level.end()),
    2
  );
  
  // Make sure that a11 now has a parent node
  // Make sure a11 has parent of "a21"
  EXPECT_EQ(
    third_state.parent[std::distance(
                          third_state.id.begin(), 
                          std::find(third_state.id.begin(), third_state.id.end(), "a11"))],
   "a21"
  );
  
}


TEST(testSBM, build_with_connections){
  SBM my_SBM;

    // Start with a single node in the network
  my_SBM.add_node("a1", 1);
  EXPECT_EQ(1, my_SBM.nodes[0]->size());
  
  // Add more nodes
  my_SBM.add_node("a2", 1);
  my_SBM.add_node("a3", 1);
  my_SBM.add_node("a4", 1);
  my_SBM.add_node("a5", 1);
  my_SBM.add_node("a10", 1);
  my_SBM.add_node("a11", 1);
  my_SBM.add_node("a13", 1);
  my_SBM.add_node("a14", 1);
  my_SBM.add_node("a6", 1);
  my_SBM.add_node("a7", 1);
  my_SBM.add_node("a8", 1);
  my_SBM.add_node("a9", 1);
  my_SBM.add_node("a12", 1);
  my_SBM.add_node("b1", 0);
  my_SBM.add_node("b2", 0);
  my_SBM.add_node("b3", 0);
  my_SBM.add_node("b4", 0);

  // Add connections
  my_SBM.add_connection("a1", "b1");
  my_SBM.add_connection("a2", "b1");
  my_SBM.add_connection("a3", "b1");
  my_SBM.add_connection("a4", "b1");
  my_SBM.add_connection("a5", "b1");
  my_SBM.add_connection("a1", "b2");
  my_SBM.add_connection("a2", "b2");
  my_SBM.add_connection("a3", "b2");
  my_SBM.add_connection("a4", "b2");
  my_SBM.add_connection("a5", "b2");
  my_SBM.add_connection("a10", "b2");
  my_SBM.add_connection("a11", "b2");
  my_SBM.add_connection("a13", "b2");
  my_SBM.add_connection("a14", "b2");
  my_SBM.add_connection("a6", "b3");
  my_SBM.add_connection("a7", "b3");
  my_SBM.add_connection("a8", "b3");
  my_SBM.add_connection("a9", "b3");
  my_SBM.add_connection("a10", "b3");
  my_SBM.add_connection("a12", "b3");
  my_SBM.add_connection("a13", "b3");
  my_SBM.add_connection("a14", "b3");
  my_SBM.add_connection("a10", "b4");
  my_SBM.add_connection("a11", "b4");
  my_SBM.add_connection("a12", "b4");
  my_SBM.add_connection("a13", "b4");
  my_SBM.add_connection("a14", "b4");

  // There should be a total of 18 nodes
  EXPECT_EQ(18, my_SBM.nodes.at(0)->size());

  // Now start initialization of the MCMC chain by assigning every node their
  // own parent group
  my_SBM.give_every_node_a_group_at_level(0);

  // There should be a total of 18 nodes at level 1
  EXPECT_EQ(18, my_SBM.nodes.at(1)->size());
  
  // A node from this new level should have a single child
  EXPECT_EQ(1, my_SBM.get_node_from_level(1)->children.size());

}


TEST(testSBM, calculating_transition_probs){
  
  SBM my_SBM;
  
  // Add nodes to graph first
  NodePtr a1 = my_SBM.add_node("a1", 0);
  NodePtr a2 = my_SBM.add_node("a2", 0);
  NodePtr a3 = my_SBM.add_node("a3", 0);
  NodePtr a4 = my_SBM.add_node("a4", 0);
  NodePtr b1 = my_SBM.add_node("b1", 1);
  NodePtr b2 = my_SBM.add_node("b2", 1);
  NodePtr b3 = my_SBM.add_node("b3", 1);
  NodePtr b4 = my_SBM.add_node("b4", 1);

  // There should be a total of 8 nodes
  EXPECT_EQ(8, my_SBM.nodes.at(0)->size());

  // Add connections
  my_SBM.add_connection(a1, b1);
  my_SBM.add_connection(a1, b2);
  my_SBM.add_connection(a2, b1);
  my_SBM.add_connection(a2, b2);
  my_SBM.add_connection(a3, b1);
  my_SBM.add_connection(a3, b2);
  my_SBM.add_connection(a3, b4);
  my_SBM.add_connection(a4, b3);

  // Create groups

  // Make 2 type 0/a groups
  NodePtr a1_1 = my_SBM.create_group_node(0, 1);
  NodePtr a1_2 = my_SBM.create_group_node(0, 1);
  NodePtr a1_3 = my_SBM.create_group_node(0, 1);

  // Make 3 type 1/b groups
  NodePtr b1_1 = my_SBM.create_group_node(1, 1);
  NodePtr b1_2 = my_SBM.create_group_node(1, 1);
  NodePtr b1_3 = my_SBM.create_group_node(1, 1);


  // There should be a total of 6 level one groups
  EXPECT_EQ(6, my_SBM.nodes.at(1)->size());

  // Assign nodes to their groups
  a1->set_parent(a1_1);
  a2->set_parent(a1_2);
  a3->set_parent(a1_2);
  a4->set_parent(a1_3);

  b1->set_parent(b1_1);
  b2->set_parent(b1_1);
  b3->set_parent(b1_2);
  b4->set_parent(b1_3);

  // The group we hope a1 wants to join should have two members
  EXPECT_EQ("a2, a3", print_node_ids(a1_2->children));
 
  // Calculate move probabilities for node a1
  Trans_Probs a1_move_probs = my_SBM.get_transition_probs_for_groups(a1);
  EXPECT_EQ("0-1_0, 0-1_1, 0-1_2", print_node_ids(a1_move_probs.group));
  EXPECT_EQ("0-1_0, 0-1_1, 0-1_2", print_node_ids(a1_move_probs.group));

  double two = 2;
  double six = 6;
  double four = 4;
  double eps = 0.01;
  double tolerance = 0.005;

  // Prob of a1 staying in a1_1 should be approximately (2 + eps)/(6 + 4*eps)
  ASSERT_NEAR(
    (two + eps)/(six + four*eps),
    a1_move_probs.probability[0],
    tolerance
  );

  // Prob of a1 joining a1_2 should be approximately (4 + eps)/(6 + 4*eps)
  ASSERT_NEAR(
    (four + eps)/(six + four*eps),
    a1_move_probs.probability[1],
    tolerance
  );

  // Prob of a1 joining a1_3 should be approximately (0 + eps)/(6 + 4*eps)
  ASSERT_NEAR(
    (eps)/(six + four*eps),
    a1_move_probs.probability[2],
    tolerance
  );

  // Probabilities for transition should sum to 1
  ASSERT_NEAR(
    a1_move_probs.probability[0] + a1_move_probs.probability[1] + a1_move_probs.probability[2],
    1,
    tolerance
  );

  // Roll a 'Random' dice and choose which group to move the node to
  a1->set_parent(a1_2);

  // Make sure this transition was respected
  EXPECT_EQ("a1, a2, a3", print_node_ids(a1_2->children));
}


TEST(testSBM, cleaning_empty_groups){
  SBM my_SBM;
  
  // Start with a few nodes in the network
  NodePtr n1 = my_SBM.add_node("n1", 0);
  NodePtr n2 = my_SBM.add_node("n2", 0);
  NodePtr n3 = my_SBM.add_node("n3", 0);
  NodePtr n4 = my_SBM.add_node("n4", 0);

  // Create a few group nodes at first level
  NodePtr g1_1 = my_SBM.create_group_node(0, 1);
  NodePtr g1_2 = my_SBM.create_group_node(0, 1);
  NodePtr g1_3 = my_SBM.create_group_node(0, 1);
  NodePtr g1_4 = my_SBM.create_group_node(0, 1);
  
  // Create two groups for second level
  NodePtr g2_1 = my_SBM.create_group_node(0, 2);  
  NodePtr g2_2 = my_SBM.create_group_node(0, 2);
  
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
  EXPECT_EQ(3, my_SBM.nodes.size());
  EXPECT_EQ(4, my_SBM.nodes.at(0)->size());
  EXPECT_EQ(4, my_SBM.nodes.at(1)->size());
  EXPECT_EQ(2, my_SBM.nodes.at(2)->size());
  
  
  // Run group cleanup
  int num_culled = my_SBM.clean_empty_groups();
  
  // Three groups should have been cleaned
  EXPECT_EQ(3, num_culled);
  
  // Two should have been taken from the first group level
  EXPECT_EQ(2, my_SBM.nodes.at(1)->size());
  
  // And 1 should have been taken from the second group level
  EXPECT_EQ(1, my_SBM.nodes.at(2)->size());
  
  // Run group cleanup again
  int num_culled_clean = my_SBM.clean_empty_groups();
  
  // No groups should have been culled
  EXPECT_EQ(0, num_culled_clean);
}


TEST(testSBM, edge_count_map){
  SBM my_SBM;
  
  // Base-level nodes
  NodePtr a1 = my_SBM.add_node("a1", 0);
  NodePtr a2 = my_SBM.add_node("a2", 0);
  NodePtr a3 = my_SBM.add_node("a3", 0);
  NodePtr a4 = my_SBM.add_node("a4", 0);
  NodePtr b1 = my_SBM.add_node("b1", 1);
  NodePtr b2 = my_SBM.add_node("b2", 1);
  NodePtr b3 = my_SBM.add_node("b3", 1);
  NodePtr b4 = my_SBM.add_node("b4", 1);

  // level one groups
  NodePtr a11 = my_SBM.add_node("a11", 0, 1);
  NodePtr a12 = my_SBM.add_node("a12", 0, 1);
  NodePtr a13 = my_SBM.add_node("a13", 0, 1);
  NodePtr b11 = my_SBM.add_node("b11", 1, 1);
  NodePtr b12 = my_SBM.add_node("b12", 1, 1);
  NodePtr b13 = my_SBM.add_node("b13", 1, 1);

  // level two groups
  NodePtr a21 = my_SBM.add_node("a21", 0, 2);
  NodePtr a22 = my_SBM.add_node("a22", 0, 2);
  NodePtr b21 = my_SBM.add_node("b21", 1, 2);

  // Add connections
  my_SBM.add_connection(a1, b1);
  my_SBM.add_connection(a1, b2);

  my_SBM.add_connection(a2, b1);
  my_SBM.add_connection(a2, b2);
  my_SBM.add_connection(a2, b3);
  my_SBM.add_connection(a2, b4);

  my_SBM.add_connection(a3, b1);
  my_SBM.add_connection(a3, b2);
  my_SBM.add_connection(a3, b3);

  my_SBM.add_connection(a4, b1);
  my_SBM.add_connection(a4, b3);

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
  EXPECT_EQ(3, my_SBM.nodes.size());
  EXPECT_EQ(8, my_SBM.nodes.at(0)->size());
  EXPECT_EQ(6, my_SBM.nodes.at(1)->size());
  EXPECT_EQ(3, my_SBM.nodes.at(2)->size());
  
  // Build the network connection map for first level
  EdgeCounts l1_edges = my_SBM.gather_edge_counts(1);
  
  // The edge count map should have 6 non-empty entries 
  EXPECT_EQ(6, l1_edges.size());
  
  // Check num edges between groups
  EXPECT_EQ(l1_edges[find_edges(a11, b11)], 2 );
  EXPECT_EQ(l1_edges[find_edges(a11, b12)], 0 );
  EXPECT_EQ(l1_edges[find_edges(a11, b13)], 0 );
  
  EXPECT_EQ(l1_edges[find_edges(a12, b11)], 4 );
  EXPECT_EQ(l1_edges[find_edges(a12, b12)], 2 );
  EXPECT_EQ(l1_edges[find_edges(a12, b13)], 1 );
  
  EXPECT_EQ(l1_edges[find_edges(a13, b11)], 1 );
  EXPECT_EQ(l1_edges[find_edges(a13, b12)], 1 );
  EXPECT_EQ(l1_edges[find_edges(a13, b13)], 0 );
 
  // Direction shouldn't matter
  EXPECT_EQ(l1_edges[find_edges(a11, b11)],
            l1_edges[find_edges(b11, a11)]);

  EXPECT_EQ(l1_edges[find_edges(a11, b12)],
            l1_edges[find_edges(b12, a11)]);

  EXPECT_EQ(l1_edges[find_edges(a11, b13)],
            l1_edges[find_edges(b13, a11)]);

  EXPECT_EQ(l1_edges[find_edges(a12, b11)],
            l1_edges[find_edges(b11, a12)]);

  EXPECT_EQ(l1_edges[find_edges(a12, b12)],
            l1_edges[find_edges(b12, a12)]);

  EXPECT_EQ(l1_edges[find_edges(a12, b13)],
            l1_edges[find_edges(b13, a12)]);

  EXPECT_EQ(l1_edges[find_edges(a13, b11)],
            l1_edges[find_edges(b11, a13)]);

  EXPECT_EQ(l1_edges[find_edges(a13, b12)],
            l1_edges[find_edges(b12, a13)]);

  EXPECT_EQ(l1_edges[find_edges(a13, b13)],
            l1_edges[find_edges(b13, a13)]);
  
  // Repeat for level 2
  EdgeCounts l2_edges = my_SBM.gather_edge_counts(2);
  
  // Check num edges between groups
  EXPECT_EQ(l2_edges[find_edges(a22, b21)], 9);

  
  // Now we will change the group for a node and make sure the changes are
  // updated properly with the update_edge_counts() function
  
  // Change a3's parent from a12 to a13.
  a3->set_parent(a13);
  
  // Update the level 1 edge counts
  SBM::update_edge_counts(l1_edges, 1, a3, a12, a13);
  
  // Make sure that the needed change were made to a12's connections:
  EXPECT_EQ(l1_edges[find_edges(a12, b11)], 2);
  EXPECT_EQ(l1_edges[find_edges(a12, b12)], 1);
  EXPECT_EQ(l1_edges[find_edges(a12, b13)], 1);

  // And to a13's as well...
  EXPECT_EQ(l1_edges[find_edges(a13, b11)], 3);
  EXPECT_EQ(l1_edges[find_edges(a13, b12)], 2);
  EXPECT_EQ(l1_edges[find_edges(a13, b13)], 0);

  
  // Nothing should have changed for the level 2 connections
  
  // Update the level 2 edge counts
  SBM::update_edge_counts(l2_edges, 2, a3, a12, a13);
  
  // Check num edges between groups
  EXPECT_EQ(l2_edges[find_edges(a21, b21)], 2);
  EXPECT_EQ(l2_edges[find_edges(a22, b21)], 9);
}


TEST(testSBM, node_move_proposals){
  double tol = 0.01;
  SBM my_SBM = build_simple_SBM();

  NodePtr a1 = my_SBM.get_node_by_id("a1");

  // Initialize a sampler to choose group
  Sampler my_sampler;

  int num_trials = 1000;
  int num_times_no_move = 0;
  NodePtr old_group = a1->parent;

  // Run multiple trials and of move and see how often a given node is moved
  for (int i = 0; i < num_trials; ++i)
  {
    // Do move attempt (dry run)
    NodePtr new_group = my_SBM.propose_move_for_node(a1, my_sampler);

    if (new_group->id == old_group->id) num_times_no_move++;
  }
  
  double frac_of_time_no_change = double(num_times_no_move)/double(num_trials);

  // Prob of a1 staying in a1_1 should be approximately (2 + eps)/(6 + 4*eps)
  // Make sure model's decisions to move a1 reflects this.
  double two = 2;
  double six = 6;
  double four = 4;
  double eps = 0.01;
  
  ASSERT_NEAR(
    (two + eps)/(six + four*eps),
    frac_of_time_no_change,
    tol
  );
}


TEST(testSBM, node_move_attempts){
  double tol = 0.01;
  SBM my_SBM = build_simple_SBM();

  NodePtr a1 = my_SBM.get_node_by_id("a1");
  
  EdgeCounts l1_edges = my_SBM.gather_edge_counts(1);

  // Initialize a sampler to choose group
  Sampler my_sampler;
  
  int num_trials = 1000;
  int num_times_no_move = 0;
  NodePtr old_group = a1->parent;

  // Run multiple trials and of move and see how often a given node is moved
  for (int i = 0; i < num_trials; ++i)
  {
    // Do move attempt (dry run)
    NodePtr new_group = my_SBM.attempt_move(a1, l1_edges, my_sampler);

    if (new_group->id == old_group->id) num_times_no_move++;
  }
  
  // Prob of a1 staying in a1_1 should be approximately (2 + eps)/(6 + 4*eps)
  // Make sure model's decisions to move a1 reflects this.
  double two = 2;
  double six = 6;
  double four = 4;
  double eps = 0.01;

  ASSERT_NEAR(
    (two + eps)/(six + four*eps),
    double(num_times_no_move)/double(num_trials),
    tol
  );
  
}


TEST(testSBM, node_move_sweeps){
  double tol = 0.01;
  
  int n_trials = 100;
  
  std::set<int> num_nodes_moved;
  
  // Run a node sweep on the same SBM model a bunch of times
  for (int i = 0; i < n_trials; i++)
  {
    // Setup simple SBM model w/ no hierarchy
    SBM my_SBM = build_simple_SBM();
    
    // Run a single sweep at the node-level
    int nodes_moved = my_SBM.run_move_sweep(0);
    
    // Record how many were moved here
    num_nodes_moved.insert(nodes_moved);
  }
  
  // Make sure the number of nodes moved isn't constant
  EXPECT_NE(num_nodes_moved.size(), 1);
  
  // Make sure that if we do the sweep a bunch of times that the later parts
  // sweeps will be more stable because of convergence
  
  // Setup simple SBM model
  SBM my_SBM = build_simple_SBM();
  int n_sweeps = 100;
  vector<int> n_moved_first_third;
  vector<int> n_moved_last_third;
  
  for (int i = 0; i < n_sweeps; i++)
  {
    // Run a single sweep at the node-level
    int n_moved = my_SBM.run_move_sweep(0);
    
    // If we're in first third of sweeps push to first thirds vector
    if(i < (n_sweeps/3)) n_moved_first_third.push_back(n_moved);
    
    // If we're in the last third of sweeps, push to last thirds vector
    if(i > (2*n_sweeps/3)) n_moved_last_third.push_back(n_moved);
  }
  
  // Calculate average num moves for both thirds
  float first_third_avg_moves = std::accumulate(n_moved_first_third.begin(), 
                                                n_moved_first_third.end(), 
                                                0.0 ) / n_moved_first_third.size(); 
  
  float last_third_avg_moves = std::accumulate(n_moved_last_third.begin(), 
                                               n_moved_last_third.end(), 
                                               0.0 ) / n_moved_last_third.size(); 
  
  // std::cout << "First third avg num moves: " << first_third_avg_moves << std::endl;
  // std::cout << "Last third avg num moves: " << last_third_avg_moves << std::endl;
  
  // Hope that the first third is more "volitile" than the last
  ASSERT_TRUE(first_third_avg_moves > last_third_avg_moves);
  
};


TEST(testSBM, mcmc_chain_initialization){
 
  // Setup simple SBM model
  SBM my_SBM = build_simple_SBM();
  
  // Start with every node in its own group
  my_SBM.give_every_node_a_group_at_level(0);
  
  int n_sweeps = 100;
  vector<int> n_moved;
  
  for (int i = 0; i < n_sweeps; i++)
  {
    // Record how many nodes were moved
    n_moved.push_back( my_SBM.run_move_sweep(0));
    
    // Check if structure hasn't changed in last 5 sweeps
    float avg_num_moves = avg_last_n(n_moved, 5);
    
    if (avg_num_moves == 0.00) 
    {
      //std::cout << "Model convered after " << i+1 << " sweeps" << std::endl;
      break;
    } 
    
    if (i == (n_sweeps - 1)) 
    {
      //std::cout << "Model failed to converge in after " << n_sweeps << "sweeps" << "avg num moves at end: "<< avg_num_moves << std::endl;
    }
  }
  
  // Make sure we combined at least some groups
  ASSERT_TRUE(
    my_SBM.get_level(1)->size() < my_SBM.get_level(0)->size()
  );
  
  // // Dump state for printing
  // State_Dump model_state = my_SBM.get_sbm_state();
  // 
  // for (int i = 0; i < model_state.id.size(); i++)
  // {
  //   std::cout << model_state.id[i] << ", " << model_state.parent[i] << std::endl;
  // }
  
};


TEST(testSBM, entropy_calculation){
  
  // Setup simple SBM model
  SBM my_SBM = build_simple_SBM();
  
  EdgeCounts l1_edges = my_SBM.gather_edge_counts(1);

  // Compute full entropy at first level of nodes
  double model_entropy = my_SBM.compute_entropy(0);
 
  // Test entropy is near a hand-calculated value
  // Should be -8 - ( 2*log(2) + 3*log(6) ) - ( 2*log(2/12) + 4*log(4/30) + 1*log(1/5) + 1*log(1) ) = -1.509004
  ASSERT_NEAR(
    model_entropy,
    -1.509004,
    0.1
  );
  
  // (2*log(2/12) + 4*log(4/30) + 1*log(1/5) + 1*log(1) )
  
  // Calculate entropy delta caused by moving a node
  NodePtr node_to_move = my_SBM.get_node_by_id("a1");
  NodePtr from_group = node_to_move->parent;
  NodePtr to_group = my_SBM.get_node_by_id("a12", 1);

  // Calculate the entropy delta along with acceptance prob
  Proposal_Res proposal_results = my_SBM.compute_acceptance_prob(
    l1_edges,
    node_to_move,
    to_group,
    0.1
  );
  
  double entropy_delta = proposal_results.entropy_delta;
  
  // std::cout << "Moving a1 from a11->a12: prob = " << proposal_results.prob_of_accept << std::endl;

  // Now we will actually move the desired node and test to see if entropy has changed
  // Move node
  node_to_move->set_parent(to_group);
 
  // Recalculate entropy
  double new_entropy = my_SBM.compute_entropy(0);
 
  // Get difference from original
  double real_entropy_delta = model_entropy - new_entropy;
  
  EXPECT_NEAR(
    entropy_delta,
    real_entropy_delta,
    0.1
  );
  
  EXPECT_EQ(
    proposal_results.entropy_delta,
    entropy_delta
  );
};

int main(int argc, char* argv[]){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
