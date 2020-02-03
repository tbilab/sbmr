#' Trace plot of multiple MCMC sweep sweeps
#'
#' A simple line plot showing both the entropy delta and the number of nodes
#' moved for each sweep.
#'
#' @param sweep_results Results of running `sbmR::mcmc_sweep()`
#'
#' @return GGplot line plot with x-axis as sweep number and y-axes as sweep
#'   entropy delta and number of nodes moved during sweep.
#' @export
#'
#' @examples
#'
#' set.seed(42)
#'
#' # Start with a random network and assign randomly to 4 blocks
#' n_blocks <- 3
#' my_sbm <- create_sbm(sim_basic_block_network(n_blocks = n_blocks, n_nodes_per_block = 15)) %>%
#'   initialize_blocks(num_blocks = n_blocks)
#'
#' # Run 25 MCMC sweeps
#' sweep_results <- my_sbm %>% mcmc_sweep(num_sweeps = 25, variable_num_blocks = FALSE)
#'
#' # Plot results
#' visualize_mcmc_trace(sweep_results)
#'
visualize_mcmc_trace <- function(sweep_results){

  sweep_results$sweep_info %>%
    dplyr::mutate(sweep = 1:n()) %>%
    tidyr::pivot_longer(entropy_delta:num_nodes_moved) %>%
      ggplot2::ggplot(ggplot2::aes(x = sweep, y = value)) +
      ggplot2::geom_line() +
      ggplot2::facet_grid(name~., scales = "free_y")
}
