#' Run a single MCMC sweep over nodes
#'
#' Runs a single Metropolis Hastings MCMC sweep across all nodes at specified
#' level (For algorithm details see
#' \href{https://arxiv.org/abs/1705.10225}{Piexoto, 2018}). Each node is given a
#' chance to move blocks or stay in current block and all nodes are processed in
#' random order. Takes the level that the sweep should take place on (int) and
#' if new blocks blocks can be proposed and empty blocks removed (boolean).
#'
#' @inheritParams add_node
#' @inheritParams collapse_blocks
#' @param num_sweeps Number of times all nodes are passed through for move
#'   proposals.
#' @param level Level of nodes who's blocks will have their block membership run
#'   through MCMC proposal-accept routine.
#' @param variable_num_blocks Should the model allow new blocks to be created or
#'   empty blocks removed while sweeping or should number of blocks remain
#'   constant?
#' @param track_pairs Return a dataframe with all pairs of nodes along with the
#'   number of sweeps they shared the same group?
#' @param verbose If set to `TRUE` then each proposed move for all sweeps will
#'   have information given on entropy delta, probability of moving, and if the
#'   move were accepted printed to the console.
#'
#' @return List with two dataframes. The first telling for all sweeps everytime
#'   a node was moved and what group it was moved to. The second telling for
#'   each sweep the entropy delta and total number of nodes that were moved to
#'   new groups in that sweep. If `track_pairs = TRUE`, then an additional
#'   `pairing_counts` dataframe is added to output.
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
#' # Calculate entropy with random blocks
#' compute_entropy(my_sbm)
#'
#' # Run 4 MCMC sweeps
#' sweep_results <- my_sbm %>% mcmc_sweep(num_sweeps = 4, variable_num_blocks = FALSE)
#'
#' # Look at the per-sweep level information
#' sweep_results$sweep_info
#'
#' # Get idea of node-pair similarity by looking at how often every pair of nodes is connected over sweeps
#' my_sbm %>% mcmc_sweep(num_sweeps = 4, track_pairs = TRUE)
#'
mcmc_sweep <- function(sbm, num_sweeps = 1, variable_num_blocks = TRUE, eps = 0.1, track_pairs = FALSE, level = 0, verbose = FALSE){

  results <- sbm$mcmc_sweep(as.integer(level),
                 as.integer(num_sweeps),
                 eps,
                 variable_num_blocks,
                 track_pairs,
                 verbose)

  if (track_pairs) {
    # Clean up pair connections results
    results$pairing_counts <-  results$pairing_counts %>%
      tidyr::separate(node_pair, into = c("node_a", "node_b"), sep = "--") %>%
      dplyr::mutate(proportion_connected = times_connected/num_sweeps)
  } else {
    # Remove the empty pair counts results
    results['pairing_counts'] <- NULL
  }

  results
}
