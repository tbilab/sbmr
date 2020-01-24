#' Choose and load best model state from agglomerative collapsing algorithm
#'
#' @inheritParams mcmc_sweep
#' @inheritParams build_score_fn
#' @inheritParams visualize_collapse_results
#' @param collapse_results Results of running agglomerative collapse algorithm
#'   on sbm with \code{\link{collapse_blocks}}.
#' @param verbose Should model tell you what step was chosen (`TRUE` or `FALSE`)?
#'
#' @return SBM model updated to match state from the merge step with 'best'
#'   partitioning.
#' @export
#'
#' @examples
#' set.seed(42)
#'
#' # Start with a random network of two blocks with 25 nodes each
#' network <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 25)
#'
#' # Create SBM from simulated data
#' my_sbm <- create_sbm(network)
#'
#' # Run agglomerative clustering with no intermediate MCMC steps on network
#' collapse_results <- collapse_run(my_sbm, sigma = 3, start_block_num = 1, end_block_num = 6)
#'
#' # Choose best result with default heuristic
#' my_sbm <- choose_best_collapse_state(my_sbm, collapse_results, verbose = TRUE)
#'
#' # Score heuristic that fits a nonlinear model to observed values and chooses by largest negative residual
#' nls_score <- function(e, k){
#'   entropy_model <- nls(e ~ a + b * log(k), start = list(a = max(e), b = -25))
#'   -residuals(entropy_model)
#' }
#'
#' # Choose result using custom heuristic function
#' my_sbm <- choose_best_collapse_state(my_sbm,
#'                                      collapse_results,
#'                                      heuristic = nls_score,
#'                                      verbose = TRUE)
#'
choose_best_collapse_state <- function(sbm, collapse_results, use_entropy_value_for_score = FALSE,  heuristic = 'dev_from_rolling_mean', verbose = FALSE){

  # Apply the heuristic on the entropy column and choose the higheset value
  best_state <- collapse_results %>%
    dplyr::arrange(num_blocks)

  if (use_entropy_value_for_score){
    best_state <- dplyr::mutate(best_state, score = build_score_fn(heuristic)(entropy, num_blocks))
  } else {
    best_state <- dplyr::mutate(best_state, score = build_score_fn(heuristic)(entropy_delta, num_blocks))
  }

  best_state <- dplyr::filter(best_state, score == max(score, na.rm = TRUE))

  if(verbose){
    n <- best_state$num_blocks[1]
    entropy <- best_state$entropy[1]
    print(glue::glue("Choosing collapse with {n} blocks and an entropy of {round(entropy,4)}."))
  }

  # Load best state into model and return
  sbm %>% load_from_state(best_state$state[[1]])
}
