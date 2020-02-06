#' Visualize agglomerative collapse results
#'
#' Plots entropy against number of blocks left for a collapse run. If
#' `heuristic` is set to value other than `NULL` a second plot of the score for
#' each merger step according to the heuristic provided is also shown.
#'
#' @family visualizations
#'
#' @param collapse_results Dataframe of agglomerative merging based collapse
#'   results as returned from \link{\code{collapse_run}}, or
#'   \link{\code{collapse_blocks(report_all_steps = TRUE)}}.
#' @param use_entropy_value_for_score If set to `TRUE` then instead of the merge results
#'   entropy delta being used for the score function the entropy will be.
#'   Typically the heuristics work better on entropy delta values.
#' @inheritParams build_score_fn
#'
#' @return GGplot object comparing the fit results and each step's deviance from
#'   the rolling mean
#' @export
#'
#' @examples
#'
#' set.seed(42)
#'
#' # Start with a random network
#' net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 35)
#'
#' # Run agglomerative clustering with no intermediate MCMC steps on sbm of simulated data
#' collapse_results <- collapse_blocks(net, sigma = 1.5)
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
#' # largest negative residual (built into package as heuristic = "nls_residual")
#' nls_score <- function(e, k){
#'   entropy_model <- nls(e ~ a + b * log(k), start = list(a = max(e), b = -25))
#'   -residuals(entropy_model)
#' }
#'
#' visualize_collapse_results(collapse_results, heuristic = nls_score)
#'
visualize_collapse_results <- function(collapse_results, use_entropy_value_for_score = FALSE, heuristic = NULL){

  if(!is.null(heuristic)){

    collapse_results <- collapse_results %>%
      dplyr::arrange(num_blocks)

    if (use_entropy_value_for_score){
      collapse_results <- dplyr::mutate(collapse_results, score = build_score_fn(heuristic)(entropy, num_blocks))
    } else {
      collapse_results <- dplyr::mutate(collapse_results, score = build_score_fn(heuristic)(entropy_delta, num_blocks))
    }

  }

  collapse_results %>%
    dplyr::select(-state) %>%
    tidyr::pivot_longer(-num_blocks) %>%
    dplyr::mutate(
      name = stringr::str_replace_all(name, "_", " ")
    ) %>%
    ggplot2::ggplot(ggplot2::aes(x = num_blocks, y = value)) +
    ggplot2::geom_point() +
    ggplot2::geom_line() +
    ggplot2::facet_grid(name~., scales = 'free_y') +
    ggplot2::labs(x = "number of blocks", y = "")
}


