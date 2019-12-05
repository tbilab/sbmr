#' sbmR
#'
#' Insert main package description here...
#'
#' Imports
#' @useDynLib sbmR, .registration = TRUE
#' @export Rcpp_SBM
#' @importFrom Rcpp loadModule
"_PACKAGE"

Rcpp::loadModule("Rcpp_SBM", TRUE)
