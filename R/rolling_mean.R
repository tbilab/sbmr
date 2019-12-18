#' Calculate a lagged rolling mean
#'
#' Returns a vector of the rolling mean of a vector calculated for a given
#' window size.
#'
#' The rolling mean is calculated using the available data. For elements at
#' start of the vector before a full window is able to fit, the mean is simply
#' taken with respect to the available values. E.g. for a window size of `3` the
#' first element will be the same as the source vector, the second will be the
#' second elements distance from mean of the first and second elements of the
#' source vector, and the remaining will be the mean of the element and the
#' previous two elements.
#'
#' This is used internally to compute elbow points in entropy calculations
#'
#' @param vec A vector of numeric values
#' @param window Size of rolling mean to compare against
#'
#' @return Vector of same length as `vec` with each element being the mean of
#'   the last `window` elements in `vec` (including current element).
#' @export
#'
#' @examples
#'
#' my_vec <- 1:25
#' rolled_mean <- rolling_mean(my_vec)
#' plot(rolled_mean)
#'
#' # An increased window size
#' rolled_mean_window_5 <- rolling_mean(my_vec, window = 5)
#' plot(rolled_mean_window_5)
#'
rolling_mean <- function(vec, window = 3){
  purrr::map_dbl(
    1:length(vec),
    function(i) mean( vec[max(i - window, 1): i], na.rm=TRUE)
  )
}
