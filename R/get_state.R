#' Dump state of model to dataframe
#'
#' Exports the current state of the network as dataframe with each node as a row
#' and columns for node id, parent id, node type, and node level.
#'
#' @inheritParams add_node
#'
#' @return A dataframe describing the current state of the network: \describe{
#'   \item{id}{id of node}
#'   \item{parent}{id of node's parent. `none` if node has no parent.}
#'   \item{type}{type of node}
#'   \item{level}{level of node in network hierarchy}
#' }
#'
#' @export
#'
#' @examples
#'
#' # Build a bipartite network with 1 node and one block for each type
#' my_sbm <- create_sbm() %>%
#'   add_node('a1', type = 'a') %>%
#'   add_node('a11', type = 'a', level = 1) %>%
#'   add_node('b1', type = 'b') %>%
#'   add_node('b11', type = 'b', level = 1) %>%
#'   set_node_parent('a1', 'a11') %>%
#'   set_node_parent('b1', 'b11')
#'
#' my_sbm %>% get_state()
#'
get_state <- function(sbm){
  sbm$get_state()
}
