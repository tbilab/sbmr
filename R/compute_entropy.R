#' Compute entropy for current model state
#'
#' Computes the (degree-corrected) entropy for the network at the specified
#' level (int).
#'
#' @inheritParams add_node
#' @param level Level of nodes for which block entropy is accessed. E.g. the
#'   default value of `0` will get entropy of model as described by the blocks
#'   in level `1`.
#'
#' @return Entropy value (numeric).
#' @export
#'
#' @examples
#'
#' # Build basic network with 3 nodes and two blocks
#' my_sbm <- create_sbm() %>%
#'   add_node('node_1') %>%
#'   add_node('node_2') %>%
#'   add_node('node_3') %>%
#'   add_node('node_11', level = 1) %>%
#'   add_node('node_12', level = 1) %>%
#'   add_connection('node_1', 'node_2') %>%
#'   add_connection('node_1', 'node_3') %>%
#'   add_connection('node_2', 'node_3') %>%
#'   set_node_parent(child_id = 'node_1', parent_id = 'node_11') %>%
#'   set_node_parent(child_id = 'node_2', parent_id = 'node_11') %>%
#'   set_node_parent(child_id = 'node_3', parent_id = 'node_12')
#'
#' # Compute entropy of network
#' compute_entropy(my_sbm)
#'
compute_entropy <- function(sbm, level = 0){
  sbm$compute_entropy(as.integer(level))
}
