#' Dump state of current SBM model to dataframe
#'
#' Exports the current state of the network as dataframe with each node as a row
#' and columns for node id, parent id, node type, and node level. Can also be
#' accomplished by `attr(sbm, 'state')`.
#'
#' @family modeling
#'
#' @inheritParams add_node
#'
#' @return A dataframe describing the current state of the network: \describe{
#'   \item{id}{id of node} \item{parent}{id of node's parent. `none` if node has
#'   no parent.} \item{type}{type of node} \item{level}{level of node in network
#'   hierarchy} }
#'
#' @export
#'
#' @examples
#'
#' # Build a bipartite network with 1 node and one block for each type
#' my_sbm <- create_sbm() %>%
#'   add_node('a1', type = 'a') %>%
#'   add_node('b1', type = 'b')
#'
#' my_sbm %>% get_state()
#'
get_state <- function(x){
  UseMethod("get_state")
}

get_state.default <- function(x){
  cat("get_state generic")
}

#' @export
get_state.sbm_network <- function(x){
  attr(verify_model(x), 'state')
}

