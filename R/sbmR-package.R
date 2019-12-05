#' sbmR
#'
#' Insert main package description here...
#'
#' Imports
#' @useDynLib sbmR, .registration = TRUE
#' @export SBM
#' @importFrom Rcpp loadModule
"_PACKAGE"

Rcpp::loadModule("SBM", TRUE)
