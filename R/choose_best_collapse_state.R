#' Choose and load best model state from agglomerative collapsing algorithm
#'
#' @inheritParams mcmc_sweep
#' @param collapse_results Results of running agglomerative collapse algorithm on sbm with \code{\link{collapse_groups}}.
#' @param heuristic How the best partitioning is defined. Currently only `'lowest' `is supported which chooses the point with lowest entropy.
#'
#' @return SBM model updated to match state from the merge step with 'best' partitioning.
#' @export
#'
#' @examples
#'
choose_best_collapse_state <- function(sbm, collapse_results, heuristic = 'lowest'){

  if(heuristic == 'lowest'){

    best_state <- collapse_results %>%
      filter(entropy == min(entropy)) %>%
      pull('state') %>%
      pluck(1)
  } else {
    stop("Other choice heuristics not yet implemented.")
  }


  my_sbm %>% load_from_state(best_state)
}
