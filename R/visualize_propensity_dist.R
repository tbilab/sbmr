#' Visualize pairwise propensity distribution
#'
#' Investigate the results of cluster stability from MCMC sweeps. Shows the
#' empirical CDF of the pairwise probability/proportion of two nodes residing in
#' the same block over the given MCMC sweeps.
#'
#' `sbmr::mcmc_sweep(sbm, track_pairs = TRUE, ...)` must be run on object before this function.
#'
#' @family visualizations
#'
#' @inheritParams verify_model
#' @inheritParams get_sweep_pair_counts
#'
#' @return GGplot2 plot containing ECDF of the pairwise propensities of residing in same block for MCMC sweeps.
#' @export
#'
#' @examples
#'
#' # Simulate a model
#' net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 25) %>%
#'   initialize_blocks(3) %>%
#'   mcmc_sweep(num_sweeps = 75, track_pairs = TRUE)
#'
#' # Plot results
#' visualize_propensity_dist(net)
#'
visualize_propensity_dist <- function(sbm, isolate_type = NULL){
  UseMethod("visualize_propensity_dist")
}


#' @export
visualize_propensity_dist.sbm_network <- function(sbm, isolate_type = NULL){
  pair_counts <- get_sweep_pair_counts(sbm, isolate_type = isolate_type)

  non_zero_pair <- which(pair_counts$times_connected != 0)[1]
  n_sweeps <- round(pair_counts$times_connected[non_zero_pair]/pair_counts$proportion_connected[non_zero_pair])

  ggplot2::ggplot(pair_counts, ggplot2::aes(x = proportion_connected, color = type)) +
    ggplot2::stat_ecdf() +
    ggplot2::labs(
      title = "Pairwise block concensus proportion distribution",
      subtitle = glue::glue("Results from {n_sweeps} MCMC sweeps"),
      y = "Proportion of pairs w/ prob < p",
      x = "prob of connection"
    )
}

utils::globalVariables(c("proportion_connected"))

