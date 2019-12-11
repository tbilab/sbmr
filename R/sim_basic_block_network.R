#' Simulate basic block network
#'
#' Simulates a simple block structured network with a desired number of constant
#' sized groups. The probability of connections between given groups will be
#' drawn from a passed function that must return a numeric value that will be
#' used as the propensity parameter for drawing edge counts from a node pair.
#' The default values will draw propensity values for bernouli edges with an
#' average of a 50% chance of connection.
#'
#' @param n_groups How many groups to simulate
#' @param n_nodes_per_group How many nodes in each group
#' @param prob_of_connections_dist Function that takes a single size argumenet
#'   and returns a vector of connection propensities of specified size. Default
#'   is drawing Bernouli probabilities from a symmetric beta distribution
#'   centered at 0.5
#' @inheritParams sim_sbm_network
#'
#' @seealso \code{\link{sim_sbm_network}} \code{\link{sim_simple_network}}
#'
#' @return A list with a `nodes` dataframe (containing a node's `id` and `group`
#'   membership) and a `edges` dataframe (containing `from` and `to` nodes along
#'   with the total number of `connections` as drawn from `edge_dist`.)
#' @export
#'
#' @examples
#' sim_basic_block_network(n_groups = 4, n_nodes_per_group = 10)
sim_basic_block_network <- function(
  n_groups = 2,
  n_nodes_per_group = 5,
  prob_of_connections_dist = function(n) rbeta(n = n, shape1 = 3.5, shape2 = 3.5),
  edge_dist = purrr::rbernoulli,
  allow_self_connections = FALSE,
  keep_connection_counts = FALSE){

  # Build groups option with a constant number of nodes per group
  groups <- dplyr::tibble(
    group = paste0("g", 1:n_groups),
    n_nodes = n_nodes_per_group
  )

  # Build connection propensities by going through all combinations of group
  # pairs and drawing the connection propensity from the passed
  # prob_of_connections_dist function.
  group_pair_inds <- get_combination_indices(n_groups, repeats = TRUE)
  n_pairs <- length(group_pair_inds$a)
  connection_propensities <- dplyr::tibble(
    group_1 = groups$group[group_pair_inds$a],
    group_2 = groups$group[group_pair_inds$b],
    propensity = prob_of_connections_dist(n_pairs)
  )

  # Pass the constructed dataframes to the main sbm simulation function and remove connections
  sim_sbm_network(
    group_info = groups,
    connection_propensities = connection_propensities,
    edge_dist = purrr::rbernoulli,
    allow_self_connections = allow_self_connections,
    keep_connection_counts = keep_connection_counts
  )
}

