#' Build score function from heuristic
#'
#' (Internal) Takes a heuristic object and turns it into a function that takes
#' two arguments, entropy and number of blocks that is used to calculate the
#' score for a given entropy-number of blocks step for agglomerative merging
#' algorithms.
#'
#' @family helpers
#'
#' @param heuristic How the best partitioning is defined. Takes either a
#'   function that takes one/two arguments: an entropy vector and an optional
#'   number of blocks vector with each element corresponding to a given
#'   location, or a string labeling algorithm. Currently only `"lowest"`,
#'   `"dev_from_rolling_mean"`, `"delta_ratio"`, `"trend_deviation"`, and `"nls_residual"` are supported.
#'
#' @return A function that takes and entropy and number of block vector and
#'   returns a score for partitioning (higher = better)
#' @export
#'
#' @examples
#'
#' # Setup fake entropy and number of blocks vectors
#' entropy <- -(10:1)*1000 + rnorm(10, 200)
#' n_blocks <- 1:10
#'
#' # Works with heuristic functions that take two arguments
#' nls_score <- function(e, k){
#'   entropy_model <- nls(e ~ a + b * log(k), start = list(a = max(e), b = -25))
#'   -residuals(entropy_model)
#' }
#'
#' build_score_fn(nls_score)(entropy, n_blocks)
#'
#' # Works with functions that take one argument
#' invert_score <- function(e) -e/2
#' build_score_fn(invert_score)(entropy, n_blocks)
#'
#' # Works with predefined strings
#' build_score_fn("dev_from_rolling_mean")(entropy, n_blocks)
#' build_score_fn("lowest")(entropy, n_blocks)
#'
build_score_fn <- function(heuristic){

  heuristic_is_function <- 'function' %in% class(heuristic)
  allowed_heuristics <- c('lowest', 'dev_from_rolling_mean', 'nls_residual',
                          'delta_ratio', 'trend_deviation')

  if(!heuristic_is_function){
    if(heuristic %>% not_in(allowed_heuristics)){
      stop(glue::glue(
        "Hueristic must be either a function or one of {{",
        paste(allowed_heuristics, collapse = ", "),
        "}}."
      ))
    }
  }

  function(value = NULL, k = NULL){
    if(heuristic_is_function){
      # If heuristic function only takes one argument its entropy, otherwise it
      # needs entropy and number of blocks
      just_entropy <- length(formals(heuristic)) == 1

      if(just_entropy){
        res <- heuristic(value)
      } else {
        res <- heuristic(value, k)
      }
    } else if (heuristic == 'lowest') {
      res <- -value
    } else if(heuristic == 'dev_from_rolling_mean'){
      res <- rolling_mean(value) - value
    } else if(heuristic == 'nls_residual'){
      entropy_model <- stats::nls(value ~ a + b * log(k), start = list(a = max(value), b = -25))
      res <- as.numeric(-stats::residuals(entropy_model))
    } else if(heuristic == 'delta_ratio'){
      window_size <- 2
      rolling_avg_at_k <- rolling_mean(value, window = window_size)
      rolling_avg_after <- dplyr::lead(rolling_avg_at_k, window_size)
      res <- rolling_avg_at_k/rolling_avg_after
    } else if(heuristic == 'trend_deviation'){
      x1 <- dplyr::lag(k)
      y1 <- dplyr::lag(value)
      x2 <- dplyr::lead(k)
      y2 <- dplyr::lead(value)
      # Distance between a line defined by two points and a given point
      res <- abs((y2 - y1)*k - (x2 - x1)*value + x2*y1 - y2*x1)/sqrt( (y2-y1)^2 + (x2-x1)^2 )
    } else {
      warning("Unrecognized heuristic. Returning untransformed value.")
      res <- value
    }
    res
  }
}
