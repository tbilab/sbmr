#' Choose and load best model state from agglomerative collapsing algorithm
#'
#' @family modeling
#' @seealso visualize_collapse_results
#'
#' @inheritParams mcmc_sweep
#' @inheritParams build_score_fn
#' @inheritParams calculate_collapse_score
#' @inheritParams visualize_collapse_results
#' @param verbose Should model tell you what step was chosen (`TRUE` or
#'   `FALSE`)?
#'
#'
#' @inherit new_sbm_network return
#' @export
#'
#' @examples
#' set.seed(42)
#'
#' # Start with a random network of two blocks with 25 nodes each
#' net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 25) %>%
#'   collapse_blocks(sigma = 1.4)
#'
#' # Choose best result with default heuristic
#' net <- choose_best_collapse_state(net, verbose = TRUE)
#'
#' # Score heuristic that fits a nonlinear model to observed values and chooses by
#' # largest negative residual
#' nls_score <- function(e, k){
#'   entropy_model <- nls(e ~ a + b * log(k), start = list(a = max(e), b = -25))
#'   -residuals(entropy_model)
#' }
#'
#' # Choose result using custom heuristic function
#' my_sbm <- choose_best_collapse_state(net,
#'                                      heuristic = nls_score,
#'                                      verbose = TRUE)
#'
choose_best_collapse_state <- function(sbm,
                                       heuristic = 'dev_from_rolling_mean',
                                       use_entropy = FALSE,
                                       verbose = FALSE){
  UseMethod("choose_best_collapse_state")
}

#' @export
choose_best_collapse_state.sbm_network <- function(sbm,
                                                   heuristic = 'dev_from_rolling_mean',
                                                   use_entropy = FALSE,
                                                   verbose = FALSE){

  best_state <- sbm %>%
    calculate_collapse_score(heuristic = heuristic,
                             use_entropy = use_entropy,
                             remove_state = FALSE) %>%
    dplyr::filter(score == max(score, na.rm = TRUE))

  if(verbose){
    n <- best_state$n_blocks[1]
    print(glue::glue("Choosing collapse with {n} blocks and an entropy of {round(best_state$entropy[1],4)}."))
  }

  # Update the model and s3 class states and return
  verify_model(sbm) %>% update_state(best_state$state[[1]])
}

utils::globalVariables(c("n_blocks", "entropy_delta", "score"))

