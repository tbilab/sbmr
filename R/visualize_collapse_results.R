#' Visualize agglomerative collapse results
#'
#' @param collapse_results Dataframe of agglomerative merging based collapse
#'   results as returned from \link{\code{collapse_run}}, or
#'   \link{\code{collapse_groups(report_all_steps = TRUE)}}.
#' @param rolling_window Size of rolling window used to calculate each steps deviance from the rolling mean...
#'
#' @return GGplot object comparing the fit results and each step's deviance from the rolling mean
#' @export
#'
#' @examples
plot_collapse_results <- function(collapse_results, rolling_window = 3){

  collapse_results %>%
    dplyr::mutate(
      window_avg = rolling_mean(entropy, rolling_window),
      `deviation from avg` = window_avg - entropy
    ) %>%
    dplyr::select(-state, -window_avg) %>%
    tidyr::pivot_longer(-num_groups) %>%
    ggplot2::ggplot(ggplot2::aes(x = num_groups, y = value)) +
    ggplot2::geom_point() +
    ggplot2::geom_line() +
    ggplot2::facet_grid(name~., scales = 'free_y')
}


