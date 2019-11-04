// [[Rcpp::plugins(cpp11)]]

#include <Rcpp.h>

#include "helpers.cpp"
#include "Node.h"
#include "SBM.h"

using namespace Rcpp;

// [[Rcpp::export]]
List setup_SBM(){
  SBM my_SBM;
  
  // Add some nodes to SBM
  my_SBM.get_node_by_id("n1", 0);
  my_SBM.get_node_by_id("n2", 0);
  my_SBM.get_node_by_id("n3", 0);
  my_SBM.get_node_by_id("m1", 1);
  my_SBM.get_node_by_id("m2", 1);
  my_SBM.get_node_by_id("m3", 1);
  my_SBM.get_node_by_id("m4", 1);
  
  // Create a group node
  my_SBM.create_group_node(0, 1);
  my_SBM.create_group_node(1, 1);

  return List::create(
    _["num_nodes"]            = my_SBM.nodes[0].size(),
    _["level 0"]              = print_node_ids(*my_SBM.get_node_level(0)),
    _["level 1"]              = print_node_ids(*my_SBM.get_node_level(1)),
    _["nodes of first type"]  = print_node_ids(my_SBM.get_nodes_of_type_at_level(0,0)),
    _["nodes of second type"] = print_node_ids(my_SBM.get_nodes_of_type_at_level(1,0)),
    _["num levels"]           = my_SBM.nodes.size()
  );
}

/*** R
setup_SBM()
*/
