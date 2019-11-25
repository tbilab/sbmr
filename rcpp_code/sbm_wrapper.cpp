#include <Rcpp.h>
#include "../SBM.h"

using namespace Rcpp;

class Rcpp_SBM: public SBM {
public:
  // Rcpp_SBM(int seed):sampler(seed) {};
  void add_node_rcpp(
      std::string id,
      int type,
      int level)
  {
    add_node(id, type, level);
  }
  
  void add_connections_rcpp(std::string node_a_id, std::string node_b_id)
  {
    add_connection(node_a_id, node_b_id);
  }

  DataFrame get_state_rcpp()
  {
    // Grab state dump struct
    State_Dump state = get_state();

    // Create and return dump of state as dataframe
    return DataFrame::create(
        _("id") = state.id,
        _("parent") = state.parent,
        _("type") = state.type,
        _("level") = state.level);
  }
};



RCPP_MODULE(sbm_module)
{
  class_<Rcpp_SBM>("Rcpp_SBM")
  
  .constructor()
  
  .method("add_node_rcpp", &Rcpp_SBM::add_node_rcpp)
  .method("add_connections_rcpp", &Rcpp_SBM::add_connections_rcpp)
  .method("get_state_rcpp", &Rcpp_SBM::get_state_rcpp)
  ;
}

/*** R
sbm <- new(Rcpp_SBM)

sbm$add_node_rcpp("a1", 0L, 0L)
sbm$add_node_rcpp("a2", 0L, 0L)
sbm$add_node_rcpp("a3", 0L, 0L)
sbm$add_node_rcpp("b1", 1L, 0L)
sbm$add_node_rcpp("b2", 1L, 0L)
sbm$add_node_rcpp("b3", 1L, 0L)

sbm$add_connections_rcpp("a1", "b1")
sbm$add_connections_rcpp("a1", "b2")
sbm$add_connections_rcpp("a1", "b3")
sbm$add_connections_rcpp("a2", "b3")
sbm$add_connections_rcpp("a3", "b2")




sbm$get_state_rcpp()

*/