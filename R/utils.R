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

# Get index type for a string type
get_type_index <- function(string_type, sbm){
  (attr(sbm, 'type_map') %>%
    dplyr::filter(type == string_type) %>%
    dplyr::pull(type_index))[1]
}

convert_to_type_index <- function(string_types, sbm){
  dplyr::tibble(type = string_types) %>%
    dplyr::left_join(attr(sbm, 'type_map'), by = 'type') %>%
    dplyr::pull(type_index)
}

convert_to_string_index <- function(integer_types, sbm){
  dplyr::tibble(type_index = integer_types) %>%
    dplyr::left_join(attr(sbm, 'type_map'), by = 'type_index') %>%
    dplyr::pull(type)
}

