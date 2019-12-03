#include <Rcpp.h>
#include "../SBM.h"

using namespace Rcpp;

inline DataFrame state_to_df(State_Dump state)
{
    // Create and return dump of state as dataframe
    return DataFrame::create(
        _["id"] = state.id,
        _["parent"] = state.parent,
        _["type"] = state.type,
        _["level"] = state.level,
        _["stringsAsFactors"] = false);
}

class Rcpp_SBM: public SBM {
public:
  // Rcpp_SBM(int seed):sampler(seed) {};
  void add_node_rcpp(
      const std::string id,
      const int type,
      const int level)
  {
    add_node(id, type, level);
  }
  
  void add_connection_rcpp(const std::string node_a_id, const std::string node_b_id)
  {
    add_connection(node_a_id, node_b_id);
  }

  DataFrame get_state_rcpp()
  {
    // Grab state dump struct
    State_Dump state = get_state();

    // Create and return dump of state as dataframe
    return state_to_df(state);
  }

  void set_node_parent_rcpp(
      const std::string child_id, 
      const std::string parent_id, 
      const int level = 0,
      const bool building_network = false)
  {
    NodePtr child_node = get_node_by_id(child_id, level);
    NodePtr parent_node = get_node_by_id(parent_id, level + 1);
    
    // If we try and set a parent for a node when the network is not fully built out 
    // the default set_node_parent method will attempt to find the edge counts to non-existant
    // parents at a given level which will result in errors. 
    if (building_network)
    {
      child_node->set_parent(parent_node);
    }
    else 
    {
      set_node_parent(child_node, parent_node);
    }
  }

  double compute_entropy_rcpp(const int level)
  {
    return compute_entropy(level);
  }

  int mcmc_sweep_rcpp(
      const int level,
      const bool variable_num_groups)
  {
    return mcmc_sweep(level, variable_num_groups);
  }
  
  // Getters and setters for inhereted fields
  void set_beta(const double beta) {BETA = beta;}
  double get_beta() {return BETA;}
  
  void set_epsilon(const double eps) {EPS = eps;}
  double get_epsilon() {return EPS;}
  
  void set_sigma(const double sigma) {SIGMA = sigma;}
  double get_sigma() {return SIGMA;}
  
  void set_greedy(const bool greedy) {GREEDY = greedy;}
  bool get_greedy() {return GREEDY;}
  
  void set_n_checks_per_group(const int n) {N_CHECKS_PER_GROUP = n;}
  int get_n_checks_per_group() {return N_CHECKS_PER_GROUP;}
  
  
  List collapse_groups_rcpp(
      const int node_level,
      const int num_mcmc_steps)
  {

    auto init_results = collapse_groups(node_level,num_mcmc_steps);

    List entropy_results;

    for (auto step = init_results.begin();
              step != init_results.end();
              step++)
    {

      entropy_results.push_back(
        List::create(
          _["entropy"] = step->entropy,
          _["state"] = state_to_df(step->state)
        ));
    }

    return entropy_results;
  }

  void load_from_state_rcpp(
      std::vector<string> id,
      std::vector<string> parent,
      std::vector<int> level,
      std::vector<int> type)
  {
    // Construct a state dump from vectors and
    // pass the constructed state to load_state function
    load_from_state(State_Dump(id, parent, level, type));
  }
};

RCPP_MODULE(sbm_module)
{
  class_<Rcpp_SBM>("Rcpp_SBM")
  
  .constructor()
  
  .property("EPS", &Rcpp_SBM::get_epsilon,  &Rcpp_SBM::set_epsilon, "Epsilon value for ergodicity" )
  .property("BETA", &Rcpp_SBM::get_beta,  &Rcpp_SBM::set_beta, "Beta value for MCMC acceptance probability" )
  .property("GREEDY", &Rcpp_SBM::get_greedy,  &Rcpp_SBM::set_greedy, "Perform merging and sweeps in greedy way?" )
  .property("SIGMA", &Rcpp_SBM::get_sigma,  &Rcpp_SBM::set_sigma, "Sigma value for determining rate of agglomerative merging" )
  .property("N_CHECKS_PER_GROUP", &Rcpp_SBM::get_n_checks_per_group,  &Rcpp_SBM::set_n_checks_per_group, "If not in greedy mode, how many options do we check per node for moves in agglomerative merging?" )
  
  .method("add_node_rcpp", &Rcpp_SBM::add_node_rcpp)
  .method("add_connection_rcpp", &Rcpp_SBM::add_connection_rcpp)
  .method("get_state_rcpp", &Rcpp_SBM::get_state_rcpp)
  .method("load_from_state_rcpp", &Rcpp_SBM::load_from_state_rcpp)
  .method("set_node_parent_rcpp", &Rcpp_SBM::set_node_parent_rcpp)
  .method("compute_entropy_rcpp", &Rcpp_SBM::compute_entropy_rcpp)
  .method("mcmc_sweep_rcpp", &Rcpp_SBM::mcmc_sweep_rcpp)
  .method("collapse_groups_rcpp", &Rcpp_SBM::collapse_groups_rcpp)
  .method("set_epsilon", &Rcpp_SBM::set_epsilon)
  .method("set_greedy", &Rcpp_SBM::set_greedy)
  .method("set_beta", &Rcpp_SBM::set_beta)
  .method("set_sigma", &Rcpp_SBM::set_sigma)
  .method("set_n_checks_per_group", &Rcpp_SBM::set_n_checks_per_group)
  
  
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

sbm$add_node_rcpp("a11", 0L, 1L)
sbm$add_node_rcpp("a12", 0L, 1L)
sbm$add_node_rcpp("b11", 1L, 1L)
sbm$add_node_rcpp("b12", 1L, 1L)


sbm$add_connection_rcpp("a1", "b1")
sbm$add_connection_rcpp("a1", "b2")
sbm$add_connection_rcpp("a1", "b3")
sbm$add_connection_rcpp("a2", "b3")
sbm$add_connection_rcpp("a3", "b2")


sbm$set_node_parent_rcpp("a1", "a11", 0, TRUE)
sbm$set_node_parent_rcpp("a2", "a11", 0, TRUE)
sbm$set_node_parent_rcpp("a3", "a12", 0, TRUE)
sbm$set_node_parent_rcpp("b1", "b11", 0, TRUE)
sbm$set_node_parent_rcpp("b2", "b11", 0, TRUE)
sbm$set_node_parent_rcpp("b3", "b12", 0, TRUE)


load_state <- function(sbm, state_dump){
  sbm$load_from_state_rcpp(
    state_dump$id, 
    state_dump$parent, 
    state_dump$level, 
    state_dump$type)
}

original_state <- sbm$get_state_rcpp()

sbm$compute_entropy_rcpp(0L)

# Set some model parameters
sbm$GREEDY <- TRUE
sbm$BETA <- 1.5
sbm$EPS <- 0.1
sbm$N_CHECKS_PER_GROUP <- 5

init_results <- sbm$collapse_groups_rcpp(0, 15)

# desired_state <- init_results[[1]]$state
# sbm$load_from_state_rcpp(desired_state$id, desired_state$parent, desired_state$level, desired_state$type)


# sbm$mcmc_sweep_rcpp(0,FALSE,0.1,1.5)
# sbm$mcmc_sweep_rcpp(0L,FALSE)
# sbm$compute_entropy_rcpp(0L)


*/