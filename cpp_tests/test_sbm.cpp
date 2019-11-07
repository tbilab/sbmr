#include<gtest/gtest.h>
#include "../helpers.h"
#include "../SBM.h"


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



TEST(testSBM, building_network){
  SBM my_SBM;

  // Start with a single node in the network
  my_SBM.add_node("n1", 0);

  EXPECT_EQ(1, my_SBM.nodes[0]->size());

  // Add a few more
  my_SBM.add_node("n2", 0);
  my_SBM.add_node("n3", 0);
  my_SBM.add_node("m1", 1);
  my_SBM.add_node("m2", 1);
  my_SBM.add_node("m3", 1);
  my_SBM.add_node("m4", 1);

  EXPECT_EQ(7, my_SBM.nodes[0]->size());

  // We should start off with a single level as no group nodes are added
  EXPECT_EQ(1, my_SBM.nodes.size());

  // Create a group node for type 0
  my_SBM.create_group_node(0, 1);
  EXPECT_EQ(2, my_SBM.nodes.size());
  EXPECT_EQ(1, my_SBM.get_nodes_of_type_at_level(0,1).size());


  // Create another group node for type 1
  my_SBM.create_group_node(1, 1);

  // Should be one group node for type 1 at level 1
  EXPECT_EQ(1, my_SBM.get_nodes_of_type_at_level(1,1).size());

  // Should be a total of 2 group nodes for level 1
  EXPECT_EQ(2, my_SBM.nodes.at(1)->size());
}


TEST(testSBM, build_with_connections){
  SBM my_SBM;

  // Add nodes to graph first
  my_SBM.add_node("a1", 1);
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
    

  // There should be 4 total connections between first a group and first b group

  // ... and 5 total out of the first a group
  EXPECT_EQ(2, a1_1->connections_to_node(b1_1).n_total);
  EXPECT_EQ(5, a1_2->connections_to_node(b1_1).n_total);
  EXPECT_EQ(1, a1_3->connections_to_node(b1_1).n_total);

  EXPECT_EQ(6, b1_1->connections_to_node(a1_2).n_total);
  EXPECT_EQ(1, b1_2->connections_to_node(a1_2).n_total);
  EXPECT_EQ(1, b1_3->connections_to_node(a1_2).n_total);


  // Check connection counts between groups... E.g. there should be no
  // connections between first a group and second b group
  EXPECT_EQ(2, a1_1->connections_to_node(b1_1).n_between);
  EXPECT_EQ(0, a1_1->connections_to_node(b1_2).n_between);
  EXPECT_EQ(0, a1_1->connections_to_node(b1_3).n_between);

  EXPECT_EQ(4, a1_2->connections_to_node(b1_1).n_between);
  EXPECT_EQ(0, a1_2->connections_to_node(b1_2).n_between);
  EXPECT_EQ(1, a1_2->connections_to_node(b1_3).n_between);

  EXPECT_EQ(0, a1_3->connections_to_node(b1_1).n_between);
  EXPECT_EQ(1, a1_3->connections_to_node(b1_2).n_between);
  EXPECT_EQ(0, a1_3->connections_to_node(b1_3).n_between);

  EXPECT_EQ(b1_1->connections_to_node(a1_1).n_between,
            a1_1->connections_to_node(b1_1).n_between);

  EXPECT_EQ(b1_2->connections_to_node(a1_1).n_between,
            a1_1->connections_to_node(b1_2).n_between);

  EXPECT_EQ(b1_3->connections_to_node(a1_1).n_between,
            a1_1->connections_to_node(b1_3).n_between);

  EXPECT_EQ(b1_1->connections_to_node(a1_2).n_between,
            a1_2->connections_to_node(b1_1).n_between);

  EXPECT_EQ(b1_2->connections_to_node(a1_2).n_between,
            a1_2->connections_to_node(b1_2).n_between);

  EXPECT_EQ(b1_3->connections_to_node(a1_2).n_between,
            a1_2->connections_to_node(b1_3).n_between);

  EXPECT_EQ(b1_1->connections_to_node(a1_3).n_between,
            a1_3->connections_to_node(b1_1).n_between);

  EXPECT_EQ(b1_2->connections_to_node(a1_3).n_between,
            a1_3->connections_to_node(b1_2).n_between);

  EXPECT_EQ(b1_3->connections_to_node(a1_3).n_between,
            a1_3->connections_to_node(b1_3).n_between);

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
  
  // The edge count map should have 12 non-empty entries 
  // 6 off diagonal + 6 total node counts.
  EXPECT_EQ(12, l1_edges.size());
  
  // Check num edges between groups
  EXPECT_EQ(l1_edges[id_pair("a11", "b11")], 2 );
  EXPECT_EQ(l1_edges[id_pair("a11", "b12")], 0 );
  EXPECT_EQ(l1_edges[id_pair("a11", "b13")], 0 );
  
  EXPECT_EQ(l1_edges[id_pair("a12", "b11")], 4 );
  EXPECT_EQ(l1_edges[id_pair("a12", "b12")], 2 );
  EXPECT_EQ(l1_edges[id_pair("a12", "b13")], 1 );
  
  EXPECT_EQ(l1_edges[id_pair("a13", "b11")], 1 );
  EXPECT_EQ(l1_edges[id_pair("a13", "b12")], 1 );
  EXPECT_EQ(l1_edges[id_pair("a13", "b13")], 0 );
 
  // Direction shouldn't matter
  EXPECT_EQ(l1_edges[id_pair("a11", "b11")],
            l1_edges[id_pair("b11", "a11")]);

  EXPECT_EQ(l1_edges[id_pair("a11", "b12")],
            l1_edges[id_pair("b12", "a11")]);

  EXPECT_EQ(l1_edges[id_pair("a11", "b13")],
            l1_edges[id_pair("b13", "a11")]);

  EXPECT_EQ(l1_edges[id_pair("a12", "b11")],
            l1_edges[id_pair("b11", "a12")]);

  EXPECT_EQ(l1_edges[id_pair("a12", "b12")],
            l1_edges[id_pair("b12", "a12")]);

  EXPECT_EQ(l1_edges[id_pair("a12", "b13")],
            l1_edges[id_pair("b13", "a12")]);

  EXPECT_EQ(l1_edges[id_pair("a13", "b11")],
            l1_edges[id_pair("b11", "a13")]);

  EXPECT_EQ(l1_edges[id_pair("a13", "b12")],
            l1_edges[id_pair("b12", "a13")]);

  EXPECT_EQ(l1_edges[id_pair("a13", "b13")],
            l1_edges[id_pair("b13", "a13")]);


  // Diagonals should hold total edge counts for group
  EXPECT_EQ(l1_edges[id_pair("a11", "a11")], 2);
  EXPECT_EQ(l1_edges[id_pair("a12", "a12")], 7);
  EXPECT_EQ(l1_edges[id_pair("a13", "a13")], 2);
  
  EXPECT_EQ(l1_edges[id_pair("b11", "b11")], 7);
  EXPECT_EQ(l1_edges[id_pair("b12", "b12")], 3);
  EXPECT_EQ(l1_edges[id_pair("b13", "b13")], 1);
  
  // Repeat for level 2
  EdgeCounts l2_edges = my_SBM.gather_edge_counts(2);
  
  // Check num edges between groups
  EXPECT_EQ(l2_edges[id_pair("a21", "b21")], 2);
  EXPECT_EQ(l2_edges[id_pair("a22", "b21")], 9);
  
  EXPECT_EQ(l2_edges[id_pair("a21", "a21")], 2);
  EXPECT_EQ(l2_edges[id_pair("a22", "a22")], 9);
  EXPECT_EQ(l2_edges[id_pair("b21", "b21")], 11);
  
}



int main(int argc, char* argv[]){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
