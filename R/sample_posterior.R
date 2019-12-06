#' Sample from SBM posterior
#'
#' @inheritParams create_sbm
#' @param n_sweeps number of MCMC sweeps of all nodes (after burn-in)
#' @param n_chains How many seperate MCMC chains to start and run
#' @param burnin_sweeps How many sweeps to run before starting to record
#' @param return_sweep_info Return entropy and number of nodes changed for each
#'   sweep (defaults to `TRUE`)
#' @param starting_partition Algorithm to start each chain with? Current options
#'   are `"agglomerative"` or `"random"`.
#' @param verbose Array of what info should be printed as model runs. Default
#'   shows `"initialization"` info of model. Other options are `"silent` to
#'   print nothing, or `"iteration progress"` which prints out results of each
#'   mcmc iteration as it happens.
#' @param ... Additional arguments to \code{\link{create_sbm}}
#'
#' @return
#' @export
#'
#' @examples
sample_posterior <- function(
  edges, nodes = NULL,
  n_sweeps = 100,
  n_chains = 1,
  burnin_sweeps = 10,
  return_sweep_info = TRUE, # separate df of entropy by sweep
  starting_partition = "agglomerative",
  verbose = c("initialization"),
  ...
){
  sbm <- create_sbm(edges, nodes = NULL, ...)
  sbm
}
