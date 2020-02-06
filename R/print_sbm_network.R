#' Print network
#'
#' @family helpers
#'
#' @inheritParams verify_model
#'
#' @return Output of the number of nodes and edges in model along with
#'   indication of if a model has been initialized
#' @export
#'
#' @examples
#' # Build small object from simple edge dataframe
#' small_edges <- dplyr::tribble(
#'   ~a_node, ~b_node,
#'   "a1"   , "b1"   ,
#'   "a1"   , "b2"   ,
#'   "a1"   , "b3"   ,
#'   "a2"   , "b1"   ,
#'   "a2"   , "b4"   ,
#'   "a3"   , "b1"
#' )
#'
#' sbm_net <- new_sbm_network(small_edges, edges_from_col = a_node, edges_to_col = b_node)
#' print(sbm_net)
#'
print.sbm_network <- function(x){

  N <- attr(x, "n_nodes")
  E <- attr(x, "n_edges")
  cat(glue::glue("SBM Network with {N} nodes and {E} edges."), "\n")

  if (!is.null(x$model)){
    cat("The network has an initialized model object.", "\n")
  }
}
