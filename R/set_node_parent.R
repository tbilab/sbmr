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
#' @return `sbm_network` object
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
  child_index <- which(state$id == child_id)
  child_type <- state$type[child_index]
  child_level <- state$level[child_index]

  # Modify the child's entry to have the new parent
  state$parent[child_index] <- parent_id

  # Check if the requested node parent is in the state currently...
  parent_missing <- not_in(parent_id, state$id)

  # Build parent entry in state if need be
  if(parent_missing){
    new_parent_entry <- dplyr::tibble(id = parent_id,
                                      parent = "none",
                                      type = child_type,
                                      level = child_level + 1)

    state <- dplyr::bind_rows(state,new_parent_entry)
  }

  # Force model to rebuild with the new state
  sbm$model$load_from_state(state$id,
                            state$parent,
                            state$level,
                            state$type)

  # Make sure state attribute is updated
  attr(sbm, 'state') <- state

  sbm
}

