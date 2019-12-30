#' Add a new node to network
#'
#' Add a node to the network. Takes the node id (string), the node type
#' (string), and the node level (int).
#'
#' @param sbm SBM model object as created by \code{\link{create_sbm()}}.
#' @param id Unique identifying name for node.
#' @param type Type of node (string). This is used to distinguish multipartite networks. E.g. "person", or "publication", etc..
#' @param level Level in node hierarchy. Level `0` indicates node is at the data level, levels greater than `0` are block nodes.
#'
#' @return SBM model object modified with node added. \emph{Note that object is modified in place as well.}
#' @export
#'
#' @examples
#'
#' my_sbm <- create_sbm() %>%
#'   add_node('node_1') %>%
#'   add_node('node_2')
#'
#' get_state(my_sbm)
#'
add_node <- function(sbm, id, type = "node", level = 0){
  sbm$add_node(id, type, as.integer(level))
  sbm
}
