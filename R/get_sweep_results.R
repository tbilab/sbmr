#' Get mcmc sweep results
#'
#' Retrieves the MCMC sweep results from an `sbm_network` object. Needs to have
#' \code{\link{mcmc_sweep}} run on the object before.
#'
#' @family helpers
#'
#' @inheritParams verify_model
#'
#' @return MCMC sweep results from an `sbm_network` object
#' @export
#'
#' @examples
#'
#' # Start with a small random network
#' net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 15) %>%
#'   initialize_blocks(num_blocks = 4) %>%
#'   mcmc_sweep(num_sweeps = 4, variable_num_blocks = FALSE)
#'
#' # Retrieve the sweep results from network
#' get_sweep_results(net)
#'
get_sweep_results <- function(sbm){
  UseMethod("get_sweep_results")
}

get_sweep_results.default <- function(sbm){
  cat("get_sweep_results generic")
}

#' @export
get_sweep_results.sbm_network <- function(sbm){
  sweep_results <- sbm$mcmc_sweeps
  if(is.null(sweep_results)){
    stop("mcmc_sweep() must be run on network before results can be retrieved")
  }

  sweep_results
}
