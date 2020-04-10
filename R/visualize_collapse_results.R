#' Visualize agglomerative collapse results
#'
#' Plots entropy against number of blocks left for a collapse run. If
#' `heuristic` is set to value other than `NULL` a second plot of the score for
#' each merger step according to the heuristic provided is also shown.
#'
#' Either \code{\link{collapse_run}}, or `collapse_blocks(report_all_steps =
#' TRUE)` must be run prior to calling this function. (See
#' \code{\link{collapse_blocks}}.)
#'
#' @family visualizations
#'
#' @inheritParams verify_model
#' @inheritParams calculate_collapse_score
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
#' # Start with a random networ and run agglomerative clustering with no
#' # intermediate MCMC steps on sbm of simulated data
#' net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 35) %>%
#'   collapse_blocks(sigma = 1.5)
#'
#' # =============================================================================
#' # Visualize using no heuristic
#' visualize_collapse_results(net)
#'
#' # =============================================================================
#' # Visualize using deviance from rolling average heuristic
#' visualize_collapse_results(net, heuristic = 'dev_from_rolling_mean')
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
#' visualize_collapse_results(net, heuristic = nls_score)
#'
visualize_collapse_results <- function(sbm, heuristic = NULL, use_entropy = FALSE){
  UseMethod("visualize_collapse_results")
}


#' @export
visualize_collapse_results.sbm_network <- function(sbm, heuristic = NULL, use_entropy = FALSE){

  calculate_collapse_score(sbm, heuristic = heuristic, use_entropy = use_entropy) %>%
    tidyr::pivot_longer(-n_blocks) %>%
    dplyr::mutate(name = stringr::str_replace_all(name, "_", " ")) %>%
    ggplot2::ggplot(ggplot2::aes(x = n_blocks, y = value)) +
    ggplot2::geom_point() +
    ggplot2::geom_line() +
    ggplot2::facet_grid(name~., scales = 'free_y') +
    ggplot2::labs(x = "number of blocks", y = "")
}

utils::globalVariables(c("entropy", "n_blocks", "state", "name"))

