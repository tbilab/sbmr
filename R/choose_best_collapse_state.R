#' Choose and load best model state from agglomerative collapsing algorithm
#'
#' @family modeling
#' @seealso visualize_collapse_results
#'
#' @inheritParams mcmc_sweep
#' @inheritParams build_score_fn
#' @inheritParams visualize_collapse_results
#' @param verbose Should model tell you what step was chosen (`TRUE` or `FALSE`)?
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
#'   collapse_blocks(sigma = 2)
#'
#' # Choose best result with default heuristic
#' net <- choose_best_collapse_state(net, verbose = TRUE)
#'
#' # Score heuristic that fits a nonlinear model to observed values and chooses by largest negative residual
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
                                       use_entropy_value_for_score = FALSE,
                                       heuristic = 'dev_from_rolling_mean',
                                       verbose = FALSE){
  UseMethod("choose_best_collapse_state")
}

choose_best_collapse_state.default <- function(sbm,
                                               use_entropy_value_for_score = FALSE,
                                               heuristic = 'dev_from_rolling_mean',
                                               verbose = FALSE){
  cat("choose_best_collapse_state generic")
}

#' @export
choose_best_collapse_state.sbm_network <- function(sbm,
                                                   use_entropy_value_for_score = FALSE,
                                                   heuristic = 'dev_from_rolling_mean',
                                                   verbose = FALSE){
  collapse_results <- get_collapse_results(sbm)
  # Apply the heuristic on the entropy column and choose the higheset value
  best_state <- collapse_results %>%
    dplyr::arrange(num_blocks)

  missing_entropy_delta <- !("entropy_delta" %in% colnames(best_state))

  if(missing_entropy_delta & !use_entropy_value_for_score){
    best_state <- best_state %>%
      dplyr::mutate(entropy_delta = dplyr::lag(entropy) - entropy) %>%
      dplyr::filter(!is.na(entropy_delta))
  }

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

  # Update the model and s3 class states and return
  verify_model(sbm) %>% update_state(best_state$state[[1]])
}

