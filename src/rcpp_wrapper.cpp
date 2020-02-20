#include "SBM.h"


#include <RcppCommon.h>

// declaring the specialization
namespace Rcpp {
  template <> SEXP wrap(const State_Dump&);
  template <> SEXP wrap(const NodePtr&);
  template <> SEXP wrap(const BlockEdgeCounts&);
  template <> SEXP wrap(const MCMC_Sweeps&);
  template <> SEXP wrap(const CollapseResults&);
  template <> SEXP wrap(const NodeEdgeMap&);
}

#include <Rcpp.h>
using namespace Rcpp;

inline DataFrame state_to_df(const State_Dump& state)
{
  // Create and return dump of state as dataframe
  return DataFrame::create(
      _["id"]               = state.id,
      _["parent"]           = state.parent,
      _["type"]             = state.type,
      _["level"]            = state.level,
      _["stringsAsFactors"] = false);
}

namespace Rcpp {
// Let Rcpp know how to convert a state dump object into a dataframe for R
template <>
SEXP wrap(const State_Dump& state)
{
  // Create and return dump of state as dataframe
  return DataFrame::create(
      _["id"]               = state.id,
      _["parent"]           = state.parent,
      _["type"]             = state.type,
      _["level"]            = state.level,
      _["stringsAsFactors"] = false);
}

// Node pointer just gets converted to the node id
template <>
SEXP wrap(const NodePtr& node_ref)
{
  return 0;
}

template <>
SEXP wrap(const BlockEdgeCounts& block_edge_counts)
{

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

template <>
SEXP wrap(const MCMC_Sweeps& results)
{

  // Check if we have pair tracking information present
  const int  size_tracked_pairs = results.block_consensus.concensus_pairs.size();
  const bool tracked_pairs      = size_tracked_pairs > 0;

  // Initialize vectors to hold pair tracking results, if needed.
  std::vector<std::string> node_pair;
  std::vector<int>         times_connected;

  if (tracked_pairs) {
    // Fill out pair tracking vectors with map internals
    node_pair.reserve(size_tracked_pairs);
    times_connected.reserve(size_tracked_pairs);

    for (const auto& pair : results.block_consensus.concensus_pairs) {
      node_pair.push_back(pair.first);
      times_connected.push_back((pair.second).times_connected);
    }
  }

  // package up results into a list
  return List::create(
      _["nodes_moved"] = results.nodes_moved,
      _["sweep_info"]  = DataFrame::create(
          _["entropy_delta"]    = results.sweep_entropy_delta,
          _["num_nodes_moved"]  = results.sweep_num_nodes_moved,
          _["stringsAsFactors"] = false),
      _["pairing_counts"] = tracked_pairs ? DataFrame::create(
                                _["node_pair"]        = node_pair,
                                _["times_connected"]  = times_connected,
                                _["stringsAsFactors"] = false)
                                          : "NA");
}

template <>
SEXP wrap(const CollapseResults& collapse_results)
{
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

template <>
SEXP wrap(const NodeEdgeMap& node_connections)
{
  const int n_connections = node_connections.size();

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
};
}

RCPP_MODULE(SBM)
{
  class_<SBM>("SBM")

      .constructor()

      .method("add_node",
              &SBM ::add_node,
              "Add a node to the network. Takes the node id (string), the node type (string), and the node level (int). Use level = 0 for data-level nodes.")
      .method("add_edge",
              &SBM ::add_edge,
              "Connects two nodes in network (at level 0) by their ids (string).")
      .method("add_edge_types",
              &SBM ::add_edge_types,
              "Add list of allowed pairs of node types for edges.")
      .method("initialize_blocks",
              &SBM ::initialize_blocks,
              "Adds a desired number of blocks and randomly assigns them for a given level. num_blocks = -1 means every node gets their own block")
      .method("get_state",
              &SBM ::get_state,
              "Exports the current state of the network as dataframe with each node as a row and columns for node id, parent id, node type, and node level.")
      .method("get_node_to_block_edge_counts",
              &SBM ::get_node_to_block_edge_counts,
              "Returns a dataframe with the requested nodes connection counts to all blocks/nodes at a desired level.")
      .method("get_block_edge_counts",
              &SBM ::get_block_edge_counts,
              "Returns a dataframe of counts of edges between all connected pairs of blocks at given level.")
      .method("load_from_state",
              &SBM ::load_from_state,
              "Takes model state export as given by SBM$get_state() and returns model to specified state. This is useful for resetting model before running various algorithms such as agglomerative merging.")
      .method("get_entropy",
              &SBM ::get_entropy,
              "Computes the (degree-corrected) entropy for the network at the specified level (int).")
      .method("mcmc_sweep",
              &SBM ::mcmc_sweep,
              "Runs a single MCMC sweep across all nodes at specified level. Each node is given a chance to move blocks or stay in current block and all nodes are processed in random order. Takes the level that the sweep should take place on (int) and if new blocks blocks can be proposed and empty blocks removed (boolean).")
      .method("collapse_blocks",
              &SBM ::collapse_blocks,
              "Performs agglomerative merging on network, starting with each block has a single node down to one block per node type. Arguments are level to perform merge at (int) and number of MCMC steps to peform between each collapsing to equilibriate block. Returns list with entropy and model state at each merge.")
      .method("collapse_run",
              &SBM ::collapse_run,
              "Performs a sequence of block collapse steps on network. Targets a range of final blocks numbers and collapses to them and returns final result form each collapse.");
}
