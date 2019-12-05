## usethis namespace: start
#' @useDynLib sbmR, .registration = TRUE
## usethis namespace: end
NULL

## usethis namespace: start
#' @importFrom Rcpp sourceCpp
## usethis namespace: end
NULL


# Tell package we're using RCPP modules
Rcpp::loadModule("Rcpp_SBM", TRUE)
