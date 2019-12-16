#' sbmR
#'
#' Inference and visualization of network data using the Stochastic Block Model.
#'
#' Imports
#' @useDynLib sbmR, .registration = TRUE
#' @export SBM
#' @importFrom Rcpp loadModule
"_PACKAGE"

Rcpp::loadModule("SBM", TRUE)

# Set show method for object so we get something other than an ugly pointer to
# object
setClass("Rcpp_SBM")
setMethod( "show", "Rcpp_SBM" , function(object) {
  show(object$get_state())
})





