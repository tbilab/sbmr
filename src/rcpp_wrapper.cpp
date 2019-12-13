#include <Rcpp.h>
#include "SBM.h"

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

class Rcpp_SBM : public SBM
{
public:

  // Keeps track of user friendly string types and turns them
  // into c++ friendly integer types
  int current_type_int = 0;
  std::unordered_map<string, int> type_string_to_int;
  std::unordered_map<int, string> type_int_to_string;


  void add_node(const std::string id, const std::string type, const int level)
  {
    int node_int_type;

    // See if the node's type is in our list
    auto loc_of_int_type = type_string_to_int.find(type);

    if (loc_of_int_type == type_string_to_int.end())
    {
      // If its a new type, we need to add a new entry for this type to both maps
      type_string_to_int.emplace(type, current_type_int);
      type_int_to_string.emplace(current_type_int, type);

      // Set the value as newly designated integer
      node_int_type = current_type_int;

      // Iterate type integer keeper forward
      current_type_int++;
    }
    else
    {
      node_int_type = loc_of_int_type->second;
    }

    // Check to make sure that this node doesn't already exist in the network
    auto base_level = get_level(level);
    if (base_level->find(id) != base_level->end())
    {
      warning(id + " already exists in network\n");
    }
    else
    {
      // Add node to model
      SBM::add_node(id, node_int_type, level);
    }
  }

  NodePtr find_node_by_id(const std::string node_id, const int level)
  {
    try
    {
      return nodes.at(level)->at(node_id);
    }
    catch (...)
    {
      stop("Can't find node " + node_id + " at level " + std::to_string(level));
    }
  }

  void add_connection(const std::string node_a_id, const std::string node_b_id)
  {
    SBM::add_connection(find_node_by_id(node_a_id, 0),
                        find_node_by_id(node_b_id, 0));
  }

  DataFrame get_state()
  {
    // Grab state from class
    auto state = SBM::get_state();

    // Convert the type column to the string types
    std::vector<string> string_types;
    string_types.reserve(state.type.size());

    for (auto type_it = state.type.begin();
              type_it != state.type.end();
              type_it++)
    {
      // Convert int to string and push to vector
      string_types.push_back(type_int_to_string[*type_it]);
    }

    return DataFrame::create(
      _["id"] = state.id,
      _["parent"] = state.parent,
      _["type"] = string_types,
      _["level"] = state.level,
      _["stringsAsFactors"] = false);
  }

  void set_node_parent(const std::string child_id,
                       const std::string parent_id,
                       const int level = 0)
  {
    find_node_by_id(child_id, level)->set_parent(find_node_by_id(parent_id, level + 1));
  }

  void initialize_groups(const int num_groups, const int level)
  {
   Network::initialize_groups(num_groups, level);
  }

  double compute_entropy(const int level)
  {
    return SBM::compute_entropy(level);
  }

  int mcmc_sweep(int level, bool variable_num_groups)
  {
    // std::cout << "Running rcpp_SBM mcmc function" << std::endl;
    return SBM::mcmc_sweep(level, variable_num_groups);
  }

  List collapse_groups(const int node_level,
                       const int num_mcmc_steps,
                       int desired_num_groups,
                       const bool exhaustive)
  {
    // Book keep sigma value in case we change it with exhaustive mode
    const int old_sigma = SIGMA;

    if (exhaustive)
    {
      // Set Sigma value below 1 to insure a single group is merged each step
      SIGMA = 0.5;

      // Perform merge with single group per node type requested.
      desired_num_groups = node_type_counts.size();
    }


    std::cout << "Collapsing groups w/  " << std::to_string(desired_num_groups) << " desired groups" << std::endl;
    // Perform collapse
    auto collapse_results = SBM::collapse_groups(node_level, num_mcmc_steps, desired_num_groups);

    // Reset to old sigma value if needed
    if (exhaustive) {
      SIGMA = old_sigma;
    }

    List entropy_results;

    for (auto step = collapse_results.begin();
         step != collapse_results.end();
         step++)
    {
      entropy_results.push_back(
          List::create(
              _["entropy"] = step->entropy,
              _["state"] = state_to_df(step->state)));
    }

    return entropy_results;
  }

  void load_from_state(std::vector<string> id,
                       std::vector<string> parent,
                       std::vector<int> level,
                       std::vector<string> string_types)
  {
    // Convert the types from strings to integers
    // Convert the type column to the string types
    std::vector<int> int_types;
    int_types.reserve(string_types.size());

    for (auto type_it = string_types.begin();
         type_it != string_types.end();
         type_it++)
    {

      // Make sure that the requested type has been seen by the model already and
      // send message to R if it hasnt.

      auto loc_of_int_type = type_string_to_int.find(*type_it);
      if (loc_of_int_type == type_string_to_int.end())
      {
        stop((*type_it) + " not found in model");
      } else {
        // Convert string to int and push to vector
        int_types.push_back(loc_of_int_type->second);
      }

    }

    // Construct a state dump from vectors and
    // pass the constructed state to load_state function
    SBM::load_from_state(State_Dump(id, parent, level, int_types));
  }

  // Getters and setters for inhereted fields
  void set_beta(const double beta) { BETA = beta; }
  double get_beta() { return BETA; }

  void set_epsilon(const double eps) { EPS = eps; }
  double get_epsilon() { return EPS; }

  void set_sigma(const double sigma) { SIGMA = sigma; }
  double get_sigma() { return SIGMA; }

  void set_greedy(const bool greedy) { GREEDY = greedy; }
  bool get_greedy() { return GREEDY; }

  void set_n_checks_per_group(const int n) { N_CHECKS_PER_GROUP = n; }
  int get_n_checks_per_group() { return N_CHECKS_PER_GROUP; }
};

RCPP_MODULE(SBM)
{
  class_<Rcpp_SBM>("SBM")

      .constructor()

      .property("EPS",
                &Rcpp_SBM::get_epsilon, &Rcpp_SBM::set_epsilon,
                "Epsilon value for ergodicity")

      .property("BETA",
                &Rcpp_SBM::get_beta, &Rcpp_SBM::set_beta,
                "Beta value for MCMC acceptance probability")

      .property("GREEDY",
                &Rcpp_SBM::get_greedy, &Rcpp_SBM::set_greedy,
                "Perform merging and sweeps in greedy way?")

      .property("SIGMA",
                &Rcpp_SBM::get_sigma, &Rcpp_SBM::set_sigma,
                "Sigma value for determining rate of agglomerative merging")

      .property("N_CHECKS_PER_GROUP",
                &Rcpp_SBM::get_n_checks_per_group, &Rcpp_SBM::set_n_checks_per_group,
                "If not in greedy mode, how many options do we check per node for moves in agglomerative merging?")

      .method("add_node",
              &Rcpp_SBM::add_node,
              "Add a node to the network. Takes the node id (string), the node type (string), and the node level (int). Use level = 0 for data-level nodes."
              )
      .method("add_connection",
              &Rcpp_SBM::add_connection,
              "Connects two nodes in network (at level 0) by their ids (string).")
      .method("set_node_parent",
              &Rcpp_SBM::set_node_parent,
              "Sets the parent node (or group) for a given node. Takes child node's id (string), parent node's id (string), and the level of child node (int).")
      .method("initialize_groups",
             &Rcpp_SBM::initialize_groups,
             "Adds a desired number of groups and randomly assigns them for a given level. num_groups = -1 means every node gets their own group")
      .method("get_state",
              &Rcpp_SBM::get_state,
              "Exports the current state of the network as dataframe with each node as a row and columns for node id, parent id, node type, and node level.")
      .method("load_from_state",
              &Rcpp_SBM::load_from_state,
              "Takes model state export as given by SBM$get_state() and returns model to specified state. This is useful for resetting model before running various algorithms such as agglomerative merging.")
      .method("compute_entropy",
              &Rcpp_SBM::compute_entropy,
              "Computes the (degree-corrected) entropy for the network at the specified level (int).")
      .method("mcmc_sweep",
              &Rcpp_SBM::mcmc_sweep,
              "Runs a single MCMC sweep across all nodes at specified level. Each node is given a chance to move groups or stay in current group and all nodes are processed in random order. Takes the level that the sweep should take place on (int) and if new groups groups can be proposed and empty groups removed (boolean).")
      .method("collapse_groups",
              &Rcpp_SBM::collapse_groups,
              "Performs agglomerative merging on network, starting with each group has a single node down to one group per node type. Arguments are level to perform merge at (int) and number of MCMC steps to peform between each collapsing to equilibriate group. Returns list with entropy and model state at each merge.");
}

/*** R
sbm <- new(SBM)

sbm$add_node("a1", "a", 0)
sbm$add_node("a2", "a", 0)
sbm$add_node("a3", "a", 0)
sbm$add_node("b1", "b", 0)
sbm$add_node("b2", "b", 0)
sbm$add_node("b3", "b", 0)


sbm$get_state()

sbm$add_node("a11", "a", 1L)
sbm$add_node("a12", "a", 1L)
sbm$add_node("b11", "b", 1L)
sbm$add_node("b12", "b", 1L)

sbm$add_connection("a1", "b1")
sbm$add_connection("a1", "b2")
sbm$add_connection("a1", "b3")
sbm$add_connection("a2", "b3")
sbm$add_connection("a3", "b2")


sbm$set_node_parent("a1", "a11", 0)
sbm$set_node_parent("a2", "a11", 0)
sbm$set_node_parent("a3", "a12", 0)
sbm$set_node_parent("b1", "b11", 0)
sbm$set_node_parent("b2", "b11", 0)
sbm$set_node_parent("b3", "b12", 0)


# Set some model parameters
sbm$GREEDY <- TRUE
sbm$BETA <- 1.5
sbm$EPS <- 0.1
sbm$N_CHECKS_PER_GROUP <- 5

original_state <- sbm$get_state()

for(i in 1:10){
  entro_pre <- sbm$compute_entropy(0L)
  groups_moved <- sbm$mcmc_sweep(0L,FALSE)
  print(paste("started with entropy of", entro_pre, "and moved", groups_moved))
}

new_state <- sbm$get_state()

load_state(sbm, original_state)


# Bring me back to original state
# load_state(sbm, original_state)
#
# library(tidyverse)
# merge_results <- sbm$collapse_groups(0, 15)
# load_state(sbm, original_state)
# new_state <- sbm$get_state()
#
# sort_state <- . %>%  arrange(level, type, id)
# original_state %>% sort_state()
# new_state %>% sort_state()

#
#
# desired_state <- init_results[[1]]$state
#
# sbm$get_state()
# load_state(sbm, original_state)
# sbm$get_state()
# sbm$mcmc_sweep(0L,FALSE)
# sbm$mcmc_sweep(0L,FALSE)
# sbm$compute_entropy(0L)


*/
