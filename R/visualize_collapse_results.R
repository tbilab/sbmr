plot_collapse_results <- function(collapse_results, rolling_window = 3){

  collapse_results %>%
    dplyr::mutate(
      window_avg = purrr::map_dbl(
        1:dplyr::n(),
        function(ind){
          mean(vec[((ind - window): ind) %>% subset(.>0)], na.rm=TRUE)
        }
      ),
      window_avg = ifelse(is.na(window_avg), dplyr::cummean(entropy), window_avg),
      `deviation from avg` = window_avg - entropy
    ) %>%
    dplyr::select(-state, -window_avg) %>%
    tidyr::pivot_longer(-num_groups) %>%
    ggplot2::ggplot(ggplot2::aes(x = num_groups, y = value)) +
    ggplot2::geom_point() +
    ggplot2::geom_line() +
    ggplot2::facet_grid(name~., scales = 'free_y')
}


