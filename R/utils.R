#' Pipe operator
#'
#' See \code{magrittr::\link[magrittr:pipe]{\%>\%}} for details.
#'
#' @name %>%
#' @rdname pipe
#' @keywords internal
#' @export
#' @importFrom magrittr %>%
#' @usage lhs \%>\% rhs
NULL



#' Get Combination Indices
#'
#' Generates a list with two arrays of indices that make up complete enumeration
#' of all combinations (order does _not_ matter) of elements of vector of length
#' `n`.
#'
#' @family helpers
#'
#' @param n Number of elements in vector to get combinations of
#' @param repeats Do we allow repeats in pairs? E.g. should a-a, b-b, etc. be allowed in generated pairs?
#'
#' @return A list with two vectors (`$a` and `$b`) containing integer indices of elements for every pair.
#'
#' @examples
#'
#' my_letters <- head(letters, 5)
#' letter_pairs <- get_combination_indices(length(my_letters), repeats = FALSE)
#'
#' dplyr::tibble(
#'   letter_1 = letters[letter_pairs$a],
#'   letter_2 = letters[letter_pairs$b]
#' )
#'
get_combination_indices <- function(n, repeats = FALSE){

  rep_counts <- n:1

  if(!repeats){
    rep_counts <- rep_counts - 1
  }

  list(
    a = rep(1:n, times = rep_counts),
    b = purrr::flatten_int( purrr::map(rep_counts, ~{tail(1:n, .x)}) )
  )
}


# Determine if an object is of the class list, not the type
is_list_class <- function(x){
  "list" %in% class(x)
}

# Syntax sugar
not_in <- function(a, b){
  !(a %in% b)
}

get_collapse_results <- function(x){
  collapse_results <- x$collapse_results
  if(is.null(collapse_results)) {
    stop("Need to run collapse_groups() or collapse_run() on network before visualizing.")
  }
  collapse_results
}

get_mcmc_sweep_results <- function(x){
  sweep_results <- x$mcmc_sweeps
  if(is.null(sweep_results)){
    stop("mcmc_sweep() must be run on network before results can be visualized.")
  }

  sweep_results
}

get_mcmc_pair_counts <- function(sweep_results){
  pair_counts <- sweep_results$pairing_counts

  # Make sure we have propensity counts before proceeding
  if(is.null(pair_counts)){
    stop("Sweep results do not contain pairwise propensities. Try rerunning MCMC sweep with track_pairs = TRUE.")
  }

  pair_counts
}
