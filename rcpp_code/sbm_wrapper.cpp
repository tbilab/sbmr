#include <Rcpp.h>
#include "../SBM.h"

using namespace Rcpp;

class Rcpp_SBM: public SBM {
public:
  Rcpp_SBM(){};
  void add_node_rcpp(
      std::string id,
      int type,
      int level)
  {
    add_node(id, type, level);
  }
};


class Uniform
{
public:
  Uniform(double min_, double max_) : min(min_), max(max_) {}
  NumericVector draw(int n) const
  {
    RNGScope scope;
    return runif(n, min, max);
  }
  double min, max;
};

double uniformRange(Uniform *w)
{
  return w->max - w->min;
}

RCPP_MODULE(unif_module)
{
  class_<Uniform>("Uniform")
  .constructor<double, double>()
  .field("min", &Uniform::min)
  .field("max", &Uniform::max)
  .method("draw", &Uniform::draw)
  .method("range", &uniformRange);
}


RCPP_MODULE(sbm_module)
{
  class_<Rcpp_SBM>("Rcpp_SBM")
  
  .constructor()
  
  .method("add_node_rcpp", &Rcpp_SBM::add_node_rcpp);
  
  ;
}

/*** R
u <- new(Uniform, 0, 10) 
u$draw(10L)
u$range()


sbm <- new(Rcpp_SBM, 42)

*/