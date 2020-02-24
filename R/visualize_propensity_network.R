#' Plot network of nodes connected by pairwise block propensity
#'
#' Takes the results of `mcmc_sweep()` with `track_pairs = TRUE` and plots the
#' data as a network where two nodes have an edge between them if their pairwise
#' propensity to be in the same block is above some threshold.
#'
#'
#' @family visualizations
#'
#' @inheritParams  visualize_propensity_dist
#' @param proportion_threshold Threshold of pairwise propensity to consider two nodes linked. Choose carefully!
#'
#' @return An interactive network plot with force layout.
#' @export
#'
#' @examples
#'
#' set.seed(42)
#' # Simulate network data and initialize model with it
#' net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 30) %>%
#'   collapse_blocks(desired_num_blocks = 1, sigma = 1.1) %>%
#'   choose_best_collapse_state(verbose = TRUE) %>%
#'   mcmc_sweep(num_sweeps = 100, eps = 0.4, track_pairs = TRUE)
#'
#' # Plot connection propensity network
#' visualize_propensity_network(net, proportion_threshold = 0.4)
#'
visualize_propensity_network <- function(sbm, proportion_threshold = 0.2){
  UseMethod("visualize_propensity_network")
}

visualize_propensity_network.default <- function(sbm, proportion_threshold = 0.2){
  cat("visualize_propensity_network generic")
}

#' @export
visualize_propensity_network.sbm_network <- function(sbm, proportion_threshold = 0.2){

  # Make sure we have propensity counts before proceeding
  pair_counts <- get_sweep_pair_counts(sbm)

  edges <- pair_counts %>%
    dplyr::filter(proportion_connected > proportion_threshold) %>%
    dplyr::select(from = node_a, to = node_b)

  if (nrow(edges) == 0){
    stop("No node-node propensities exceed threshold. Try lowering threshold and/or making sure your model's MCMC chain has equilibriated.")
  }

  # Get all unique nodes and their avg non-zero pairwise connection proportions
  nodes <- pair_counts %>%
    tidyr::pivot_longer(c(node_a, node_b), values_to="id") %>%
    dplyr::group_by(id) %>%
    dplyr::summarise(avg_prop_connection = mean(proportion_connected[proportion_connected > 0])) %>%
    dplyr::distinct(id, avg_prop_connection)

  new_sbm_network(edges = edges, nodes = nodes, setup_model = FALSE) %>%
    visualize_network(node_color_col = "avg_prop_connection")

}

utils::globalVariables(c("proportion_connected", "node_a", "node_b", "avg_prop_connection"))

