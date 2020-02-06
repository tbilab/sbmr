#' Sample from model posterior
#'
#' Function is currently under construction
#'
#' @family modeling
#'
#' @param ... Additional arguments to \code{\link{new_sbm_network}}
#'
#' @return
#' @export
#'
#' @examples
#'
#' sample_posterior(sim_basic_block_network())
#'
sample_posterior <- function(
  edges, nodes = NULL,
  n_sweeps = 100,
  n_chains = 1,
  burnin_sweeps = 10,
  return_sweep_info = TRUE, # separate df of entropy by sweep
  starting_partition = "agglomerative",
  epsilon = 0.1,
  greedy = TRUE,
  N_CHECKS_PER_BLOCK = 5,
  initialization_mcmc_steps = 15,
  verbose = c("initialization"),
  ...
){
  sbm <- create_sbm(edges, nodes, ...)



  sbm
}
