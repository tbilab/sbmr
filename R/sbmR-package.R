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

# Set show method for object so we get something other than an ugly pointer to
# object
setClass("Rcpp_SBM")
setMethod( "show", "Rcpp_SBM" , function(object) {
  object$get_state()
})

# setGeneric("preview", function(object) standardGeneric("preview"))
# setMethod( "preview", "Rcpp_SBM" , function(object) {
#   print(object$get_state() %>% head())
# })



