#' Visualize agglomerative collapse results
#'
#' Plots entropy against number of groups left for a collapse run. If
#' `heuristic` is set to value other than `NULL` a second plot of the score for
#' each merger step according to the heuristic provided is also shown.
#'
#' @param collapse_results Dataframe of agglomerative merging based collapse
#'   results as returned from \link{\code{collapse_run}}, or
#'   \link{\code{collapse_groups(report_all_steps = TRUE)}}.
#' @inheritParams build_score_fn
#'
#' @return GGplot object comparing the fit results and each step's deviance from
#'   the rolling mean
#' @export
#'
#' @examples
#' set.seed(42)
#'
#' # Start with a random network of two groups with 25 nodes each
#' network <- sim_basic_block_network(n_groups = 3, n_nodes_per_group = 25)
#'
#' # Run agglomerative clustering with no intermediate MCMC steps on sbm of simulated data
#' collapse_results <- create_sbm(network) %>%
#'   collapse_run(sigma = 3, start_group_num = 1, end_group_num = 8)
#'
#' # =============================================================================
#' # Visualize using no heuristic
#' visualize_collapse_results(collapse_results)
#'
#' # =============================================================================
#' # Visualize using deviance from rolling average heuristic
#' visualize_collapse_results(collapse_results, heuristic = 'dev_from_rolling_mean')
#'
#' # =============================================================================
#' # Visualize using custom heuristic
#'
#' # Score heuristic that fits a nonlinear model to observed values and chooses by
#' # largest negative residual
#' nls_score <- function(e, k){
#'   entropy_model <- nls(e ~ a + b * log(k), start = list(a = max(e), b = -25))
#'   -residuals(entropy_model)
#' }
#'
#' visualize_collapse_results(collapse_results, heuristic = nls_score)
#'
visualize_collapse_results <- function(collapse_results, heuristic = NULL){

  if(!is.null(heuristic)){
    collapse_results <- collapse_results %>%
      dplyr::mutate(
        score = build_score_fn(heuristic)(entropy, num_groups),
      )
  }

  collapse_results %>%
    dplyr::select(-state) %>%
    tidyr::pivot_longer(-num_groups) %>%
    ggplot2::ggplot(ggplot2::aes(x = num_groups, y = value)) +
    ggplot2::geom_point() +
    ggplot2::geom_line() +
    ggplot2::facet_grid(name~., scales = 'free_y')
}


