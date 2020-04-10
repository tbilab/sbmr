#' Compute entropy for current model state
#'
#' Computes the (degree-corrected) entropy for the network at the node level.
#'
#' @family modeling
#'
#' @inheritParams add_node
#'
#' @return Entropy value (numeric).
#' @export
#'
#' @examples
#'
#' set.seed(42)
#'
#' # Start with a small simulated network with random block assignments
#' net <- sim_basic_block_network(n_blocks = 4, n_nodes_per_block = 15) %>%
#'   initialize_blocks(n_blocks = 4)
#'
#' # Calculate entropy with random blocks
#' entropy(net)
#'
#' # Run some MCMC sweeps
#' net <- mcmc_sweep(net, num_sweeps = 25, variable_n_blocks = FALSE)
#'
#' # Entropy after sweeps
#' entropy(net)
#'
entropy <- function(sbm){
  UseMethod("entropy")
}

entropy.default <- function(sbm){
  cat("entropy generic")
}

#' @export
entropy.sbm_network <- function(sbm){
  attr(verify_model(sbm), 'model')$entropy(0L)
}
