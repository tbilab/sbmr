#' Print network
#'
#' Addition arguments get passed to `head()` function.
#'
#' @family helpers
#'
#' @param x Object of class `sbm_network`. (See \code{\link{new_sbm_network}}
#'   for more info.)
#' @param ... Arguments passed to \code{\link[utils]{head}} that are used to
#'   control preview of node and edge data frames.
#'
#' @return Output of the number of nodes and edges in model along with
#'   indication of if a model has been initialized
#' @export
#'
#' @examples
#'
#' # Build small object from simple edge dataframe
#' small_edges <- dplyr::tribble(
#'   ~a_node, ~b_node,
#'      "a1",    "b1",
#'      "a1",    "b2",
#'      "a1",    "b3",
#'      "a2",    "b1",
#'      "a2",    "b4",
#'      "a3",    "b1"
#' )
#'
#' sbm_net <- new_sbm_network(small_edges, edges_from_col = a_node, edges_to_col = b_node)
#' print(sbm_net)
#'
print.sbm_network <- function(x, ...){
  N <- attr(x, "n_nodes")
  E <- attr(x, "n_edges")
  n_types <- length(attr(x, "node_types"))
  type_msg <- if(n_types > 1) glue::glue("of {n_types} types") else "of a single type"
  cat(glue::glue("SBM Network with {N} nodes {type_msg} and {E} edges."), "\n", "\n")

  cat("Nodes: ")
  print(utils::head(x$nodes, ...))
  cat("...", "\n","\n")

  cat("Edges: ")
  print(utils::head(x$edges, ...))
  cat("...", "\n","\n")

  if (!is.null(attr(x, 'state'))){
    cat(glue::glue("Model", "\n",
                   "Currently has {n_blocks(x)} blocks",
                   "\n"))
  }
}
