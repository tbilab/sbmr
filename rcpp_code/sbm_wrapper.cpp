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
};



RCPP_MODULE(sbm_module)
{
  class_<Rcpp_SBM>("Rcpp_SBM")
  
  .constructor()
  
  .method("add_node_rcpp", &Rcpp_SBM::add_node_rcpp);
  
  ;
}

/*** R
sbm <- new(Rcpp_SBM)

*/