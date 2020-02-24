#' Get pairwise group sharing counts from model
#'
#' Retrieves the pairwise connection propensity counts from latest MCMC sweep on an `sbm_network` object. Needs to have
#' \code{\link{mcmc_sweep(track_pairs = TRUE)}} run on the object before.
#'
#' @family helpers
#'
#' @inheritParams verify_model
#'
#' @return pairwise connection propensity counts from latest MCMC sweep
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
get_sweep_pair_counts <- function(sbm){
  UseMethod("get_sweep_pair_counts")
}

get_sweep_pair_counts.default <- function(sbm){
  cat("get_sweep_pair_counts generic")
}

#' @export
get_sweep_pair_counts.sbm_network <- function(sbm){

  pair_counts <- get_sweep_results(sbm)$pairing_counts

  # Make sure we have propensity counts before proceeding
  if(is.null(pair_counts)){
    stop("Sweep results do not contain pairwise propensities. Try rerunning MCMC sweep with track_pairs = TRUE.")
  }

  pair_counts
}
