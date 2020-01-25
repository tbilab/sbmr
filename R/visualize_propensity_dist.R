#' Visualize pairwise propensity distribution
#'
#' Investigate the results of cluster stability from MCMC sweeps. Shows the
#' empirical CDF of the pairwise probability/proportion of two nodes residing in
#' the same block over the given MCMC sweeps.
#'
#' @param sweep_results Results from running \code{\link{mcmc_sweep()}} with the argument `track_pairs = TRUE`.
#'
#' @return GGplot2 plot containing ECDF of the pairwise propensities of residing in same block for MCMC sweeps.
#' @export
#'
#' @examples
#'
#' network <- sim_basic_block_network(
#'   n_blocks = 3,
#'   n_nodes_per_block = 25
#' )
#' # Initialize model with random groups and run 75 MCMC sweeps
#' sweep_results <- create_sbm(network) %>%
#'   initialize_blocks(3) %>%
#'   mcmc_sweep(num_sweeps = 75, track_pairs = TRUE)
#'
#' # Plot results
#' visualize_propensity_dist(sweep_results)
#'
visualize_propensity_dist <- function(sweep_results){

  # Make sure we have propensity counts before proceeding
  if(is.null(sweep_results$pairing_counts)){
    stop("Sweep results do not contain pairwise propensities. Try rerunning MCMC sweep with track_pairs = TRUE.")
  }

  n_sweeps <- nrow(sweep_results$sweep_info)

  sweep_results$pairing_counts %>%
    ggplot2::ggplot(ggplot2::aes(x = proportion_connected)) +
    ggplot2::stat_ecdf() +
    ggplot2::labs(
      title = "Pairwise block concensus proportion distribution",
      subtitle = glue::glue("Results from {n_sweeps} MCMC sweeps"),
      y = "Proportion of pairs w/ prob < p",
      x = "prob of connection"
    )
}
