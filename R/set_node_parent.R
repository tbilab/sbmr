#' Set the parent of a node
#'
#' Sets the parent node (or block) for a given node. Takes child node's id
#' (string), parent node's id (string), and the level of child node (int).
#'
#' @family advanced
#'
#' @inheritParams add_node
#' @param child_id Id of node for which parent is being added
#' @param parent_id Id of parent node
#' @param level Level of child node. This will almost always stay at default
#'   value of `0`.
#'
#' @inherit new_sbm_network return
#' @export
#'
#' @examples
#'
#' # Create a model with two nodes, one at the block level (1)
#' # Start with bipartite network with 6 nodes
#' net <- dplyr::tribble(
#'   ~a_node, ~b_node,
#'   "a1"   , "b1"   ,
#'   "a1"   , "b2"   ,
#'   "a1"   , "b3"   ,
#'   "a2"   , "b1"   ,
#'   "a3"   , "b1"
#' ) %>%
#'   new_sbm_network(bipartite_edges = TRUE,
#'                   edges_from_col = a_node,
#'                   edges_to_col = b_node)
#'
#' get_state(net)
#'
#' # Make a parent node and assign it to a1
#' net <- set_node_parent(net, child_id = 'a1', parent_id = 'a1_parent')
#'
#' get_state(net)
#'
set_node_parent <- function(sbm, child_id, parent_id, level = 0){
  UseMethod("set_node_parent")
}

set_node_parent.default <- function(sbm, child_id, parent_id, level = 0){
  cat("set_node_parent generic")
}

#' @export
set_node_parent.sbm_network <- function(sbm, child_id, parent_id, level = 0){
  # Grab current state of model
  state <- attr(sbm, 'state')

  # Check if child is missing and throw error if so
  child_missing <- not_in(child_id, state$id)
  if(child_missing){
    stop(glue::glue("The child node ({child_id}) does not exist in model."))
  }

  # Get basic info about child
  child_index      <- which(state$id == child_id)
  child_type       <- state$type[child_index]
  child_level      <- state$level[child_index]

  # Modify the child's entry to have the new parent
  state$parent[child_index] <- parent_id

  # Check if the requested node parent is in the state currently...
  parent_missing <- not_in(parent_id, state$id)

  # Build parent entry in state if need be
  if(parent_missing){

    # Update state data
    state <- state %>%
      dplyr::bind_rows(dplyr::tibble(id = parent_id,
                                     parent = "none",
                                     type = child_type,
                                     level = child_level + 1))
  }

  # Force model to rebuild with the new state
  attr(sbm, 'model')$set_state(state$id,
                                     state$parent,
                                     state$level,
                                     state$type)

  # Make sure state attribute is updated
  attr(sbm, 'state') <- state

  sbm
}

