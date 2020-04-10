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
#' # Small edge list to build network
#' edges <- dplyr::tribble(
#'   ~from, ~to,
#'   "a1"   , "b1"   ,
#'   "a1"   , "b2"   ,
#'   "a1"   , "b3"   ,
#'   "a2"   , "b1"   ,
#'   "a2"   , "b4"   ,
#'   "a3"   , "b1"
#' )
#'
#' # A small simulated network with no blocks
#' net <- new_sbm_network(edges)
#'
#' state(net)
#'
#' # Add some blocks and state will reflect
#' net %>% initialize_blocks(2) %>% state()
#'
state <- function(sbm){
  UseMethod("state")
}

state.default <- function(sbm){
  cat("state generic")
}

#' @export
state.sbm_network <- function(sbm){
  # Grab state from attribute and then swap in the string types
  attr(verify_model(sbm), 'state')
}

