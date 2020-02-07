#' Run a single MCMC sweep over nodes
#'
#' Runs a single Metropolis Hastings MCMC sweep across all nodes at specified
#' level (For algorithm details see
#' \href{https://arxiv.org/abs/1705.10225}{Piexoto, 2018}). Each node is given a
#' chance to move blocks or stay in current block and all nodes are processed in
#' random order. Takes the level that the sweep should take place on (int) and
#' if new blocks blocks can be proposed and empty blocks removed (boolean).
#'
#'
#' @family modeling
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
#' @inherit new_sbm_network return
#'
#' @export
#'
#' @examples
#'
#' set.seed(42)
#'
#' # Start with a small simulated network with random block assignments
#' net <- sim_basic_block_network(n_blocks = 4, n_nodes_per_block = 15) %>%
#'   initialize_blocks(num_blocks = 4)
#'
#' # Calculate entropy with random blocks
#' get_entropy(net)
#'
#' # Run some MCMC sweeps
#' net <- mcmc_sweep(net, num_sweeps = 25, variable_num_blocks = FALSE)
#'
#' # Entropy after sweeps
#' get_entropy(net)
#'
#' # Per-sweep level information
#' get_sweep_results(net)
#'
#' # Use track_pairs = TRUE to get an idea of node-pair similarity by looking at
#' # how often every pair of nodes is connected over sweeps
#' net %>%
#'   mcmc_sweep(num_sweeps = 25, track_pairs = TRUE) %>%
#'   get_sweep_pair_counts()
#'
#'
mcmc_sweep <- function(sbm,
                       num_sweeps = 1,
                       eps = 0.1,
                       variable_num_blocks = TRUE,
                       track_pairs = FALSE,
                       level = 0,
                       verbose = FALSE,
                       return_sbm_network = TRUE){
  UseMethod("mcmc_sweep")
}

mcmc_sweep.default <- function(sbm,
                               num_sweeps = 1,
                               eps = 0.1,
                               variable_num_blocks = TRUE,
                               track_pairs = FALSE,
                               level = 0,
                               verbose = FALSE,
                               return_sbm_network = TRUE){
  cat("mcmc_sweep generic")
}

#' @export
mcmc_sweep.sbm_network <- function(sbm,
                                   num_sweeps = 1,
                                   eps = 0.1,
                                   variable_num_blocks = TRUE,
                                   track_pairs = FALSE,
                                   level = 0,
                                   verbose = FALSE){
  sbm <- verify_model(sbm)

  results <- attr(sbm, 'model')$mcmc_sweep(as.integer(level),
                                  as.integer(num_sweeps),
                                  eps,
                                  variable_num_blocks,
                                  track_pairs,
                                  verbose)


  if (track_pairs) {
    # Clean up pair connections results
    results$pairing_counts <- results$pairing_counts %>%
      tidyr::separate(node_pair, into = c("node_a", "node_b"), sep = "--") %>%
      dplyr::mutate(proportion_connected = times_connected/num_sweeps)
  } else {
    # Remove the empty pair counts results
    results['pairing_counts'] <- NULL
  }

  # Update state attribute of s3 object
  attr(sbm, 'state') <- attr(sbm, 'model')$get_state()

  # Fill in the mcmc_sweeps property slot
  sbm$mcmc_sweeps <- results

  sbm
}

