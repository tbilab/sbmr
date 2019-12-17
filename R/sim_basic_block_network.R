#' Simulate stochastic block model of given number of groups
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
#' @param propensity_drawer Function that takes a single size argumenet and
#'   returns a vector of connection propensities of specified size. Default
#'   draws propensity from `n` evenly spaced values in the 0-1 range.
#' @param return_connection_propensities If set to `TRUE` the returned list will
#'   also include the simulated connection propensities dataframe. This can be
#'   used for recreating draws using \code{\link{sim_sbm_network()}}.
#' @inheritParams sim_sbm_network
#'
#' @seealso \code{\link{sim_sbm_network}} \code{\link{sim_random_network}}
#'
#' @return A list with a `nodes` dataframe (containing a node's `id` and `group`
#'   membership), an `edges` dataframe (containing `from` and `to` nodes along
#'   with the total number of `connections` as drawn from `edge_dist`), and if
#'   `return_connection_propensities == TRUE`: `connection_propensities`
#'   dataframe that shows the randomly drawn connection propensities between
#'   groups.
#' @export
#'
#' @examples
#' sim_basic_block_network(n_groups = 4, n_nodes_per_group = 10)
sim_basic_block_network <- function(
  n_groups = 2,
  n_nodes_per_group = 5,
  propensity_drawer = function(n){sample(seq(rbeta(1, 1, 5),rbeta(1, 5, 1),length.out = n))},
  edge_dist = purrr::rbernoulli,
  allow_self_connections = FALSE,
  keep_connection_counts = FALSE,
  return_connection_propensities = FALSE){

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
    propensity = propensity_drawer(n_pairs)
  )

  # Pass the constructed dataframes to the main sbm simulation function and remove connections
  sim_results <- sim_sbm_network(
    group_info = groups,
    connection_propensities = connection_propensities,
    edge_dist = purrr::rbernoulli,
    allow_self_connections = allow_self_connections,
    keep_connection_counts = keep_connection_counts
  )

  if (return_connection_propensities){
    sim_results$connection_propensities <- connection_propensities
  }

  sim_results
}

