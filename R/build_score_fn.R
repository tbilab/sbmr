#' Build score function from heuristic
#'
#' (Internal) Takes a heuristic object and turns it into a function that takes
#' two arguments, entropy and number of blocks that is used to calculate the
#' score for a given entropy-number of blocks step for agglomerative merging
#' algorithms.
#'
#' @param heuristic How the best partitioning is defined. Takes either a
#'   function that takes one/two arguments: an entropy vector and an optional
#'   number of blocks vector with each element corresponding to a given
#'   location, or a string labeling algorithm. Currently only `"lowest"`,
#'   `"dev_from_rolling_mean"`, `"delta_ratio"`, and `"nls_residual"` are supported.
#'
#' @return A function that takes and entropy and number of block vector and
#'   returns a score for partitioning (higher = better)
#' @export
#'
#' @examples
#'
#' # Setup fake entropy and number of blocks vectors
#' entropy <- -(10:1)*1000 + rnorm(10, 200)
#' num_blocks <- 1:10
#'
#' # Works with heuristic functions that take two arguments
#' nls_score <- function(e, k){
#'   entropy_model <- nls(e ~ a + b * log(k), start = list(a = max(e), b = -25))
#'   -residuals(entropy_model)
#' }
#'
#' build_score_fn(nls_score)(entropy, num_blocks)
#'
#' # Works with functions that take one argument
#' invert_score <- function(e) -e/2
#' build_score_fn(invert_score)(entropy, num_blocks)
#'
#' # Works with predefined strings
#' build_score_fn("dev_from_rolling_mean")(entropy, num_blocks)
#' build_score_fn("lowest")(entropy, num_blocks)
#'
build_score_fn <- function(heuristic){

  heuristic_is_function <- 'function' %in% class(heuristic)

  if(heuristic_is_function){
    # If heuristic function only takes one argument its entropy, otherwise it
    # needs entropy and number of blocks
    just_entropy <- length(formals(heuristic)) == 1

    score_func <- function(entropy, num_blocks){
      if(just_entropy){
        res <- heuristic(entropy)
      } else {
        res <- heuristic(entropy, num_blocks)
      }
      res
    }
  } else {
    if(heuristic == 'lowest'){
      score_func <-  function(e, n) -e
    } else
    if(heuristic == 'dev_from_rolling_mean'){
      score_func <- function(e,n) rolling_mean(e) - e
    } else
    if(heuristic == 'nls_residual'){
      score_func <- function(e, k){
         entropy_model <- nls(e ~ a + b * log(k), start = list(a = max(e), b = -25))
         -residuals(entropy_model)
       }
    } else
    if(heuristic == 'delta_ratio'){
      score_func <- function(value, k){
        prev_value_norm <- dplyr::lag(value) / (dplyr::lag(k) - k)
        next_value_norm <- dplyr::lead(value) / (k - dplyr::lead(k))
        next_value_norm/prev_value_norm
      }
    } else
    {stop("Hueristic must be either a function or one of {\"lowest\", \"dev_from_rolling_mean\", \"nls_residual\"}.")}
  }
}
