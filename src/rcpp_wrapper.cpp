#include "SBM.h"

#include <Rcpp.h>

using namespace Rcpp;

class Rcpp_SBM : public SBM {
  public:
  // Keeps track of user friendly string types and turns them
  // into c++ friendly integer types
  int                             current_type_int = 0;
  std::unordered_map<string, int> type_string_to_int;
  std::unordered_map<int, string> type_int_to_string;


  // Add allowed edge pairs to the object. Overwrites previous work if it was there
  void add_allowed_pairs(const std::vector<std::string> from_type,
                         const std::vector<std::string> to_type)
  {
    // Clear old allowed pairs (if they exist)
    edge_type_pairs.clear();

    // Convert the types to the model-language integer types
    const std::vector<int> from_type_ints = type_to_int(from_type);
    const std::vector<int> to_type_ints   = type_to_int(to_type);

    // Add pairs to network map of allowed pairs
    const int num_pairs = from_type.size();
    for (int i = 0; i < num_pairs; i++) {
      add_allowed_node_type_combos(from_type_ints[i], to_type_ints[i]);
    }

    // Let object know that we're working with specified types now.
    specified_allowed_edges = true;
  }

  void add_node(const std::string id, const std::string type, const int level)
  {
    int node_int_type;

    // See if the node's type is in our list
    const auto loc_of_int_type = type_string_to_int.find(type);

    if (loc_of_int_type == type_string_to_int.end()) {
      // If its a new type, we need to add a new entry for this type to both maps
      type_string_to_int.emplace(type, current_type_int);
      type_int_to_string.emplace(current_type_int, type);

      // Set the value as newly designated integer
      node_int_type = current_type_int;

      // Iterate type integer keeper forward
      current_type_int++;
    }
    else {
      node_int_type = loc_of_int_type->second;
    }

    // Check to make sure that this node doesn't already exist in the network
    auto base_level = get_level(level);
    if (base_level->find(id) != base_level->end()) {
      warning(id + " already exists in network\n");
    }
    else {
      // Add node to model
      SBM::add_node(id, node_int_type, level);
    }
  }

  NodePtr find_node_by_id(const std::string node_id, const int level)
  {
    try {
      return nodes.at(level)->at(node_id);
    }
    catch (...) {
      stop("Can't find node " + node_id + " at level " + std::to_string(level));
    }
  }

  void add_edge(const std::string node_a_id, const std::string node_b_id)
  {
    const NodePtr node_a = find_node_by_id(node_a_id, 0);
    const NodePtr node_b = find_node_by_id(node_b_id, 0);

    // If the user has specified allowed edges explicitely, make sure that this edge follows protocol
    if (specified_allowed_edges) {
      const bool a_to_b_bad = !(edge_type_pairs.at(node_a->type).count(node_b->type));
      const bool b_to_a_bad = !(edge_type_pairs.at(node_b->type).count(node_a->type));

      if (a_to_b_bad | b_to_a_bad) {
        stop("Edge of " + node_a_id + " - " + node_b_id + " does not fit allowed specified allowed_edge_pairs type combos.");
      }
    }
    else {
      // If the user has not specified the allowed edges explicitely, then build allowed combos from the edges
      add_allowed_node_type_combos(node_a->type, node_b->type);
    }

    SBM::add_edge(node_a, node_b);
  }

  // Convert the types from integers (cpp friendly) to strings (r friendly)
  std::vector<string> type_to_string(const std::vector<int>& int_types)
  {
    // Convert the type column to the string types
    std::vector<string> string_types;
    string_types.reserve(int_types.size());

    for (const auto& type : int_types) {
      // Convert int to string and push to vector
      string_types.push_back(type_int_to_string[type]);
    }

    return string_types;
  }

  int get_int_type(const std::string& string_type)
  {
    // Attempt to locate string type in string-to-int map
    const auto loc_of_int_type = type_string_to_int.find(string_type);

    // If type is not in map, throw an error
    if (loc_of_int_type == type_string_to_int.end()) {
      stop(string_type + " not found in model");
    }

    // Convert string to int and return
    return loc_of_int_type->second;
  }

  // Convert the types from strings to integers
  std::vector<int> type_to_int(const std::vector<string>& string_types)
  {
    std::vector<int> int_types;
    int_types.reserve(string_types.size());

    for (const auto& type : string_types) {
      // Convert string to int and push to vector
      int_types.push_back(get_int_type(type));
    }

    return int_types;
  }

  inline DataFrame state_to_df(State_Dump state)
  {
    // Create and return dump of state as dataframe
    return DataFrame::create(
        _["id"]               = state.id,
        _["parent"]           = state.parent,
        _["type"]             = type_to_string(state.type),
        _["level"]            = state.level,
        _["stringsAsFactors"] = false);
  }

  DataFrame get_state()
  {
    return state_to_df(SBM::get_state());
  }


  void set_node_parent(const std::string child_id,
                       const std::string parent_id,
                       const int         level = 0)
  {
    find_node_by_id(child_id, level)->set_parent(find_node_by_id(parent_id, level + 1));
  }

  void initialize_blocks(const int num_blocks, const int level)
  {
    Network::initialize_blocks(num_blocks, level);
  }

  double get_entropy(const int level)
  {
    if (get_level(level + 1)->size() == 0){
      stop("Can't compute entropy for model with no current block structure.");
    }
    return SBM::get_entropy(level);
  }

  // Sets up all the initial values for the node pair tracking structure
  inline void initialize_pair_tracking_map(std::unordered_map<std::string, Pair_Status>& concensus_pairs,
                                           const LevelPtr                                node_map)
  {
    for (auto node_a_it = node_map->begin();
         node_a_it != node_map->end();
         node_a_it++) {
      for (auto node_b_it = std::next(node_a_it);
           node_b_it != node_map->end();
           node_b_it++) {
        bool in_same_group = node_a_it->second->parent == node_b_it->second->parent;

        // Initialize pair info for group
        concensus_pairs.emplace(
            make_pair_key(node_a_it->first, node_b_it->first),
            Pair_Status(in_same_group));
      }
    }
  }

  // Update the concensus pair struct with a single sweep's results
  inline void update_pair_tracking_map(std::unordered_map<std::string, Pair_Status>& concensus_pairs,
                                       const std::unordered_set<std::string>&        updated_pairs)
  {
    for (auto& pair : concensus_pairs) {

      // Check if this pair was updated on last sweep
      const bool updated_last_sweep = updated_pairs.find(pair.first) != updated_pairs.end();

      if (updated_last_sweep) {
        // Update the pair connection status
        pair.second.connected = !(pair.second).connected;
      }

      // Increment the counts if needed
      if (pair.second.connected) {
        pair.second.times_connected++;
      }
    }
  }

  // =============================================================================
  // Runs multiple MCMC sweeps and keeps track of the results efficiently
  // =============================================================================
  List mcmc_sweep(const int    level,
                  const int    num_sweeps,
                  const double eps,
                  const bool   variable_num_blocks,
                  const bool   track_pairs,
                  const bool   verbose)
  {
    // Make sure network has blocks at the level for MCMC sweeps to take place.
    // Warn and initialize groups for user
    if (get_level(level + 1)->size() == 0) {
      warning("No blocks present. Initializing one block per node.");
      Network::initialize_blocks(-1, level);
    }

    MCMC_Sweeps results = SBM::mcmc_sweep(level,
                                          num_sweeps,
                                          eps,
                                          variable_num_blocks,
                                          track_pairs,
                                          verbose);

    // Initialize vectors to hold pair tracking results, if needed.
    std::vector<std::string> node_pair;
    std::vector<int>         times_connected;
    if (track_pairs) {
      results.block_consensus.dump_results(node_pair, times_connected);
    }

    // package up results into a list
    return List::create(
        _["nodes_moved"] = results.nodes_moved,
        _["sweep_info"]  = DataFrame::create(
            _["entropy_delta"]    = results.sweep_entropy_delta,
            _["num_nodes_moved"]  = results.sweep_num_nodes_moved,
            _["stringsAsFactors"] = false),
        _["pairing_counts"] = track_pairs ? DataFrame::create(
                                  _["node_pair"]        = node_pair,
                                  _["times_connected"]  = times_connected,
                                  _["stringsAsFactors"] = false)
                                          : "NA");
  }

  List collapse_blocks(const int    node_level,
                       const int    num_mcmc_steps,
                       const int    desired_num_blocks,
                       const int    num_checks_per_block,
                       const double sigma,
                       const double eps,
                       const bool   report_all_steps)
  {

    // Perform collapse
    const auto collapse_results = SBM::collapse_blocks(node_level,
                                                       num_mcmc_steps,
                                                       desired_num_blocks,
                                                       num_checks_per_block,
                                                       sigma,
                                                       eps,
                                                       report_all_steps);

    List entropy_results;

    for (const auto& step : collapse_results) {

      entropy_results.push_back(
          List::create(
              _["entropy_delta"] = step.entropy_delta,
              _["entropy"]       = step.entropy,
              _["state"]         = state_to_df(step.state),
              _["num_blocks"]    = step.num_blocks));
    }

    return entropy_results;
  }

  List collapse_run(const int              node_level,
                    const int              num_mcmc_steps,
                    const int              num_checks_per_block,
                    const double           sigma,
                    const double           eps,
                    const std::vector<int> block_nums)
  {

    List return_to_r;
    for (const int& target_num : block_nums) {
      return_to_r.push_back(
          collapse_blocks(
              node_level,
              num_mcmc_steps,
              target_num,
              num_checks_per_block,
              sigma,
              eps,
              false)[0]);
    }
    return return_to_r;
  }

  DataFrame get_node_to_block_edge_counts(const std::string id,
                                          const int         node_level        = 0,
                                          const int         connections_level = 1)
  {
    // Grab reference to node
    NodePtr node;
    try {
      /* code */
      node = get_node_by_id(id, node_level);
    }
    catch (const std::exception& e) {
      stop("Could not find requested node " + id + " at level " + std::to_string(node_level));
    }

    // Get edges to desired level
    const NodeEdgeMap node_connections = node->gather_edges_to_level(connections_level);
    const int         n_connections    = node_connections.size();

    // Build dataframe with these values
    std::vector<std::string> connection_id;
    std::vector<int>         connection_count;
    connection_id.reserve(n_connections);
    connection_count.reserve(n_connections);

    for (const auto& connection : node_connections) {
      connection_id.push_back(connection.first->id);
      connection_count.push_back(connection.second);
    }

    return DataFrame::create(_["id"]               = connection_id,
                             _["count"]            = connection_count,
                             _["stringsAsFactors"] = false);
  }

  DataFrame get_block_edge_counts(const int level = 1)
  {
    // Make sure we have blocks at the level asked for before proceeding
    if (nodes.count(level) == 0){
      stop("Model has no blocks at level " + std::to_string(level));
    }

    // Gather to our block_edge to count map
    std::map<Edge, int> block_edge_counts = Network::gather_block_counts_at_level(level);

    const int n_pairs = block_edge_counts.size();

    // Initialize some vectors to return results with
    // Build dataframe with these values
    std::vector<std::string> block_a;
    std::vector<std::string> block_b;
    std::vector<int>         counts;
    block_a.reserve(n_pairs);
    block_b.reserve(n_pairs);
    counts.reserve(n_pairs);

    // Fill in
    for (const auto& block_edge : block_edge_counts) {
      block_a.push_back(block_edge.first.node_a->id);
      block_b.push_back(block_edge.first.node_b->id);
      counts.push_back(block_edge.second);
    }

    // Return
    return DataFrame::create(_["block_a"]          = block_a,
                             _["block_b"]          = block_b,
                             _["count"]            = counts,
                             _["stringsAsFactors"] = false);
  }

  void load_from_state(std::vector<string> id,
                       std::vector<string> parent,
                       std::vector<int>    level,
                       std::vector<string> string_types)
  {

    // Construct a state dump from vectors and
    // pass the constructed state to load_state function
    SBM::load_from_state(State_Dump(id, parent, level, type_to_int(string_types)));
  }



};

RCPP_MODULE(SBM)
{
  class_<Rcpp_SBM>("SBM")

      .constructor()

      .method("add_node",
              &Rcpp_SBM::add_node,
              "Add a node to the network. Takes the node id (string), the node type (string), and the node level (int). Use level = 0 for data-level nodes.")
      .method("add_edge",
              &Rcpp_SBM::add_edge,
              "Connects two nodes in network (at level 0) by their ids (string).")
      .method("add_allowed_pairs",
              &Rcpp_SBM::add_allowed_pairs,
              "Add list of allowed pairs of node types for edges.")
      .method("set_node_parent",
              &Rcpp_SBM::set_node_parent,
              "Sets the parent node (or block) for a given node. Takes child node's id (string), parent node's id (string), and the level of child node (int).")
      .method("initialize_blocks",
              &Rcpp_SBM::initialize_blocks,
              "Adds a desired number of blocks and randomly assigns them for a given level. num_blocks = -1 means every node gets their own block")
      .method("get_state",
              &Rcpp_SBM::get_state,
              "Exports the current state of the network as dataframe with each node as a row and columns for node id, parent id, node type, and node level.")
      .method("get_node_to_block_edge_counts",
              &Rcpp_SBM::get_node_to_block_edge_counts,
              "Returns a dataframe with the requested nodes connection counts to all blocks/nodes at a desired level.")
      .method("get_block_edge_counts",
              &Rcpp_SBM::get_block_edge_counts,
              "Returns a dataframe of counts of edges between all connected pairs of blocks at given level.")
      .method("load_from_state",
              &Rcpp_SBM::load_from_state,
              "Takes model state export as given by SBM$get_state() and returns model to specified state. This is useful for resetting model before running various algorithms such as agglomerative merging.")
      .method("get_entropy",
              &Rcpp_SBM::get_entropy,
              "Computes the (degree-corrected) entropy for the network at the specified level (int).")
      .method("mcmc_sweep",
              &Rcpp_SBM::mcmc_sweep,
              "Runs a single MCMC sweep across all nodes at specified level. Each node is given a chance to move blocks or stay in current block and all nodes are processed in random order. Takes the level that the sweep should take place on (int) and if new blocks blocks can be proposed and empty blocks removed (boolean).")
      .method("collapse_blocks",
              &Rcpp_SBM::collapse_blocks,
              "Performs agglomerative merging on network, starting with each block has a single node down to one block per node type. Arguments are level to perform merge at (int) and number of MCMC steps to peform between each collapsing to equilibriate block. Returns list with entropy and model state at each merge.")
      .method("collapse_run",
              &Rcpp_SBM::collapse_run,
              "Performs a sequence of block collapse steps on network. Targets a range of final blocks numbers and collapses to them and returns final result form each collapse.");
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

sbm$add_edge("a1", "b1")
sbm$add_edge("a1", "b2")
sbm$add_edge("a1", "b3")
sbm$add_edge("a2", "b3")
sbm$add_edge("a3", "b2")


sbm$set_node_parent("a1", "a11", 0)
sbm$set_node_parent("a2", "a11", 0)
sbm$set_node_parent("a3", "a12", 0)
sbm$set_node_parent("b1", "b11", 0)
sbm$set_node_parent("b2", "b11", 0)
sbm$set_node_parent("b3", "b12", 0)



original_state <- sbm$get_state()

for(i in 1:10){
  entro_pre <- sbm$get_entropy(0L)
  blocks_moved <- sbm$mcmc_sweep(0L,FALSE)
  print(paste("started with entropy of", entro_pre, "and moved", blocks_moved))
}

new_state <- sbm$get_state()

load_state(sbm, original_state)


# Bring me back to original state
# load_state(sbm, original_state)
#
# library(tidyverse)
# merge_results <- sbm$collapse_blocks(0, 15)
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
# sbm$get_entropy(0L)


*/
