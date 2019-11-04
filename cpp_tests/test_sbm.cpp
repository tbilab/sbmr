#include<gtest/gtest.h>
#include "../helpers.cpp"
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
  
  EXPECT_EQ("n1, n2, n3, m1, m2, m3, m4", print_node_ids(*my_SBM.get_node_level(0)));
  
  // Group name convention <type>-<level>_<id>
  EXPECT_EQ("0-1_0, 1-1_1", print_node_ids(*my_SBM.get_node_level(1)));
  
  // Filter to a given node type
  EXPECT_EQ("n1, n2, n3", print_node_ids(my_SBM.get_nodes_of_type_at_level(0,0)));
  EXPECT_EQ("m1, m2, m3, m4", print_node_ids(my_SBM.get_nodes_of_type_at_level(1,0)));
  EXPECT_EQ("1-1_1", print_node_ids(my_SBM.get_nodes_of_type_at_level(1,1)));
  
  // Get number of levels
  EXPECT_EQ(2, my_SBM.nodes.size());
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
  EXPECT_EQ(2, my_SBM.get_node_level(1)->size());
}

TEST(testSBM, build_with_connections){
  SBM my_SBM;
  
  // Add nodes to graph first
  my_SBM.add_node("1", 1);
  my_SBM.add_node("2", 1);
  my_SBM.add_node("3", 1);
  my_SBM.add_node("4", 1);
  my_SBM.add_node("5", 1);
  my_SBM.add_node("10", 1);
  my_SBM.add_node("11", 1);
  my_SBM.add_node("12", 1);
  my_SBM.add_node("13", 1);
  my_SBM.add_node("14", 1);
  my_SBM.add_node("6", 1);
  my_SBM.add_node("7", 1);
  my_SBM.add_node("8", 1);
  my_SBM.add_node("9", 1);
  my_SBM.add_node("1", 0);
  my_SBM.add_node("2", 0);
  my_SBM.add_node("3", 0);
  my_SBM.add_node("4", 0);
  
  // Add connections
  my_SBM.add_connection("1", "1");
  my_SBM.add_connection("2", "1");
  my_SBM.add_connection("3", "1");
  my_SBM.add_connection("4", "1");
  my_SBM.add_connection("5", "1");
  my_SBM.add_connection("1", "2");
  my_SBM.add_connection("2", "2");
  my_SBM.add_connection("3", "2");
  my_SBM.add_connection("4", "2");
  my_SBM.add_connection("5", "2");
  my_SBM.add_connection("10", "2");
  my_SBM.add_connection("11", "2");
  my_SBM.add_connection("12", "2");
  my_SBM.add_connection("13", "2");
  my_SBM.add_connection("14", "2");
  my_SBM.add_connection("6", "3");
  my_SBM.add_connection("7", "3");
  my_SBM.add_connection("8", "3");
  my_SBM.add_connection("9", "3");
  my_SBM.add_connection("10", "3");
  my_SBM.add_connection("11", "3");
  my_SBM.add_connection("12", "3");
  my_SBM.add_connection("13", "3");
  my_SBM.add_connection("14", "3");
  my_SBM.add_connection("8", "4");
  my_SBM.add_connection("10", "4");
  my_SBM.add_connection("11", "4");
  my_SBM.add_connection("12", "4");
  my_SBM.add_connection("13", "4");
  
  // There should be a total of 14 nodes
  EXPECT_EQ(14, my_SBM.nodes.at(0).size());
  
}



int main(int argc, char* argv[]){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
