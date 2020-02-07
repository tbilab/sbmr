#' Get collapse results from model
#'
#' Retrieves the dataframe recording the results of running either
#' \code{\link{collapse_groups()}} or \code{\link{collapse_run()}} on
#' `sbm_network` object.
#'
#' @inheritParams verify_model
#'
#' @return
#' @export
#'
#' @examples
#'
#' # Start with a random network of two blocks with 25 nodes each and
#' # run agglomerative clustering with no intermediate MCMC steps on network
#' my_sbm <- sim_basic_block_network(n_blocks = 2, n_nodes_per_block = 25) %>%
#'   collapse_blocks(num_mcmc_sweeps = 0)
#'
#' # Look at the results of the collapse directly
#' get_collapse_results(my_sbm)
#'
get_collapse_results <- function(sbm){
  UseMethod("get_collapse_results")
}

get_collapse_results.default <- function(sbm){
  cat("get_collapse_results generic")
}

#' @export
get_collapse_results.sbm_network <- function(sbm){
  collapse_results <- sbm$collapse_results
  if(is.null(collapse_results)) {
    stop("Need to run collapse_groups() or collapse_run() on network before retrieving collapse results")
  }
  collapse_results
}
