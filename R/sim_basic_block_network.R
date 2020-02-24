#' Simulate stochastic block model of given number of blocks
#'
#' Simulates a simple block structured network with a desired number of constant
#' sized blocks. The probability of edges between given blocks will be
#' drawn from a passed function that must return a numeric value that will be
#' used as the propensity parameter for drawing edge counts from a node pair.
#' The default values will draw propensity values for bernouli edges with an
#' average of a 50% chance of edge.
#'
#' If `return_edge_propensities == TRUE`: an `$edge_propensities` slot is added
#' to the returned `sbm_network` object containing a dataframe that shows the
#' randomly drawn edge propensities between blocks.
#'
#' @family simulations
#'
#' @param n_blocks How many blocks to simulate
#' @param n_nodes_per_block How many nodes in each block
#' @param propensity_drawer Function that takes a single size argumenet and
#'   returns a vector of edge propensities of specified size. Default
#'   draws propensity from `n` evenly spaced values in the 0-1 range.
#' @param return_edge_propensities If set to `TRUE` the returned list will
#'   also include the simulated edge propensities dataframe. This can be
#'   used for recreating draws using \code{\link{sim_sbm_network()}}.
#' @inheritParams sim_sbm_network
#'
#' @seealso \code{\link{sim_sbm_network}} \code{\link{sim_random_network}}
#'
#' @inherit new_sbm_network return
#'
#' @export
#'
#' @examples
#'
#' sim_basic_block_network(n_blocks = 4, n_nodes_per_block = 40)
#'
#' sim_basic_block_network(n_blocks = 8, n_nodes_per_block = 20)
#'
#' # Can save the generating edge propensities as well
#' net <- sim_basic_block_network(n_blocks = 4,
#'                                n_nodes_per_block = 20,
#'                                return_edge_propensities = TRUE)
#' net$edge_propensities
#'
#'
#' right_skewed_beta <- function(n) rbeta(n, 1, 5)
#'
#' # Distribution that draws propensities can be customized
#' net <- sim_basic_block_network(n_blocks = 4,
#'                                n_nodes_per_block = 20,
#'                                return_edge_propensities = TRUE,
#'                                propensity_drawer = right_skewed_beta)
#' net$edge_propensities
#'
sim_basic_block_network <- function(
  n_blocks = 2,
  n_nodes_per_block = 5,
  propensity_drawer = function(n){sample(seq(rbeta(1, 1, 5),rbeta(1, 5, 1),length.out = n))},
  edge_dist = purrr::rbernoulli,
  allow_self_edges = FALSE,
  keep_edge_counts = FALSE,
  return_edge_propensities = FALSE,
  setup_model = FALSE,
  random_seed = NULL){

  # Build blocks option with a constant number of nodes per block
  blocks <- dplyr::tibble(
    block = paste0("g", 1:n_blocks),
    n_nodes = n_nodes_per_block
  )

  # Build edge propensities by going through all combinations of block
  # pairs and drawing the edge propensity from the passed
  # prob_of_edges_dist function.
  block_pair_inds <- get_combination_indices(n_blocks, repeats = TRUE)
  n_pairs <- length(block_pair_inds$a)

  edge_propensities <- dplyr::tibble(
    block_1 = blocks$block[block_pair_inds$a],
    block_2 = blocks$block[block_pair_inds$b],
    propensity = propensity_drawer(n_pairs)
  )

  # Pass the constructed dataframes to the main sbm simulation function and remove edges
  simulated_sbm_network <- sim_sbm_network(
    block_info = blocks,
    edge_propensities = edge_propensities,
    edge_dist = purrr::rbernoulli,
    allow_self_edges = allow_self_edges,
    keep_edge_counts = keep_edge_counts,
    setup_model = setup_model,
    random_seed = random_seed
  )

  if (return_edge_propensities){
    # Add a value for the edge propensities to object if desired
    simulated_sbm_network$edge_propensities <- edge_propensities
  }

  simulated_sbm_network
}

