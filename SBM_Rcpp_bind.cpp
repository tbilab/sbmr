// [[Rcpp::plugins(cpp11)]]

#include <Rcpp.h>

#include <map>

#include "helpers.h"
#include "Node.h"
#include "SBM.h"

using namespace Rcpp;

// [[Rcpp::export]]
List setup_SBM(){
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
  //my_SBM.create_group_node(0, 1);
  //my_SBM.create_group_node(1, 1);

  return List::create(
    _["num_nodes"]            = my_SBM.nodes[0].size(),
    _["level 0"]              = print_node_ids(my_SBM.nodes[0]),
    _["level 1"]              = print_node_ids(my_SBM.nodes[1]),
    _["nodes of first type"]  = print_node_ids(my_SBM.get_nodes_of_type_at_level(0,0)),
    _["nodes of second type"] = print_node_ids(my_SBM.get_nodes_of_type_at_level(1,0)),
    _["num levels"]           = my_SBM.nodes.size()
  );
}

/*** R
setup_SBM()
*/
