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
  EXPECT_EQ(7, my_SBM.nodes[0].size());
  
  EXPECT_EQ("m1, m2, m3, m4, n1, n2, n3", print_node_ids(my_SBM.nodes.at(0)));
  
  // We should have two levels
  EXPECT_EQ(2, my_SBM.nodes.size());
  
  // Group name convention <type>-<level>_<id>
  EXPECT_EQ("0-1_0, 1-1_1", print_node_ids(my_SBM.nodes[1]));

  // Filter to a given node type
  EXPECT_EQ("n1, n2, n3", print_node_ids(my_SBM.get_nodes_of_type_at_level(0,0)));
  EXPECT_EQ("m1, m2, m3, m4", print_node_ids(my_SBM.get_nodes_of_type_at_level(1,0)));
  EXPECT_EQ("1-1_1", print_node_ids(my_SBM.get_nodes_of_type_at_level(1,1)));
  
  // Get number of levels
  EXPECT_EQ(2, my_SBM.nodes.size());
  
  // There should be two types of nodes
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

  EXPECT_EQ(1, my_SBM.nodes[0].size());

  // Add a few more
  my_SBM.add_node("n2", 0);
  my_SBM.add_node("n3", 0);
  my_SBM.add_node("m1", 1);
  my_SBM.add_node("m2", 1);
  my_SBM.add_node("m3", 1);
  my_SBM.add_node("m4", 1);

  EXPECT_EQ(7, my_SBM.nodes[0].size());

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
  EXPECT_EQ(2, my_SBM.nodes.at(1).size());
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
  EXPECT_EQ(18, my_SBM.nodes.at(0).size());

  // Now start initialization of the MCMC chain by assigning every node their
  // own parent group
  my_SBM.give_every_node_a_group_at_level(0);

  // There should be a total of 18 nodes at level 1
  EXPECT_EQ(18, my_SBM.nodes.at(1).size());
  
  // A node from this new level should have a single child
  EXPECT_EQ(1, my_SBM.get_node_from_level(1)->children.size());

}



int main(int argc, char* argv[]){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
