#include<gtest/gtest.h>
#include "../Node.h"
#include "../SBM.cpp"


TEST(testSBM, basic){
  SBM my_SBM;
  
  // Add some nodes to SBM
  my_SBM.get_node_by_id("n1", 0);
  my_SBM.get_node_by_id("n2", 0);
  my_SBM.get_node_by_id("n3", 0);
  my_SBM.get_node_by_id("n3", 0); // Duplicate node
  my_SBM.get_node_by_id("m1", 1);
  my_SBM.get_node_by_id("m2", 1);
  my_SBM.get_node_by_id("m3", 1);
  my_SBM.get_node_by_id("m4", 1);
  
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
  my_SBM.get_node_by_id("n1", 0);

  EXPECT_EQ(1, my_SBM.nodes[0].size());
  
  // Add a few more
  my_SBM.get_node_by_id("n2", 0);
  my_SBM.get_node_by_id("n3", 0);
  my_SBM.get_node_by_id("m1", 1);
  my_SBM.get_node_by_id("m2", 1);
  my_SBM.get_node_by_id("m3", 1);
  my_SBM.get_node_by_id("m4", 1);
  
  EXPECT_EQ(7, my_SBM.nodes[0].size());
  
  // Make sure duplicates get ignored
  my_SBM.get_node_by_id("n3", 0); // Duplicate node
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

int main(int argc, char* argv[]){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
