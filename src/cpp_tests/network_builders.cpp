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

//   my_SBM.add_connection("a1", "b1");
//   my_SBM.add_connection("a2", "b1");
//   my_SBM.add_connection("a3", "b1");
//   my_SBM.add_connection("a4", "b1");
//   my_SBM.add_connection("a5", "b1");
//   my_SBM.add_connection("a1", "b2");
//   my_SBM.add_connection("a2", "b2");
//   my_SBM.add_connection("a3", "b2");
//   my_SBM.add_connection("a4", "b2");
//   my_SBM.add_connection("a5", "b2");
//   my_SBM.add_connection("a10", "b2");
//   my_SBM.add_connection("a11", "b2");
//   my_SBM.add_connection("a12", "b2");
//   my_SBM.add_connection("a13", "b2");
//   my_SBM.add_connection("a14", "b2");
//   my_SBM.add_connection("a6", "b3");
//   my_SBM.add_connection("a7", "b3");
//   my_SBM.add_connection("a8", "b3");
//   my_SBM.add_connection("a9", "b3");
//   my_SBM.add_connection("a10", "b3");
//   my_SBM.add_connection("a11", "b3");
//   my_SBM.add_connection("a12", "b3");
//   my_SBM.add_connection("a13", "b3");
//   my_SBM.add_connection("a14", "b3");
//   my_SBM.add_connection("a10", "b4");
//   my_SBM.add_connection("a11", "b4");
//   my_SBM.add_connection("a12", "b4");
//   my_SBM.add_connection("a13", "b4");
//   my_SBM.add_connection("a14", "b4");

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

  // Make 2 type 0/a blocks
  NodePtr a11 = my_SBM.add_node("a11", 0, 1);
  NodePtr a12 = my_SBM.add_node("a12", 0, 1);
  NodePtr a13 = my_SBM.add_node("a13", 0, 1);
  NodePtr b11 = my_SBM.add_node("b11", 1, 1);
  NodePtr b12 = my_SBM.add_node("b12", 1, 1);
  NodePtr b13 = my_SBM.add_node("b13", 1, 1);

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


SBM build_simulated_SBM()
{
  // This function implements a network built using the bisbmsim package
  // produced for the TBI lab and is built using the R script at
  // /tools/simulate_test_network.R
  SBM my_SBM;

  my_SBM.add_node("a4", 1);
  my_SBM.add_node("a5", 1);
  my_SBM.add_node("a7", 1);
  my_SBM.add_node("a10", 1);
  my_SBM.add_node("a13", 1);
  my_SBM.add_node("a22", 1);
  my_SBM.add_node("a23", 1);
  my_SBM.add_node("a24", 1);
  my_SBM.add_node("a1", 1);
  my_SBM.add_node("a2", 1);
  my_SBM.add_node("a3", 1);
  my_SBM.add_node("a6", 1);
  my_SBM.add_node("a21", 1);
  my_SBM.add_node("a25", 1);
  my_SBM.add_node("a8", 1);
  my_SBM.add_node("a9", 1);
  my_SBM.add_node("a11", 1);
  my_SBM.add_node("a12", 1);
  my_SBM.add_node("a14", 1);
  my_SBM.add_node("a15", 1);
  my_SBM.add_node("a16", 1);
  my_SBM.add_node("a17", 1);
  my_SBM.add_node("a20", 1);
  my_SBM.add_node("a18", 1);
  my_SBM.add_node("a19", 1);
  my_SBM.add_node("b1", 0);
  my_SBM.add_node("b2", 0);
  my_SBM.add_node("b3", 0);
  my_SBM.add_node("b4", 0);
  my_SBM.add_node("b5", 0);
  my_SBM.add_node("b6", 0);
  my_SBM.add_node("b7", 0);
  my_SBM.add_node("b8", 0);
  my_SBM.add_node("b9", 0);
  my_SBM.add_node("b10", 0);
  my_SBM.add_node("b11", 0);
  my_SBM.add_node("b12", 0);
  my_SBM.add_node("b13", 0);
  my_SBM.add_node("b14", 0);
  my_SBM.add_node("b15", 0);
  my_SBM.add_node("b16", 0);
  my_SBM.add_node("b17", 0);
  my_SBM.add_node("b18", 0);
  my_SBM.add_node("b19", 0);
  my_SBM.add_node("b20", 0);
  my_SBM.add_node("b21", 0);
  my_SBM.add_node("b22", 0);
  my_SBM.add_node("b23", 0);
  my_SBM.add_node("b24", 0);
  my_SBM.add_node("b25", 0);

  my_SBM.add_connection("a4", "b1");
  my_SBM.add_connection("a5", "b1");
  my_SBM.add_connection("a7", "b1");
  my_SBM.add_connection("a10", "b1");
  my_SBM.add_connection("a13", "b1");
  my_SBM.add_connection("a22", "b1");
  my_SBM.add_connection("a23", "b1");
  my_SBM.add_connection("a24", "b1");
  my_SBM.add_connection("a1", "b2");
  my_SBM.add_connection("a2", "b2");
  my_SBM.add_connection("a3", "b2");
  my_SBM.add_connection("a6", "b2");
  my_SBM.add_connection("a21", "b2");
  my_SBM.add_connection("a25", "b2");
  my_SBM.add_connection("a1", "b3");
  my_SBM.add_connection("a5", "b3");
  my_SBM.add_connection("a6", "b3");
  my_SBM.add_connection("a8", "b3");
  my_SBM.add_connection("a9", "b3");
  my_SBM.add_connection("a11", "b3");
  my_SBM.add_connection("a12", "b3");
  my_SBM.add_connection("a14", "b3");
  my_SBM.add_connection("a15", "b3");
  my_SBM.add_connection("a16", "b3");
  my_SBM.add_connection("a17", "b3");
  my_SBM.add_connection("a20", "b3");
  my_SBM.add_connection("a23", "b3");
  my_SBM.add_connection("a25", "b3");
  my_SBM.add_connection("a1", "b4");
  my_SBM.add_connection("a3", "b4");
  my_SBM.add_connection("a9", "b4");
  my_SBM.add_connection("a10", "b4");
  my_SBM.add_connection("a13", "b4");
  my_SBM.add_connection("a18", "b4");
  my_SBM.add_connection("a23", "b4");
  my_SBM.add_connection("a1", "b5");
  my_SBM.add_connection("a3", "b5");
  my_SBM.add_connection("a8", "b5");
  my_SBM.add_connection("a9", "b5");
  my_SBM.add_connection("a12", "b5");
  my_SBM.add_connection("a13", "b5");
  my_SBM.add_connection("a18", "b5");
  my_SBM.add_connection("a19", "b5");
  my_SBM.add_connection("a20", "b5");
  my_SBM.add_connection("a25", "b5");
  my_SBM.add_connection("a1", "b6");
  my_SBM.add_connection("a6", "b6");
  my_SBM.add_connection("a7", "b6");
  my_SBM.add_connection("a9", "b6");
  my_SBM.add_connection("a11", "b6");
  my_SBM.add_connection("a12", "b6");
  my_SBM.add_connection("a16", "b6");
  my_SBM.add_connection("a19", "b6");
  my_SBM.add_connection("a20", "b6");
  my_SBM.add_connection("a21", "b6");
  my_SBM.add_connection("a22", "b6");
  my_SBM.add_connection("a23", "b6");
  my_SBM.add_connection("a24", "b6");
  my_SBM.add_connection("a1", "b7");
  my_SBM.add_connection("a4", "b7");
  my_SBM.add_connection("a8", "b7");
  my_SBM.add_connection("a12", "b7");
  my_SBM.add_connection("a13", "b7");
  my_SBM.add_connection("a15", "b7");
  my_SBM.add_connection("a16", "b7");
  my_SBM.add_connection("a17", "b7");
  my_SBM.add_connection("a19", "b7");
  my_SBM.add_connection("a24", "b7");
  my_SBM.add_connection("a25", "b7");
  my_SBM.add_connection("a2", "b8");
  my_SBM.add_connection("a5", "b8");
  my_SBM.add_connection("a6", "b8");
  my_SBM.add_connection("a9", "b8");
  my_SBM.add_connection("a10", "b8");
  my_SBM.add_connection("a13", "b8");
  my_SBM.add_connection("a16", "b8");
  my_SBM.add_connection("a21", "b8");
  my_SBM.add_connection("a22", "b8");
  my_SBM.add_connection("a1", "b9");
  my_SBM.add_connection("a2", "b9");
  my_SBM.add_connection("a4", "b9");
  my_SBM.add_connection("a11", "b9");
  my_SBM.add_connection("a14", "b9");
  my_SBM.add_connection("a16", "b9");
  my_SBM.add_connection("a17", "b9");
  my_SBM.add_connection("a21", "b9");
  my_SBM.add_connection("a24", "b9");
  my_SBM.add_connection("a25", "b9");
  my_SBM.add_connection("a1", "b10");
  my_SBM.add_connection("a2", "b10");
  my_SBM.add_connection("a11", "b10");
  my_SBM.add_connection("a13", "b10");
  my_SBM.add_connection("a19", "b10");
  my_SBM.add_connection("a25", "b10");
  my_SBM.add_connection("a1", "b11");
  my_SBM.add_connection("a7", "b11");
  my_SBM.add_connection("a8", "b11");
  my_SBM.add_connection("a13", "b11");
  my_SBM.add_connection("a18", "b11");
  my_SBM.add_connection("a21", "b11");
  my_SBM.add_connection("a1", "b12");
  my_SBM.add_connection("a6", "b12");
  my_SBM.add_connection("a8", "b12");
  my_SBM.add_connection("a17", "b12");
  my_SBM.add_connection("a20", "b12");
  my_SBM.add_connection("a25", "b12");
  my_SBM.add_connection("a1", "b13");
  my_SBM.add_connection("a3", "b13");
  my_SBM.add_connection("a4", "b13");
  my_SBM.add_connection("a9", "b13");
  my_SBM.add_connection("a13", "b13");
  my_SBM.add_connection("a15", "b13");
  my_SBM.add_connection("a18", "b13");
  my_SBM.add_connection("a19", "b13");
  my_SBM.add_connection("a20", "b13");
  my_SBM.add_connection("a22", "b13");
  my_SBM.add_connection("a25", "b13");
  my_SBM.add_connection("a2", "b14");
  my_SBM.add_connection("a3", "b14");
  my_SBM.add_connection("a5", "b14");
  my_SBM.add_connection("a7", "b14");
  my_SBM.add_connection("a8", "b14");
  my_SBM.add_connection("a9", "b14");
  my_SBM.add_connection("a12", "b14");
  my_SBM.add_connection("a13", "b14");
  my_SBM.add_connection("a16", "b14");
  my_SBM.add_connection("a18", "b14");
  my_SBM.add_connection("a22", "b14");
  my_SBM.add_connection("a23", "b14");
  my_SBM.add_connection("a24", "b14");
  my_SBM.add_connection("a1", "b15");
  my_SBM.add_connection("a5", "b15");
  my_SBM.add_connection("a12", "b15");
  my_SBM.add_connection("a13", "b15");
  my_SBM.add_connection("a14", "b15");
  my_SBM.add_connection("a18", "b15");
  my_SBM.add_connection("a20", "b15");
  my_SBM.add_connection("a21", "b15");
  my_SBM.add_connection("a23", "b15");
  my_SBM.add_connection("a1", "b16");
  my_SBM.add_connection("a4", "b16");
  my_SBM.add_connection("a10", "b16");
  my_SBM.add_connection("a12", "b16");
  my_SBM.add_connection("a13", "b16");
  my_SBM.add_connection("a14", "b16");
  my_SBM.add_connection("a19", "b16");
  my_SBM.add_connection("a22", "b16");
  my_SBM.add_connection("a1", "b17");
  my_SBM.add_connection("a3", "b17");
  my_SBM.add_connection("a8", "b17");
  my_SBM.add_connection("a9", "b17");
  my_SBM.add_connection("a10", "b17");
  my_SBM.add_connection("a12", "b17");
  my_SBM.add_connection("a17", "b17");
  my_SBM.add_connection("a18", "b17");
  my_SBM.add_connection("a19", "b17");
  my_SBM.add_connection("a21", "b17");
  my_SBM.add_connection("a2", "b18");
  my_SBM.add_connection("a6", "b18");
  my_SBM.add_connection("a8", "b18");
  my_SBM.add_connection("a11", "b18");
  my_SBM.add_connection("a12", "b18");
  my_SBM.add_connection("a13", "b18");
  my_SBM.add_connection("a14", "b18");
  my_SBM.add_connection("a15", "b18");
  my_SBM.add_connection("a16", "b18");
  my_SBM.add_connection("a17", "b18");
  my_SBM.add_connection("a18", "b18");
  my_SBM.add_connection("a20", "b18");
  my_SBM.add_connection("a21", "b18");
  my_SBM.add_connection("a23", "b18");
  my_SBM.add_connection("a24", "b18");
  my_SBM.add_connection("a1", "b19");
  my_SBM.add_connection("a2", "b19");
  my_SBM.add_connection("a4", "b19");
  my_SBM.add_connection("a5", "b19");
  my_SBM.add_connection("a7", "b19");
  my_SBM.add_connection("a13", "b19");
  my_SBM.add_connection("a20", "b19");
  my_SBM.add_connection("a21", "b19");
  my_SBM.add_connection("a25", "b19");
  my_SBM.add_connection("a1", "b20");
  my_SBM.add_connection("a4", "b20");
  my_SBM.add_connection("a6", "b20");
  my_SBM.add_connection("a9", "b20");
  my_SBM.add_connection("a10", "b20");
  my_SBM.add_connection("a12", "b20");
  my_SBM.add_connection("a15", "b20");
  my_SBM.add_connection("a18", "b20");
  my_SBM.add_connection("a19", "b20");
  my_SBM.add_connection("a24", "b20");
  my_SBM.add_connection("a2", "b21");
  my_SBM.add_connection("a3", "b21");
  my_SBM.add_connection("a6", "b21");
  my_SBM.add_connection("a8", "b21");
  my_SBM.add_connection("a10", "b21");
  my_SBM.add_connection("a11", "b21");
  my_SBM.add_connection("a14", "b21");
  my_SBM.add_connection("a16", "b21");
  my_SBM.add_connection("a17", "b21");
  my_SBM.add_connection("a18", "b21");
  my_SBM.add_connection("a20", "b21");
  my_SBM.add_connection("a21", "b21");
  my_SBM.add_connection("a22", "b21");
  my_SBM.add_connection("a23", "b21");
  my_SBM.add_connection("a3", "b22");
  my_SBM.add_connection("a4", "b22");
  my_SBM.add_connection("a21", "b22");
  my_SBM.add_connection("a23", "b22");
  my_SBM.add_connection("a25", "b22");
  my_SBM.add_connection("a3", "b23");
  my_SBM.add_connection("a5", "b23");
  my_SBM.add_connection("a7", "b23");
  my_SBM.add_connection("a8", "b23");
  my_SBM.add_connection("a9", "b23");
  my_SBM.add_connection("a11", "b23");
  my_SBM.add_connection("a13", "b23");
  my_SBM.add_connection("a15", "b23");
  my_SBM.add_connection("a17", "b23");
  my_SBM.add_connection("a19", "b23");
  my_SBM.add_connection("a20", "b23");
  my_SBM.add_connection("a22", "b23");
  my_SBM.add_connection("a1", "b24");
  my_SBM.add_connection("a5", "b24");
  my_SBM.add_connection("a8", "b24");
  my_SBM.add_connection("a11", "b24");
  my_SBM.add_connection("a12", "b24");
  my_SBM.add_connection("a13", "b24");
  my_SBM.add_connection("a14", "b24");
  my_SBM.add_connection("a17", "b24");
  my_SBM.add_connection("a20", "b24");
  my_SBM.add_connection("a21", "b24");
  my_SBM.add_connection("a24", "b24");
  my_SBM.add_connection("a1", "b25");
  my_SBM.add_connection("a3", "b25");
  my_SBM.add_connection("a4", "b25");
  my_SBM.add_connection("a5", "b25");
  my_SBM.add_connection("a7", "b25");
  my_SBM.add_connection("a10", "b25");
  my_SBM.add_connection("a13", "b25");
  my_SBM.add_connection("a15", "b25");
  my_SBM.add_connection("a16", "b25");
  my_SBM.add_connection("a19", "b25");
  my_SBM.add_connection("a22", "b25");
  my_SBM.add_connection("a24", "b25");
  my_SBM.add_connection("a25", "b25");

  return my_SBM;
}