#' Add a new node to network
#'
#' Add a node to the network. Takes the node id (string), the node type
#' (string), and the node level (int).
#'
#' @param sbm `sbm_network` object as created by \code{\link{new_sbm_network()}}.
#' @param id Unique identifying name for node.
#' @param type Type of node (string). This is used to distinguish multipartite networks. E.g. "person", or "publication", etc..
#'
#' @return `sbm_network` object  with node added.
#' @export
#'
#' @examples
#'
#' my_sbm <- create_sbm() %>%
#'   add_node('node_1') %>%
#'   add_node('node_2')
#'
#' get_state(my_sbm)
#'
add_node <- function(sbm, id, type = NULL){
  UseMethod("add_node")
}

add_node.default <- function(sbm, id, type = NULL){
  cat("add_node generic")
}

#' @export
add_node.sbm_network <- function(sbm, id, type = NULL){

  node_not_in_network <- not_in(id, sbm_nodes$id)
  if(node_not_in_network){

    type_missing <- is.null(type)

    if(type_missing){
      type <- sbm$nodes$type[1]
      warning(glue::glue("{id} node not in network but has no specified type. Defaulting to {from_node_type}"))
    }

    # Add node to nodes list
    sbm$nodes <- dplyr::bind_rows(sbm$nodes,
                                  dplyr::tibble(id = id, type = type))
  } else {
    message(glue::glue("{id} node was already in network. No action taken."))
  }

  # Before we start modifying the model object, first verify it
  sbm <- verify_model(sbm)

  # Add node to s4 model class
  sbm$model$add_node(id, type, 0L)

  # Update state with new nodes [inefficient]
  attr(sbm, "state") <- sbm$model$get_state()

  sbm
}

