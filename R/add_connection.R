#' Add edge between two nodes in network
#'
#' Connects two nodes in network (at level 0) by their ids (string).
#'
#' @inheritParams add_node
#' @param a_node Id of first node in edge
#' @param b_node Id of second node in edge
#'
#' @return SBM model object modified with edge added. \emph{Note that object is modified in place as well.}
#' @export
#'
#' @examples
#'
#' # Create SBM and add two nodes
#' my_sbm <- create_sbm() %>%
#'   add_node('node_1') %>%
#'   add_node('node_2')
#'
#' # Add edge between two nodes
#' my_sbm %>% add_edge('node_1', 'node_2')
#'
add_edge <- function(sbm, a_node, b_node){
  sbm$add_edge(a_node, b_node)
  sbm
}
