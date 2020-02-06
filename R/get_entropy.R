#' Compute entropy for current model state
#'
#' Computes the (degree-corrected) entropy for the network at the node level.
#'
#' @inheritParams add_node
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
#'   add_edge('node_1', 'node_2') %>%
#'   add_edge('node_1', 'node_3') %>%
#'   add_edge('node_2', 'node_3') %>%
#'   set_node_parent(child_id = 'node_1', parent_id = 'node_11') %>%
#'   set_node_parent(child_id = 'node_2', parent_id = 'node_11') %>%
#'   set_node_parent(child_id = 'node_3', parent_id = 'node_12')
#'
#' # Compute entropy of network
#' get_entropy(my_sbm)
#'
get_entropy <- function(x){
  set_generic("get_entropy")
}

get_entropy.default <- function(x){
  cat("get_entropy generic")
}

#' @export
get_entropy.sbm_network <- function(x){
  sbm$model$get_entropy(0L)
}
