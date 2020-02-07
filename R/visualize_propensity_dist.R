#' Visualize pairwise propensity distribution
#'
#' Investigate the results of cluster stability from MCMC sweeps. Shows the
#' empirical CDF of the pairwise probability/proportion of two nodes residing in
#' the same block over the given MCMC sweeps.
#'
#' `sbmR::mcmc_sweep(sbm, track_pairs = TRUE, ...)` must be run on object before this function.
#'
#' @family visualizations
#'
#' @inheritParams verify_model
#'
#' @return GGplot2 plot containing ECDF of the pairwise propensities of residing in same block for MCMC sweeps.
#' @export
#'
#' @examples
#'
#' # Simulate a model
#' net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 25) %>%
#'   initialize_blocks(3)
#'
#' # Initialize model with random groups and run 75 MCMC sweeps
#' sweep_results <- net %>% mcmc_sweep(num_sweeps = 75, track_pairs = TRUE)
#'
#' # Plot results
#' visualize_propensity_dist(sweep_results)
#'
visualize_propensity_dist <- function(sbm){
  UseMethod("visualize_propensity_dist")
}

visualize_propensity_dist.default <- function(sbm){
  cat("visualize_propensity_dist generic")
}

#' @export
visualize_propensity_dist.sbm_network <- function(sbm){

  sweep_results <- get_mcmc_sweep_results(sbm)

  n_sweeps <- nrow(sweep_results$sweep_info)

  get_mcmc_pair_counts(sweep_results) %>%
    ggplot2::ggplot(ggplot2::aes(x = proportion_connected)) +
    ggplot2::stat_ecdf() +
    ggplot2::labs(
      title = "Pairwise block concensus proportion distribution",
      subtitle = glue::glue("Results from {n_sweeps} MCMC sweeps"),
      y = "Proportion of pairs w/ prob < p",
      x = "prob of connection"
    )
}

