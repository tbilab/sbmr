#' Add a new node to network
#'
#' Add a node to the network. Takes the node id (string), the node type
#' (string), and the node level (int).
#'
#' @family advanced
#'
#' @param sbm `sbm_network` object as created by
#'   \code{\link{new_sbm_network()}}.
#' @param id Unique identifying name for node.
#' @param type Type of node (string). This is used to distinguish multipartite
#'   networks. E.g. "person", or "publication", etc..
#' @inheritParams verify_model
#'
#' @return `sbm_network` object  with node added.
#' @export
#'
#' @examples
#'
#' # Start with network with 5 nodes.
#' net <- sim_random_network(n_nodes = 5, prob_of_edge = 1, setup_model = TRUE)
#'
#' # Verify nodes
#' net$nodes
#' net %>% get_state()
#'
#' # Add two nodes
#' net <- net %>%
#'   add_node('new_node1', type = 'node') %>%
#'   add_node('new_node2', type = 'node')
#'
#' # New nodes are added to node data and state
#' net$nodes
#' net %>% get_state()
#'
add_node <- function(sbm, id, type = NULL, show_messages = TRUE){
  UseMethod("add_node")
}

add_node.default <- function(sbm, id, type = NULL, show_messages = TRUE){
  cat("add_node generic")
}

#' @export
add_node.sbm_network <- function(sbm, id, type = NULL, show_messages = TRUE){

  type_missing <- is.null(type)
  node_not_in_network <- not_in(id, sbm$nodes$id)

  if(node_not_in_network){

    if(type_missing){
      type <- sbm$nodes$type[1]
      if(show_messages){
        message(glue::glue("{id} node not in network but has no specified type. Defaulting to {type}"))
      }
    }

    # Add node to nodes list
    sbm$nodes <- dplyr::bind_rows(sbm$nodes,
                                  dplyr::tibble(id = id, type = type))


    # Before we start modifying the model object, first verify it
    sbm <- verify_model(sbm)

    # Add node to s4 model class
    attr(sbm, 'model')$add_node(id, type, 0L)

    # Update state with new nodes [inefficient]
    attr(sbm, "state") <- attr(sbm, 'model')$get_state()

    # Update number of nodes
    attr(sbm, "n_nodes") <- attr(sbm, "n_nodes") + 1

  } else {

    if (!type_missing) {
      # If type was requested, make the node that exists has that requested
      # type. If it doesn't give error to the user.
      node_index <- which(sbm$nodes$id == id)
      prev_type <- sbm$nodes$type[node_index]
      if(prev_type != type){
        stop(glue::glue("{id} node was already in network with type {prev_type}.",
                        "Replacing with type with {type} would invalidate model state.",
                        "Try rebuilding model or using different node id"))
      }


    }

    if(show_messages){
      message(glue::glue("{id} node was already in network. No action taken."))
    }
  }

  sbm
}

