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
      std::string id,
      int type,
      int level)
  {
    add_node(id, type, level);
  }
  
  void add_connection_rcpp(std::string node_a_id, std::string node_b_id)
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

  void set_node_parent(std::string child_id, std::string parent_id, int level = 0)
  {
    NodePtr child_node = get_node_by_id(child_id, level);
    NodePtr parent_node = get_node_by_id(parent_id, level + 1);

    child_node->set_parent(parent_node);
  }

  double compute_entropy_rcpp(int level)
  {
    return compute_entropy(level);
  }

  int mcmc_sweep_rcpp(
      int level,
      bool variable_num_groups,
      double eps,
      double beta)
  {
    return mcmc_sweep(level, variable_num_groups, eps, beta);
  }

  List collapse_groups_rcpp(
      int node_level,
      int num_mcmc_steps,
      bool greedy,
      int n_checks_per_group,
      double beta,
      double epsilon)
  {
    auto init_results = collapse_groups(
        node_level,
        num_mcmc_steps,
        greedy,
        n_checks_per_group,
        beta,
        epsilon);

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
  
  .method("add_node_rcpp", &Rcpp_SBM::add_node_rcpp)
  .method("add_connection_rcpp", &Rcpp_SBM::add_connection_rcpp)
  .method("get_state_rcpp", &Rcpp_SBM::get_state_rcpp)
  .method("load_from_state_rcpp", &Rcpp_SBM::load_from_state_rcpp)
  .method("set_node_parent", &Rcpp_SBM::set_node_parent)
  .method("compute_entropy_rcpp", &Rcpp_SBM::compute_entropy_rcpp)
  .method("mcmc_sweep_rcpp", &Rcpp_SBM::mcmc_sweep_rcpp)
  .method("collapse_groups_rcpp", &Rcpp_SBM::collapse_groups_rcpp)
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

sbm$set_node_parent("a1", "a11", 0)
sbm$set_node_parent("a2", "a11", 0)
sbm$set_node_parent("a3", "a12", 0)
sbm$set_node_parent("b1", "b11", 0)
sbm$set_node_parent("b2", "b11", 0)
sbm$set_node_parent("b3", "b12", 0)


sbm$get_state_rcpp()

sbm$compute_entropy_rcpp(0L)

init_results <- sbm$collapse_groups_rcpp(0, 15, TRUE, 5, 1.5, 0.1)

desired_state <- init_results[[1]]$state
sbm$load_from_state_rcpp(desired_state$id, desired_state$parent, desired_state$level, desired_state$type)



# sbm$mcmc_sweep_rcpp(0,FALSE,0.1,1.5)
# sbm$compute_entropy_rcpp(0L)


*/