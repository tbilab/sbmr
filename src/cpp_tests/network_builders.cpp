#include "../SBM.h"

// SBM build_simulated_SBM()
// {
//   // This function implements a network built using the bisbmsim package
//   // produced for the TBI lab and is built using the R script at
//   // /tools/simulate_test_network.R
//   SBM my_SBM;

//   my_SBM.add_node("a1", 1);
//   my_SBM.add_node("a2", 1);
//   my_SBM.add_node("a3", 1);
//   my_SBM.add_node("a4", 1);
//   my_SBM.add_node("a5", 1);
//   my_SBM.add_node("a10", 1);
//   my_SBM.add_node("a11", 1);
//   my_SBM.add_node("a12", 1);
//   my_SBM.add_node("a13", 1);
//   my_SBM.add_node("a14", 1);
//   my_SBM.add_node("a6", 1);
//   my_SBM.add_node("a7", 1);
//   my_SBM.add_node("a8", 1);
//   my_SBM.add_node("a9", 1);
//   my_SBM.add_node("b1", 0);
//   my_SBM.add_node("b2", 0);
//   my_SBM.add_node("b3", 0);
//   my_SBM.add_node("b4", 0);

//   my_SBM.add_edge("a1", "b1");
//   my_SBM.add_edge("a2", "b1");
//   my_SBM.add_edge("a3", "b1");
//   my_SBM.add_edge("a4", "b1");
//   my_SBM.add_edge("a5", "b1");
//   my_SBM.add_edge("a1", "b2");
//   my_SBM.add_edge("a2", "b2");
//   my_SBM.add_edge("a3", "b2");
//   my_SBM.add_edge("a4", "b2");
//   my_SBM.add_edge("a5", "b2");
//   my_SBM.add_edge("a10", "b2");
//   my_SBM.add_edge("a11", "b2");
//   my_SBM.add_edge("a12", "b2");
//   my_SBM.add_edge("a13", "b2");
//   my_SBM.add_edge("a14", "b2");
//   my_SBM.add_edge("a6", "b3");
//   my_SBM.add_edge("a7", "b3");
//   my_SBM.add_edge("a8", "b3");
//   my_SBM.add_edge("a9", "b3");
//   my_SBM.add_edge("a10", "b3");
//   my_SBM.add_edge("a11", "b3");
//   my_SBM.add_edge("a12", "b3");
//   my_SBM.add_edge("a13", "b3");
//   my_SBM.add_edge("a14", "b3");
//   my_SBM.add_edge("a10", "b4");
//   my_SBM.add_edge("a11", "b4");
//   my_SBM.add_edge("a12", "b4");
//   my_SBM.add_edge("a13", "b4");
//   my_SBM.add_edge("a14", "b4");

//   return my_SBM;
// }

// Loads a simple bipartite sbm model with optional hierarchy added
SBM build_simple_SBM()
{

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
  NodePtr a1 = my_SBM.add_node("a1", "a");
  NodePtr a2 = my_SBM.add_node("a2", "a");
  NodePtr a3 = my_SBM.add_node("a3", "a");
  NodePtr a4 = my_SBM.add_node("a4", "a");

  NodePtr b1 = my_SBM.add_node("b1", "b");
  NodePtr b2 = my_SBM.add_node("b2", "b");
  NodePtr b3 = my_SBM.add_node("b3", "b");
  NodePtr b4 = my_SBM.add_node("b4", "b");

  // Add edges
  my_SBM.add_edge(a1, b2);

  my_SBM.add_edge(a2, b1);
  my_SBM.add_edge(a2, b2);

  my_SBM.add_edge(a3, b1);
  my_SBM.add_edge(a3, b2);
  my_SBM.add_edge(a3, b4);

  my_SBM.add_edge(a4, b3);

  // Make 2 type 0/a blocks
  NodePtr a11 = my_SBM.add_node("a11", "a", 1);
  NodePtr a12 = my_SBM.add_node("a12", "a", 1);
  NodePtr a13 = my_SBM.add_node("a13", "a", 1);

  NodePtr b11 = my_SBM.add_node("b11", "b", 1);
  NodePtr b12 = my_SBM.add_node("b12", "b", 1);
  NodePtr b13 = my_SBM.add_node("b13", "b", 1);

  // Assign nodes to their blocks
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

// Loads a simple unipartite sbm model with hierarchy added
SBM build_simple_SBM_unipartite()
{
  SBM my_SBM;

  // Add nodes to graph first
  NodePtr n1 = my_SBM.add_node("n1", "a");
  NodePtr n2 = my_SBM.add_node("n2", "a");
  NodePtr n3 = my_SBM.add_node("n3", "a");
  NodePtr n4 = my_SBM.add_node("n4", "a");
  NodePtr n5 = my_SBM.add_node("n5", "a");
  NodePtr n6 = my_SBM.add_node("n6", "a");
  // Add edge"a"
  my_SBM.add_edge(n1, n2);
  my_SBM.add_edge(n1, n3);
  my_SBM.add_edge(n1, n4);
  my_SBM.add_edge(n1, n5);
  my_SBM.add_edge(n2, n3);
  my_SBM.add_edge(n2, n4);
  my_SBM.add_edge(n2, n5);
  my_SBM.add_edge(n3, n4);
  my_SBM.add_edge(n3, n6);
  my_SBM.add_edge(n4, n5);
  my_SBM.add_edge(n4, n6);
  my_SBM.add_edge(n5, n6);

  // Make 3 blocks
  NodePtr a = my_SBM.add_node("a", "a", 1);
  NodePtr b = my_SBM.add_node("b", "a", 1);
  NodePtr c = my_SBM.add_node("c", "a", 1);

  // Assign nodes to their blocks
  n1->set_parent(a);
  n2->set_parent(a);

  n3->set_parent(b);
  n4->set_parent(b);

  n5->set_parent(c);
  n6->set_parent(c);

  return my_SBM;
}

SBM build_bipartite_simulated()
{
  // This function implements a network built using the bisbmsim package
  // produced for the TBI lab and is built using the R script at
  // /tools/simulate_test_network.R
  SBM my_SBM;

  my_SBM.add_node("a4",  "a");
  my_SBM.add_node("a5",  "a");
  my_SBM.add_node("a7",  "a");
  my_SBM.add_node("a10", "a");
  my_SBM.add_node("a13", "a");
  my_SBM.add_node("a22", "a");
  my_SBM.add_node("a23", "a");
  my_SBM.add_node("a24", "a");
  my_SBM.add_node("a1",  "a");
  my_SBM.add_node("a2",  "a");
  my_SBM.add_node("a3",  "a");
  my_SBM.add_node("a6",  "a");
  my_SBM.add_node("a21", "a");
  my_SBM.add_node("a25", "a");
  my_SBM.add_node("a8",  "a");
  my_SBM.add_node("a9",  "a");
  my_SBM.add_node("a11", "a");
  my_SBM.add_node("a12", "a");
  my_SBM.add_node("a14", "a");
  my_SBM.add_node("a15", "a");
  my_SBM.add_node("a16", "a");
  my_SBM.add_node("a17", "a");
  my_SBM.add_node("a20", "a");
  my_SBM.add_node("a18", "a");
  my_SBM.add_node("a19", "a");
  my_SBM.add_node("b1",  "b");
  my_SBM.add_node("b2",  "b");
  my_SBM.add_node("b3",  "b");
  my_SBM.add_node("b4",  "b");
  my_SBM.add_node("b5",  "b");
  my_SBM.add_node("b6",  "b");
  my_SBM.add_node("b7",  "b");
  my_SBM.add_node("b8",  "b");
  my_SBM.add_node("b9",  "b");
  my_SBM.add_node("b10", "b");
  my_SBM.add_node("b11", "b");
  my_SBM.add_node("b12", "b");
  my_SBM.add_node("b13", "b");
  my_SBM.add_node("b14", "b");
  my_SBM.add_node("b15", "b");
  my_SBM.add_node("b16", "b");
  my_SBM.add_node("b17", "b");
  my_SBM.add_node("b18", "b");
  my_SBM.add_node("b19", "b");
  my_SBM.add_node("b20", "b");
  my_SBM.add_node("b21", "b");
  my_SBM.add_node("b22", "b");
  my_SBM.add_node("b23", "b");
  my_SBM.add_node("b24", "b");
  my_SBM.add_node("b25", "b");

  my_SBM.add_edge("a4", "b1");
  my_SBM.add_edge("a5", "b1");
  my_SBM.add_edge("a7", "b1");
  my_SBM.add_edge("a10", "b1");
  my_SBM.add_edge("a13", "b1");
  my_SBM.add_edge("a22", "b1");
  my_SBM.add_edge("a23", "b1");
  my_SBM.add_edge("a24", "b1");
  my_SBM.add_edge("a1", "b2");
  my_SBM.add_edge("a2", "b2");
  my_SBM.add_edge("a3", "b2");
  my_SBM.add_edge("a6", "b2");
  my_SBM.add_edge("a21", "b2");
  my_SBM.add_edge("a25", "b2");
  my_SBM.add_edge("a1", "b3");
  my_SBM.add_edge("a5", "b3");
  my_SBM.add_edge("a6", "b3");
  my_SBM.add_edge("a8", "b3");
  my_SBM.add_edge("a9", "b3");
  my_SBM.add_edge("a11", "b3");
  my_SBM.add_edge("a12", "b3");
  my_SBM.add_edge("a14", "b3");
  my_SBM.add_edge("a15", "b3");
  my_SBM.add_edge("a16", "b3");
  my_SBM.add_edge("a17", "b3");
  my_SBM.add_edge("a20", "b3");
  my_SBM.add_edge("a23", "b3");
  my_SBM.add_edge("a25", "b3");
  my_SBM.add_edge("a1", "b4");
  my_SBM.add_edge("a3", "b4");
  my_SBM.add_edge("a9", "b4");
  my_SBM.add_edge("a10", "b4");
  my_SBM.add_edge("a13", "b4");
  my_SBM.add_edge("a18", "b4");
  my_SBM.add_edge("a23", "b4");
  my_SBM.add_edge("a1", "b5");
  my_SBM.add_edge("a3", "b5");
  my_SBM.add_edge("a8", "b5");
  my_SBM.add_edge("a9", "b5");
  my_SBM.add_edge("a12", "b5");
  my_SBM.add_edge("a13", "b5");
  my_SBM.add_edge("a18", "b5");
  my_SBM.add_edge("a19", "b5");
  my_SBM.add_edge("a20", "b5");
  my_SBM.add_edge("a25", "b5");
  my_SBM.add_edge("a1", "b6");
  my_SBM.add_edge("a6", "b6");
  my_SBM.add_edge("a7", "b6");
  my_SBM.add_edge("a9", "b6");
  my_SBM.add_edge("a11", "b6");
  my_SBM.add_edge("a12", "b6");
  my_SBM.add_edge("a16", "b6");
  my_SBM.add_edge("a19", "b6");
  my_SBM.add_edge("a20", "b6");
  my_SBM.add_edge("a21", "b6");
  my_SBM.add_edge("a22", "b6");
  my_SBM.add_edge("a23", "b6");
  my_SBM.add_edge("a24", "b6");
  my_SBM.add_edge("a1", "b7");
  my_SBM.add_edge("a4", "b7");
  my_SBM.add_edge("a8", "b7");
  my_SBM.add_edge("a12", "b7");
  my_SBM.add_edge("a13", "b7");
  my_SBM.add_edge("a15", "b7");
  my_SBM.add_edge("a16", "b7");
  my_SBM.add_edge("a17", "b7");
  my_SBM.add_edge("a19", "b7");
  my_SBM.add_edge("a24", "b7");
  my_SBM.add_edge("a25", "b7");
  my_SBM.add_edge("a2", "b8");
  my_SBM.add_edge("a5", "b8");
  my_SBM.add_edge("a6", "b8");
  my_SBM.add_edge("a9", "b8");
  my_SBM.add_edge("a10", "b8");
  my_SBM.add_edge("a13", "b8");
  my_SBM.add_edge("a16", "b8");
  my_SBM.add_edge("a21", "b8");
  my_SBM.add_edge("a22", "b8");
  my_SBM.add_edge("a1", "b9");
  my_SBM.add_edge("a2", "b9");
  my_SBM.add_edge("a4", "b9");
  my_SBM.add_edge("a11", "b9");
  my_SBM.add_edge("a14", "b9");
  my_SBM.add_edge("a16", "b9");
  my_SBM.add_edge("a17", "b9");
  my_SBM.add_edge("a21", "b9");
  my_SBM.add_edge("a24", "b9");
  my_SBM.add_edge("a25", "b9");
  my_SBM.add_edge("a1", "b10");
  my_SBM.add_edge("a2", "b10");
  my_SBM.add_edge("a11", "b10");
  my_SBM.add_edge("a13", "b10");
  my_SBM.add_edge("a19", "b10");
  my_SBM.add_edge("a25", "b10");
  my_SBM.add_edge("a1", "b11");
  my_SBM.add_edge("a7", "b11");
  my_SBM.add_edge("a8", "b11");
  my_SBM.add_edge("a13", "b11");
  my_SBM.add_edge("a18", "b11");
  my_SBM.add_edge("a21", "b11");
  my_SBM.add_edge("a1", "b12");
  my_SBM.add_edge("a6", "b12");
  my_SBM.add_edge("a8", "b12");
  my_SBM.add_edge("a17", "b12");
  my_SBM.add_edge("a20", "b12");
  my_SBM.add_edge("a25", "b12");
  my_SBM.add_edge("a1", "b13");
  my_SBM.add_edge("a3", "b13");
  my_SBM.add_edge("a4", "b13");
  my_SBM.add_edge("a9", "b13");
  my_SBM.add_edge("a13", "b13");
  my_SBM.add_edge("a15", "b13");
  my_SBM.add_edge("a18", "b13");
  my_SBM.add_edge("a19", "b13");
  my_SBM.add_edge("a20", "b13");
  my_SBM.add_edge("a22", "b13");
  my_SBM.add_edge("a25", "b13");
  my_SBM.add_edge("a2", "b14");
  my_SBM.add_edge("a3", "b14");
  my_SBM.add_edge("a5", "b14");
  my_SBM.add_edge("a7", "b14");
  my_SBM.add_edge("a8", "b14");
  my_SBM.add_edge("a9", "b14");
  my_SBM.add_edge("a12", "b14");
  my_SBM.add_edge("a13", "b14");
  my_SBM.add_edge("a16", "b14");
  my_SBM.add_edge("a18", "b14");
  my_SBM.add_edge("a22", "b14");
  my_SBM.add_edge("a23", "b14");
  my_SBM.add_edge("a24", "b14");
  my_SBM.add_edge("a1", "b15");
  my_SBM.add_edge("a5", "b15");
  my_SBM.add_edge("a12", "b15");
  my_SBM.add_edge("a13", "b15");
  my_SBM.add_edge("a14", "b15");
  my_SBM.add_edge("a18", "b15");
  my_SBM.add_edge("a20", "b15");
  my_SBM.add_edge("a21", "b15");
  my_SBM.add_edge("a23", "b15");
  my_SBM.add_edge("a1", "b16");
  my_SBM.add_edge("a4", "b16");
  my_SBM.add_edge("a10", "b16");
  my_SBM.add_edge("a12", "b16");
  my_SBM.add_edge("a13", "b16");
  my_SBM.add_edge("a14", "b16");
  my_SBM.add_edge("a19", "b16");
  my_SBM.add_edge("a22", "b16");
  my_SBM.add_edge("a1", "b17");
  my_SBM.add_edge("a3", "b17");
  my_SBM.add_edge("a8", "b17");
  my_SBM.add_edge("a9", "b17");
  my_SBM.add_edge("a10", "b17");
  my_SBM.add_edge("a12", "b17");
  my_SBM.add_edge("a17", "b17");
  my_SBM.add_edge("a18", "b17");
  my_SBM.add_edge("a19", "b17");
  my_SBM.add_edge("a21", "b17");
  my_SBM.add_edge("a2", "b18");
  my_SBM.add_edge("a6", "b18");
  my_SBM.add_edge("a8", "b18");
  my_SBM.add_edge("a11", "b18");
  my_SBM.add_edge("a12", "b18");
  my_SBM.add_edge("a13", "b18");
  my_SBM.add_edge("a14", "b18");
  my_SBM.add_edge("a15", "b18");
  my_SBM.add_edge("a16", "b18");
  my_SBM.add_edge("a17", "b18");
  my_SBM.add_edge("a18", "b18");
  my_SBM.add_edge("a20", "b18");
  my_SBM.add_edge("a21", "b18");
  my_SBM.add_edge("a23", "b18");
  my_SBM.add_edge("a24", "b18");
  my_SBM.add_edge("a1", "b19");
  my_SBM.add_edge("a2", "b19");
  my_SBM.add_edge("a4", "b19");
  my_SBM.add_edge("a5", "b19");
  my_SBM.add_edge("a7", "b19");
  my_SBM.add_edge("a13", "b19");
  my_SBM.add_edge("a20", "b19");
  my_SBM.add_edge("a21", "b19");
  my_SBM.add_edge("a25", "b19");
  my_SBM.add_edge("a1", "b20");
  my_SBM.add_edge("a4", "b20");
  my_SBM.add_edge("a6", "b20");
  my_SBM.add_edge("a9", "b20");
  my_SBM.add_edge("a10", "b20");
  my_SBM.add_edge("a12", "b20");
  my_SBM.add_edge("a15", "b20");
  my_SBM.add_edge("a18", "b20");
  my_SBM.add_edge("a19", "b20");
  my_SBM.add_edge("a24", "b20");
  my_SBM.add_edge("a2", "b21");
  my_SBM.add_edge("a3", "b21");
  my_SBM.add_edge("a6", "b21");
  my_SBM.add_edge("a8", "b21");
  my_SBM.add_edge("a10", "b21");
  my_SBM.add_edge("a11", "b21");
  my_SBM.add_edge("a14", "b21");
  my_SBM.add_edge("a16", "b21");
  my_SBM.add_edge("a17", "b21");
  my_SBM.add_edge("a18", "b21");
  my_SBM.add_edge("a20", "b21");
  my_SBM.add_edge("a21", "b21");
  my_SBM.add_edge("a22", "b21");
  my_SBM.add_edge("a23", "b21");
  my_SBM.add_edge("a3", "b22");
  my_SBM.add_edge("a4", "b22");
  my_SBM.add_edge("a21", "b22");
  my_SBM.add_edge("a23", "b22");
  my_SBM.add_edge("a25", "b22");
  my_SBM.add_edge("a3", "b23");
  my_SBM.add_edge("a5", "b23");
  my_SBM.add_edge("a7", "b23");
  my_SBM.add_edge("a8", "b23");
  my_SBM.add_edge("a9", "b23");
  my_SBM.add_edge("a11", "b23");
  my_SBM.add_edge("a13", "b23");
  my_SBM.add_edge("a15", "b23");
  my_SBM.add_edge("a17", "b23");
  my_SBM.add_edge("a19", "b23");
  my_SBM.add_edge("a20", "b23");
  my_SBM.add_edge("a22", "b23");
  my_SBM.add_edge("a1", "b24");
  my_SBM.add_edge("a5", "b24");
  my_SBM.add_edge("a8", "b24");
  my_SBM.add_edge("a11", "b24");
  my_SBM.add_edge("a12", "b24");
  my_SBM.add_edge("a13", "b24");
  my_SBM.add_edge("a14", "b24");
  my_SBM.add_edge("a17", "b24");
  my_SBM.add_edge("a20", "b24");
  my_SBM.add_edge("a21", "b24");
  my_SBM.add_edge("a24", "b24");
  my_SBM.add_edge("a1", "b25");
  my_SBM.add_edge("a3", "b25");
  my_SBM.add_edge("a4", "b25");
  my_SBM.add_edge("a5", "b25");
  my_SBM.add_edge("a7", "b25");
  my_SBM.add_edge("a10", "b25");
  my_SBM.add_edge("a13", "b25");
  my_SBM.add_edge("a15", "b25");
  my_SBM.add_edge("a16", "b25");
  my_SBM.add_edge("a19", "b25");
  my_SBM.add_edge("a22", "b25");
  my_SBM.add_edge("a24", "b25");
  my_SBM.add_edge("a25", "b25");

  return my_SBM;
}

// A big ugly but useful simulated dataset with 3 clusters
// Made with the following script in R using the sbmR package
// set.seed(42)
// n_blocks <- 3   # Total number of blocks
// block_size <- 15 # How many nodes in each block
// network <- sim_basic_block_network(
//   n_blocks = n_blocks,
//   n_nodes_per_block = block_size,
//   return_edge_propensities = TRUE
// )
// network %>%
//   pluck("nodes") %>%
//   glue_data("my_SBM.add_node(\"{id}\", 0);")
// network %>%
//   pluck("edges") %>%
//   glue_data("my_SBM.add_edge(\"{from}\", \"{to}\");")
SBM build_unipartite_simulated()
{
  SBM my_SBM;

  my_SBM.add_node("g1_1");
  my_SBM.add_node("g1_2");
  my_SBM.add_node("g1_3");
  my_SBM.add_node("g1_4");
  my_SBM.add_node("g1_5");
  my_SBM.add_node("g1_6");
  my_SBM.add_node("g1_7");
  my_SBM.add_node("g1_8");
  my_SBM.add_node("g1_9");
  my_SBM.add_node("g1_10");
  my_SBM.add_node("g1_11");
  my_SBM.add_node("g1_12");
  my_SBM.add_node("g1_13");
  my_SBM.add_node("g1_14");
  my_SBM.add_node("g1_15");
  my_SBM.add_node("g2_1");
  my_SBM.add_node("g2_2");
  my_SBM.add_node("g2_3");
  my_SBM.add_node("g2_4");
  my_SBM.add_node("g2_5");
  my_SBM.add_node("g2_6");
  my_SBM.add_node("g2_7");
  my_SBM.add_node("g2_8");
  my_SBM.add_node("g2_9");
  my_SBM.add_node("g2_10");
  my_SBM.add_node("g2_11");
  my_SBM.add_node("g2_12");
  my_SBM.add_node("g2_13");
  my_SBM.add_node("g2_14");
  my_SBM.add_node("g2_15");
  my_SBM.add_node("g3_1");
  my_SBM.add_node("g3_2");
  my_SBM.add_node("g3_3");
  my_SBM.add_node("g3_4");
  my_SBM.add_node("g3_5");
  my_SBM.add_node("g3_6");
  my_SBM.add_node("g3_7");
  my_SBM.add_node("g3_8");
  my_SBM.add_node("g3_9");
  my_SBM.add_node("g3_10");
  my_SBM.add_node("g3_11");
  my_SBM.add_node("g3_12");
  my_SBM.add_node("g3_13");
  my_SBM.add_node("g3_14");
  my_SBM.add_node("g3_15");

  my_SBM.add_edge("g1_1", "g1_7");
  my_SBM.add_edge("g1_1", "g1_8");
  my_SBM.add_edge("g1_1", "g1_10");
  my_SBM.add_edge("g1_1", "g1_15");
  my_SBM.add_edge("g1_1", "g3_1");
  my_SBM.add_edge("g1_1", "g3_14");
  my_SBM.add_edge("g1_2", "g1_3");
  my_SBM.add_edge("g1_2", "g1_6");
  my_SBM.add_edge("g1_2", "g1_9");
  my_SBM.add_edge("g1_2", "g1_13");
  my_SBM.add_edge("g1_2", "g1_14");
  my_SBM.add_edge("g1_2", "g1_15");
  my_SBM.add_edge("g1_2", "g3_2");
  my_SBM.add_edge("g1_2", "g3_4");
  my_SBM.add_edge("g1_2", "g3_6");
  my_SBM.add_edge("g1_2", "g3_8");
  my_SBM.add_edge("g1_2", "g3_9");
  my_SBM.add_edge("g1_3", "g1_4");
  my_SBM.add_edge("g1_3", "g1_5");
  my_SBM.add_edge("g1_3", "g1_6");
  my_SBM.add_edge("g1_3", "g1_10");
  my_SBM.add_edge("g1_3", "g1_11");
  my_SBM.add_edge("g1_3", "g1_12");
  my_SBM.add_edge("g1_3", "g1_13");
  my_SBM.add_edge("g1_3", "g2_14");
  my_SBM.add_edge("g1_3", "g2_15");
  my_SBM.add_edge("g1_3", "g3_12");
  my_SBM.add_edge("g1_4", "g1_7");
  my_SBM.add_edge("g1_4", "g1_12");
  my_SBM.add_edge("g1_4", "g2_11");
  my_SBM.add_edge("g1_4", "g3_3");
  my_SBM.add_edge("g1_4", "g3_8");
  my_SBM.add_edge("g1_4", "g3_14");
  my_SBM.add_edge("g1_5", "g1_10");
  my_SBM.add_edge("g1_5", "g1_11");
  my_SBM.add_edge("g1_5", "g1_12");
  my_SBM.add_edge("g1_5", "g1_14");
  my_SBM.add_edge("g1_5", "g2_5");
  my_SBM.add_edge("g1_5", "g3_1");
  my_SBM.add_edge("g1_5", "g3_8");
  my_SBM.add_edge("g1_5", "g3_14");
  my_SBM.add_edge("g1_6", "g1_7");
  my_SBM.add_edge("g1_6", "g1_8");
  my_SBM.add_edge("g1_6", "g1_13");
  my_SBM.add_edge("g1_6", "g1_14");
  my_SBM.add_edge("g1_6", "g1_15");
  my_SBM.add_edge("g1_6", "g3_1");
  my_SBM.add_edge("g1_6", "g3_3");
  my_SBM.add_edge("g1_6", "g3_5");
  my_SBM.add_edge("g1_6", "g3_7");
  my_SBM.add_edge("g1_6", "g3_10");
  my_SBM.add_edge("g1_6", "g3_12");
  my_SBM.add_edge("g1_6", "g3_15");
  my_SBM.add_edge("g1_7", "g1_9");
  my_SBM.add_edge("g1_7", "g1_11");
  my_SBM.add_edge("g1_7", "g1_13");
  my_SBM.add_edge("g1_7", "g1_15");
  my_SBM.add_edge("g1_7", "g2_6");
  my_SBM.add_edge("g1_7", "g2_10");
  my_SBM.add_edge("g1_7", "g3_3");
  my_SBM.add_edge("g1_7", "g3_6");
  my_SBM.add_edge("g1_7", "g3_10");
  my_SBM.add_edge("g1_7", "g3_11");
  my_SBM.add_edge("g1_7", "g3_15");
  my_SBM.add_edge("g1_8", "g1_9");
  my_SBM.add_edge("g1_8", "g1_12");
  my_SBM.add_edge("g1_8", "g2_10");
  my_SBM.add_edge("g1_8", "g3_3");
  my_SBM.add_edge("g1_8", "g3_10");
  my_SBM.add_edge("g1_8", "g3_11");
  my_SBM.add_edge("g1_9", "g1_12");
  my_SBM.add_edge("g1_9", "g1_13");
  my_SBM.add_edge("g1_9", "g1_15");
  my_SBM.add_edge("g1_9", "g3_2");
  my_SBM.add_edge("g1_9", "g3_5");
  my_SBM.add_edge("g1_9", "g3_12");
  my_SBM.add_edge("g1_10", "g1_11");
  my_SBM.add_edge("g1_10", "g1_12");
  my_SBM.add_edge("g1_10", "g1_14");
  my_SBM.add_edge("g1_10", "g1_15");
  my_SBM.add_edge("g1_10", "g3_7");
  my_SBM.add_edge("g1_10", "g3_11");
  my_SBM.add_edge("g1_10", "g3_14");
  my_SBM.add_edge("g1_10", "g3_15");
  my_SBM.add_edge("g1_11", "g2_7");
  my_SBM.add_edge("g1_11", "g2_9");
  my_SBM.add_edge("g1_11", "g3_2");
  my_SBM.add_edge("g1_11", "g3_4");
  my_SBM.add_edge("g1_12", "g1_13");
  my_SBM.add_edge("g1_12", "g1_14");
  my_SBM.add_edge("g1_12", "g1_15");
  my_SBM.add_edge("g1_12", "g2_9");
  my_SBM.add_edge("g1_12", "g3_5");
  my_SBM.add_edge("g1_12", "g3_8");
  my_SBM.add_edge("g1_13", "g2_12");
  my_SBM.add_edge("g1_13", "g3_3");
  my_SBM.add_edge("g1_14", "g3_2");
  my_SBM.add_edge("g1_14", "g3_7");
  my_SBM.add_edge("g1_15", "g2_8");
  my_SBM.add_edge("g1_15", "g3_3");
  my_SBM.add_edge("g1_15", "g3_6");
  my_SBM.add_edge("g1_15", "g3_9");
  my_SBM.add_edge("g1_15", "g3_10");
  my_SBM.add_edge("g1_15", "g3_13");
  my_SBM.add_edge("g1_15", "g3_14");
  my_SBM.add_edge("g2_1", "g2_3");
  my_SBM.add_edge("g2_1", "g2_6");
  my_SBM.add_edge("g2_1", "g2_7");
  my_SBM.add_edge("g2_1", "g2_9");
  my_SBM.add_edge("g2_1", "g2_10");
  my_SBM.add_edge("g2_1", "g2_11");
  my_SBM.add_edge("g2_1", "g2_12");
  my_SBM.add_edge("g2_1", "g2_13");
  my_SBM.add_edge("g2_1", "g2_14");
  my_SBM.add_edge("g2_1", "g2_15");
  my_SBM.add_edge("g2_1", "g3_1");
  my_SBM.add_edge("g2_1", "g3_2");
  my_SBM.add_edge("g2_1", "g3_3");
  my_SBM.add_edge("g2_1", "g3_4");
  my_SBM.add_edge("g2_1", "g3_5");
  my_SBM.add_edge("g2_1", "g3_7");
  my_SBM.add_edge("g2_1", "g3_8");
  my_SBM.add_edge("g2_1", "g3_9");
  my_SBM.add_edge("g2_1", "g3_10");
  my_SBM.add_edge("g2_1", "g3_11");
  my_SBM.add_edge("g2_1", "g3_13");
  my_SBM.add_edge("g2_1", "g3_14");
  my_SBM.add_edge("g2_1", "g3_15");
  my_SBM.add_edge("g2_2", "g2_4");
  my_SBM.add_edge("g2_2", "g2_5");
  my_SBM.add_edge("g2_2", "g2_6");
  my_SBM.add_edge("g2_2", "g2_7");
  my_SBM.add_edge("g2_2", "g2_9");
  my_SBM.add_edge("g2_2", "g2_10");
  my_SBM.add_edge("g2_2", "g2_11");
  my_SBM.add_edge("g2_2", "g2_12");
  my_SBM.add_edge("g2_2", "g3_1");
  my_SBM.add_edge("g2_2", "g3_2");
  my_SBM.add_edge("g2_2", "g3_3");
  my_SBM.add_edge("g2_2", "g3_4");
  my_SBM.add_edge("g2_2", "g3_5");
  my_SBM.add_edge("g2_2", "g3_6");
  my_SBM.add_edge("g2_2", "g3_7");
  my_SBM.add_edge("g2_2", "g3_8");
  my_SBM.add_edge("g2_2", "g3_9");
  my_SBM.add_edge("g2_2", "g3_10");
  my_SBM.add_edge("g2_2", "g3_11");
  my_SBM.add_edge("g2_2", "g3_12");
  my_SBM.add_edge("g2_2", "g3_13");
  my_SBM.add_edge("g2_2", "g3_14");
  my_SBM.add_edge("g2_2", "g3_15");
  my_SBM.add_edge("g2_3", "g2_5");
  my_SBM.add_edge("g2_3", "g2_6");
  my_SBM.add_edge("g2_3", "g2_8");
  my_SBM.add_edge("g2_3", "g2_10");
  my_SBM.add_edge("g2_3", "g2_11");
  my_SBM.add_edge("g2_3", "g2_12");
  my_SBM.add_edge("g2_3", "g2_14");
  my_SBM.add_edge("g2_3", "g2_15");
  my_SBM.add_edge("g2_3", "g3_1");
  my_SBM.add_edge("g2_3", "g3_2");
  my_SBM.add_edge("g2_3", "g3_3");
  my_SBM.add_edge("g2_3", "g3_4");
  my_SBM.add_edge("g2_3", "g3_5");
  my_SBM.add_edge("g2_3", "g3_6");
  my_SBM.add_edge("g2_3", "g3_7");
  my_SBM.add_edge("g2_3", "g3_8");
  my_SBM.add_edge("g2_3", "g3_9");
  my_SBM.add_edge("g2_3", "g3_10");
  my_SBM.add_edge("g2_3", "g3_11");
  my_SBM.add_edge("g2_3", "g3_13");
  my_SBM.add_edge("g2_3", "g3_14");
  my_SBM.add_edge("g2_3", "g3_15");
  my_SBM.add_edge("g2_4", "g2_5");
  my_SBM.add_edge("g2_4", "g2_7");
  my_SBM.add_edge("g2_4", "g2_10");
  my_SBM.add_edge("g2_4", "g2_11");
  my_SBM.add_edge("g2_4", "g2_13");
  my_SBM.add_edge("g2_4", "g2_14");
  my_SBM.add_edge("g2_4", "g2_15");
  my_SBM.add_edge("g2_4", "g3_1");
  my_SBM.add_edge("g2_4", "g3_2");
  my_SBM.add_edge("g2_4", "g3_3");
  my_SBM.add_edge("g2_4", "g3_4");
  my_SBM.add_edge("g2_4", "g3_5");
  my_SBM.add_edge("g2_4", "g3_6");
  my_SBM.add_edge("g2_4", "g3_7");
  my_SBM.add_edge("g2_4", "g3_8");
  my_SBM.add_edge("g2_4", "g3_9");
  my_SBM.add_edge("g2_4", "g3_10");
  my_SBM.add_edge("g2_4", "g3_11");
  my_SBM.add_edge("g2_4", "g3_12");
  my_SBM.add_edge("g2_4", "g3_13");
  my_SBM.add_edge("g2_4", "g3_15");
  my_SBM.add_edge("g2_5", "g2_7");
  my_SBM.add_edge("g2_5", "g2_8");
  my_SBM.add_edge("g2_5", "g2_9");
  my_SBM.add_edge("g2_5", "g2_12");
  my_SBM.add_edge("g2_5", "g2_13");
  my_SBM.add_edge("g2_5", "g2_14");
  my_SBM.add_edge("g2_5", "g3_1");
  my_SBM.add_edge("g2_5", "g3_2");
  my_SBM.add_edge("g2_5", "g3_3");
  my_SBM.add_edge("g2_5", "g3_4");
  my_SBM.add_edge("g2_5", "g3_5");
  my_SBM.add_edge("g2_5", "g3_6");
  my_SBM.add_edge("g2_5", "g3_7");
  my_SBM.add_edge("g2_5", "g3_8");
  my_SBM.add_edge("g2_5", "g3_9");
  my_SBM.add_edge("g2_5", "g3_11");
  my_SBM.add_edge("g2_5", "g3_12");
  my_SBM.add_edge("g2_5", "g3_13");
  my_SBM.add_edge("g2_5", "g3_14");
  my_SBM.add_edge("g2_5", "g3_15");
  my_SBM.add_edge("g2_6", "g2_7");
  my_SBM.add_edge("g2_6", "g2_8");
  my_SBM.add_edge("g2_6", "g2_10");
  my_SBM.add_edge("g2_6", "g2_11");
  my_SBM.add_edge("g2_6", "g2_13");
  my_SBM.add_edge("g2_6", "g2_14");
  my_SBM.add_edge("g2_6", "g2_15");
  my_SBM.add_edge("g2_6", "g3_1");
  my_SBM.add_edge("g2_6", "g3_2");
  my_SBM.add_edge("g2_6", "g3_3");
  my_SBM.add_edge("g2_6", "g3_4");
  my_SBM.add_edge("g2_6", "g3_5");
  my_SBM.add_edge("g2_6", "g3_6");
  my_SBM.add_edge("g2_6", "g3_7");
  my_SBM.add_edge("g2_6", "g3_8");
  my_SBM.add_edge("g2_6", "g3_9");
  my_SBM.add_edge("g2_6", "g3_12");
  my_SBM.add_edge("g2_6", "g3_13");
  my_SBM.add_edge("g2_6", "g3_15");
  my_SBM.add_edge("g2_7", "g2_8");
  my_SBM.add_edge("g2_7", "g2_9");
  my_SBM.add_edge("g2_7", "g2_10");
  my_SBM.add_edge("g2_7", "g2_12");
  my_SBM.add_edge("g2_7", "g2_13");
  my_SBM.add_edge("g2_7", "g2_14");
  my_SBM.add_edge("g2_7", "g2_15");
  my_SBM.add_edge("g2_7", "g3_1");
  my_SBM.add_edge("g2_7", "g3_2");
  my_SBM.add_edge("g2_7", "g3_3");
  my_SBM.add_edge("g2_7", "g3_4");
  my_SBM.add_edge("g2_7", "g3_6");
  my_SBM.add_edge("g2_7", "g3_7");
  my_SBM.add_edge("g2_7", "g3_8");
  my_SBM.add_edge("g2_7", "g3_9");
  my_SBM.add_edge("g2_7", "g3_10");
  my_SBM.add_edge("g2_7", "g3_11");
  my_SBM.add_edge("g2_7", "g3_12");
  my_SBM.add_edge("g2_7", "g3_13");
  my_SBM.add_edge("g2_7", "g3_14");
  my_SBM.add_edge("g2_7", "g3_15");
  my_SBM.add_edge("g2_8", "g2_9");
  my_SBM.add_edge("g2_8", "g2_10");
  my_SBM.add_edge("g2_8", "g2_11");
  my_SBM.add_edge("g2_8", "g2_12");
  my_SBM.add_edge("g2_8", "g2_15");
  my_SBM.add_edge("g2_8", "g3_2");
  my_SBM.add_edge("g2_8", "g3_3");
  my_SBM.add_edge("g2_8", "g3_5");
  my_SBM.add_edge("g2_8", "g3_6");
  my_SBM.add_edge("g2_8", "g3_7");
  my_SBM.add_edge("g2_8", "g3_8");
  my_SBM.add_edge("g2_8", "g3_9");
  my_SBM.add_edge("g2_8", "g3_10");
  my_SBM.add_edge("g2_8", "g3_11");
  my_SBM.add_edge("g2_8", "g3_12");
  my_SBM.add_edge("g2_8", "g3_13");
  my_SBM.add_edge("g2_8", "g3_14");
  my_SBM.add_edge("g2_9", "g2_10");
  my_SBM.add_edge("g2_9", "g2_11");
  my_SBM.add_edge("g2_9", "g2_12");
  my_SBM.add_edge("g2_9", "g2_13");
  my_SBM.add_edge("g2_9", "g2_14");
  my_SBM.add_edge("g2_9", "g2_15");
  my_SBM.add_edge("g2_9", "g3_1");
  my_SBM.add_edge("g2_9", "g3_2");
  my_SBM.add_edge("g2_9", "g3_3");
  my_SBM.add_edge("g2_9", "g3_5");
  my_SBM.add_edge("g2_9", "g3_6");
  my_SBM.add_edge("g2_9", "g3_7");
  my_SBM.add_edge("g2_9", "g3_9");
  my_SBM.add_edge("g2_9", "g3_10");
  my_SBM.add_edge("g2_9", "g3_11");
  my_SBM.add_edge("g2_9", "g3_12");
  my_SBM.add_edge("g2_9", "g3_13");
  my_SBM.add_edge("g2_9", "g3_14");
  my_SBM.add_edge("g2_9", "g3_15");
  my_SBM.add_edge("g2_10", "g2_11");
  my_SBM.add_edge("g2_10", "g2_12");
  my_SBM.add_edge("g2_10", "g2_13");
  my_SBM.add_edge("g2_10", "g2_14");
  my_SBM.add_edge("g2_10", "g3_1");
  my_SBM.add_edge("g2_10", "g3_2");
  my_SBM.add_edge("g2_10", "g3_5");
  my_SBM.add_edge("g2_10", "g3_6");
  my_SBM.add_edge("g2_10", "g3_7");
  my_SBM.add_edge("g2_10", "g3_9");
  my_SBM.add_edge("g2_10", "g3_10");
  my_SBM.add_edge("g2_10", "g3_11");
  my_SBM.add_edge("g2_10", "g3_12");
  my_SBM.add_edge("g2_10", "g3_13");
  my_SBM.add_edge("g2_10", "g3_14");
  my_SBM.add_edge("g2_10", "g3_15");
  my_SBM.add_edge("g2_11", "g2_12");
  my_SBM.add_edge("g2_11", "g2_14");
  my_SBM.add_edge("g2_11", "g3_1");
  my_SBM.add_edge("g2_11", "g3_2");
  my_SBM.add_edge("g2_11", "g3_3");
  my_SBM.add_edge("g2_11", "g3_4");
  my_SBM.add_edge("g2_11", "g3_5");
  my_SBM.add_edge("g2_11", "g3_6");
  my_SBM.add_edge("g2_11", "g3_7");
  my_SBM.add_edge("g2_11", "g3_9");
  my_SBM.add_edge("g2_11", "g3_10");
  my_SBM.add_edge("g2_11", "g3_11");
  my_SBM.add_edge("g2_11", "g3_12");
  my_SBM.add_edge("g2_11", "g3_13");
  my_SBM.add_edge("g2_11", "g3_14");
  my_SBM.add_edge("g2_12", "g2_13");
  my_SBM.add_edge("g2_12", "g2_14");
  my_SBM.add_edge("g2_12", "g3_1");
  my_SBM.add_edge("g2_12", "g3_3");
  my_SBM.add_edge("g2_12", "g3_4");
  my_SBM.add_edge("g2_12", "g3_5");
  my_SBM.add_edge("g2_12", "g3_6");
  my_SBM.add_edge("g2_12", "g3_7");
  my_SBM.add_edge("g2_12", "g3_8");
  my_SBM.add_edge("g2_12", "g3_9");
  my_SBM.add_edge("g2_12", "g3_10");
  my_SBM.add_edge("g2_12", "g3_11");
  my_SBM.add_edge("g2_12", "g3_12");
  my_SBM.add_edge("g2_12", "g3_13");
  my_SBM.add_edge("g2_12", "g3_14");
  my_SBM.add_edge("g2_12", "g3_15");
  my_SBM.add_edge("g2_13", "g3_1");
  my_SBM.add_edge("g2_13", "g3_2");
  my_SBM.add_edge("g2_13", "g3_3");
  my_SBM.add_edge("g2_13", "g3_4");
  my_SBM.add_edge("g2_13", "g3_5");
  my_SBM.add_edge("g2_13", "g3_6");
  my_SBM.add_edge("g2_13", "g3_7");
  my_SBM.add_edge("g2_13", "g3_8");
  my_SBM.add_edge("g2_13", "g3_9");
  my_SBM.add_edge("g2_13", "g3_10");
  my_SBM.add_edge("g2_13", "g3_11");
  my_SBM.add_edge("g2_13", "g3_12");
  my_SBM.add_edge("g2_13", "g3_13");
  my_SBM.add_edge("g2_13", "g3_14");
  my_SBM.add_edge("g2_13", "g3_15");
  my_SBM.add_edge("g2_14", "g3_1");
  my_SBM.add_edge("g2_14", "g3_2");
  my_SBM.add_edge("g2_14", "g3_3");
  my_SBM.add_edge("g2_14", "g3_4");
  my_SBM.add_edge("g2_14", "g3_5");
  my_SBM.add_edge("g2_14", "g3_6");
  my_SBM.add_edge("g2_14", "g3_7");
  my_SBM.add_edge("g2_14", "g3_8");
  my_SBM.add_edge("g2_14", "g3_9");
  my_SBM.add_edge("g2_14", "g3_10");
  my_SBM.add_edge("g2_14", "g3_11");
  my_SBM.add_edge("g2_14", "g3_13");
  my_SBM.add_edge("g2_14", "g3_14");
  my_SBM.add_edge("g2_14", "g3_15");
  my_SBM.add_edge("g2_15", "g3_2");
  my_SBM.add_edge("g2_15", "g3_3");
  my_SBM.add_edge("g2_15", "g3_4");
  my_SBM.add_edge("g2_15", "g3_5");
  my_SBM.add_edge("g2_15", "g3_6");
  my_SBM.add_edge("g2_15", "g3_8");
  my_SBM.add_edge("g2_15", "g3_9");
  my_SBM.add_edge("g2_15", "g3_10");
  my_SBM.add_edge("g2_15", "g3_11");
  my_SBM.add_edge("g2_15", "g3_12");
  my_SBM.add_edge("g2_15", "g3_13");
  my_SBM.add_edge("g2_15", "g3_14");
  my_SBM.add_edge("g3_1", "g3_5");
  my_SBM.add_edge("g3_1", "g3_8");
  my_SBM.add_edge("g3_1", "g3_10");
  my_SBM.add_edge("g3_1", "g3_11");
  my_SBM.add_edge("g3_1", "g3_14");
  my_SBM.add_edge("g3_2", "g3_3");
  my_SBM.add_edge("g3_2", "g3_5");
  my_SBM.add_edge("g3_2", "g3_6");
  my_SBM.add_edge("g3_2", "g3_7");
  my_SBM.add_edge("g3_2", "g3_9");
  my_SBM.add_edge("g3_2", "g3_13");
  my_SBM.add_edge("g3_2", "g3_14");
  my_SBM.add_edge("g3_3", "g3_4");
  my_SBM.add_edge("g3_3", "g3_7");
  my_SBM.add_edge("g3_3", "g3_8");
  my_SBM.add_edge("g3_3", "g3_12");
  my_SBM.add_edge("g3_3", "g3_13");
  my_SBM.add_edge("g3_4", "g3_5");
  my_SBM.add_edge("g3_4", "g3_6");
  my_SBM.add_edge("g3_4", "g3_8");
  my_SBM.add_edge("g3_4", "g3_9");
  my_SBM.add_edge("g3_4", "g3_10");
  my_SBM.add_edge("g3_4", "g3_11");
  my_SBM.add_edge("g3_4", "g3_12");
  my_SBM.add_edge("g3_4", "g3_13");
  my_SBM.add_edge("g3_4", "g3_15");
  my_SBM.add_edge("g3_5", "g3_6");
  my_SBM.add_edge("g3_5", "g3_8");
  my_SBM.add_edge("g3_5", "g3_12");
  my_SBM.add_edge("g3_6", "g3_8");
  my_SBM.add_edge("g3_6", "g3_11");
  my_SBM.add_edge("g3_6", "g3_12");
  my_SBM.add_edge("g3_6", "g3_13");
  my_SBM.add_edge("g3_6", "g3_15");
  my_SBM.add_edge("g3_7", "g3_12");
  my_SBM.add_edge("g3_7", "g3_15");
  my_SBM.add_edge("g3_8", "g3_9");
  my_SBM.add_edge("g3_8", "g3_10");
  my_SBM.add_edge("g3_8", "g3_11");
  my_SBM.add_edge("g3_8", "g3_15");
  my_SBM.add_edge("g3_9", "g3_13");
  my_SBM.add_edge("g3_9", "g3_14");
  my_SBM.add_edge("g3_10", "g3_11");
  my_SBM.add_edge("g3_10", "g3_12");
  my_SBM.add_edge("g3_10", "g3_13");
  my_SBM.add_edge("g3_10", "g3_15");
  my_SBM.add_edge("g3_11", "g3_13");
  my_SBM.add_edge("g3_11", "g3_15");
  my_SBM.add_edge("g3_12", "g3_14");
  my_SBM.add_edge("g3_13", "g3_15");

  return my_SBM;
}
