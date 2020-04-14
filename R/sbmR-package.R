#' Package reference
#'
#' Inference and visualization of network data using the Stochastic Block Model.
#'
#' Imports
#' @useDynLib sbmr, .registration = TRUE
#' @export SBM
#' @importFrom Rcpp loadModule
"_PACKAGE"


Rcpp::loadModule("SBM", TRUE)

#' SBM C++ class
#'
#' This is the R reference to the c++ class that implements SBM functionality.
#' Almost all interfacing with this methods will be taken care of with the s3
#' class `sbm_network` and thus this should be ignored for the vast majority of
#' users. Documentation of all available methods is available in the c++ wrapper
#' code using the RCPP module protocol. See \code{\link[Rcpp]{loadModule}} for more info.
#'
#' @aliases SBM
setClass("Rcpp_SBM")


# Set show method for object so we get something other than an ugly pointer to
# object
setMethod("show", "Rcpp_SBM" , function(object) {
  n_block_levels <- object$n_levels() - 1
  n_types <- object$n_types()

  if(n_types > 1){
    type_msg <- paste0(" of types {", paste(object$node_types(), collapse = ", "), "}")
  } else {
    type_msg <- ""
  }


  cat(paste0("SBM S4 class with ", object$n_nodes_at_level(0), " nodes", type_msg, " and ", object$n_edges(), " edges.\n"))


  if(n_block_levels > 1){
    cat(paste("There are", n_block_levels, "block levels. First block level has", object$n_nodes_at_level(1), "blocks."))
  } else {
    cat("There is no initialized block structure.")
  }
})



# On package unload make sure to unlink the dynamic c++ library
.onUnload <- function(libpath){
  library.dynam.unload("sbmr", libpath)
}


