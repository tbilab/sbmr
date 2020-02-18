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
#' get_state(net)
#'
#' # Add some blocks and state will reflect
#' net %>% initialize_blocks(2) %>% get_state()
#'
get_state <- function(x){
  UseMethod("get_state")
}

get_state.default <- function(x){
  cat("get_state generic")
}

#' @export
get_state.sbm_network <- function(x){
  # Grab state from attribute and then swap in the string types
  attr(verify_model(x), 'state') %>%
    dplyr::rename(type_index = type) %>%
    dplyr::left_join(attr(x, 'type_map'), by = 'type_index') %>%
    dplyr::select(-type_index)
}

