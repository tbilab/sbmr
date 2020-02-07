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
#'   initialize_blocks(num_blocks = 4)
#'
#' # Calculate entropy with random blocks
#' get_entropy(net)
#'
#' # Run some MCMC sweeps
#' net <- mcmc_sweep(net, num_sweeps = 25, variable_num_blocks = FALSE)
#'
#' # Entropy after sweeps
#' get_entropy(net)
#'
get_entropy <- function(sbm){
  UseMethod("get_entropy")
}

get_entropy.default <- function(sbm){
  cat("get_entropy generic")
}

#' @export
get_entropy.sbm_network <- function(sbm){
  verify_model(sbm)$model$get_entropy(0L)
}
