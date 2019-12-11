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

