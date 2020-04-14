#' Get pairwise group sharing counts from model
#'
#' Retrieves the pairwise connection propensity counts from latest MCMC sweep on an `sbm_network` object. Needs to have
#' \code{\link{mcmc_sweep}}`(track_pairs = TRUE)` run on the object before.
#'
#' @family helpers
#'
#' @inheritParams verify_model
#' @param isolate_type Node type to be isolated for visualization. If left empty all types are included.
#'
#' @return pairwise connection propensity counts from latest MCMC sweep
#' @export
#'
#' @examples
#'
#' # Start with a small random network
#' net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 15) %>%
#'   initialize_blocks(n_blocks = 4) %>%
#'   mcmc_sweep(num_sweeps = 4, variable_n_blocks = FALSE)
#'
#' # Retrieve the sweep results from network
#' get_sweep_results(net)
#'
get_sweep_pair_counts <- function(sbm, isolate_type = NULL){
  UseMethod("get_sweep_pair_counts")
}



#' @export
get_sweep_pair_counts.sbm_network <- function(sbm, isolate_type = NULL){
  pair_counts <- get_sweep_results(sbm)$pairing_counts

  # Make sure we have propensity counts before proceeding
  if(is.null(pair_counts)){
    stop("Sweep results do not contain pairwise propensities. Try rerunning MCMC sweep with track_pairs = TRUE.")
  }

  # Join type info to pairs
  pair_counts <- dplyr::left_join(pair_counts, dplyr::select(sbm$nodes, node_a = id, type), by = "node_a")

  if(!is.null(isolate_type)){
    return(dplyr::filter(pair_counts, type == isolate_type))
  } else {
    return(pair_counts)
  }

}
