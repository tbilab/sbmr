#include<gtest/gtest.h>
#include "Node.cpp"

//The only function: simply multiplies a number by 2
double timesTwo(double x){
  return x*2;
}

TEST(testTimesTwo, integerTests){
  
    Node n1("n1", 0, 1),
         n2("n2", 0, 1),
         n3("n3", 0, 1),
         m1("m1", 0, 2),
         m2("m2", 0, 2),
         m3("m3", 0, 2),
         c1("c1", 1, 1),
         c2("c2", 1, 1),
         d1("d1", 1, 2),
         d2("d2", 1, 2);

    n1.set_parent(&c1);
    n2.set_parent(&c1);
    n3.set_parent(&c2);

    m1.set_parent(&d1);
    m2.set_parent(&d2);
    m3.set_parent(&d2);

    Node::connect_nodes(&n1, &m1);
    Node::connect_nodes(&n1, &m3);
    Node::connect_nodes(&n2, &m1);
    Node::connect_nodes(&n3, &m2);
    Node::connect_nodes(&n3, &m3);
    
    
    EXPECT_EQ("n1", n1.id);
    EXPECT_EQ(n1.get_parent_at_level(1)->id, n1.parent->id);
    EXPECT_EQ(2, c1.get_children_at_level(0).size()); 
    EXPECT_EQ(1, d1.get_children_at_level(0).size()); 
    EXPECT_EQ(
      "m1, m3",
      print_node_ids(n1.get_connections_to_level(0))
    );
    EXPECT_EQ(
      "d1, d2",
      print_node_ids(n1.get_connections_to_level(1))
    );
    EXPECT_EQ(
      "d1, d1, d2",
      print_node_ids(c1.get_connections_to_level(1))
    );
    EXPECT_EQ(
      "d2, d2",
      print_node_ids(c2.get_connections_to_level(1))
    );
}

int main(int argc, char* argv[]){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
