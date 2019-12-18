#' Choose and load best model state from agglomerative collapsing algorithm
#'
#' @inheritParams mcmc_sweep
#' @param collapse_results Results of running agglomerative collapse algorithm
#'   on sbm with \code{\link{collapse_groups}}.
#' @param heuristic How the best partitioning is defined. Takes either a
#'   function that takes an entropy vector or a string labeling algorithm.
#'   Currently only `lowest` and `dev_from_rolling_mean` are supported.
#' @verbose Should model tell you what step was chosen (`TRUE` or `FALSE`)?
#'
#' @return SBM model updated to match state from the merge step with 'best'
#'   partitioning.
#' @export
#'
#' @examples
#'
choose_best_collapse_state <- function(sbm, collapse_results, heuristic = 'lowest', verbose = FALSE){

  heuristic_is_function <- 'function' %in% class(heuristic)


  if(heuristic_is_function){
    score_func <- heuristic
  } else {
    if(heuristic == 'lowest'){
      score_func <-  function(vec) -vec
    } else if(heuristic == 'dev_from_rolling_mean'){
      score_func <- function(vec) rolling_mean(vec) - vec
    } else {
      stop("Other choice heuristics not yet implemented.")
    }
  }

  # Apply the heuristic on the entropy column and choose the higheset value
  best_state <- collapse_results %>%
    mutate(score = score_func(entropy)) %>%
    filter(score == max(score))

  if(verbose){
    n <- best_state$num_groups[1]
    entropy <- best_state$entropy[1]
    print(glue("Choosing collapse with {n} groups and an entropy of {entropy}."))
  }

  # Load best state into model and return
  sbm %>% load_from_state(pluck(best_state$state, 1))
}
