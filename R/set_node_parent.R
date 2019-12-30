#' Set the parent of a node
#'
#' Sets the parent node (or block) for a given node. Takes child node's id
#' (string), parent node's id (string), and the level of child node (int).
#'
#' @inheritParams add_node
#' @param child_id Id of node for which parent is being added
#' @param parent_id Id of parent node
#' @param level Level of child node. This will almost always stay at default
#'   value of `0`.
#'
#' @return SBM model object modified with node hierarchy modified. \emph{Note that object is modified in place as well.}
#' @export
#'
#' @examples
#'
#' # Create a model with two nodes, one at the block level (1)
#' my_sbm <- create_sbm() %>%
#'   add_node('node_1') %>%
#'   add_node('node_11', level = 1)
#'
#' # Assign node 11 as node 1's parent
#' my_sbm %>%
#'   set_node_parent(child_id = 'node_1', parent_id = 'node_11')
#'
set_node_parent <- function(sbm, child_id, parent_id, level = 0){
  sbm$set_node_parent(child_id, parent_id, as.integer(level))
  sbm
}
