#' Add edge between two nodes in network
#'
#' Connects two nodes in network (at level 0) by their ids (string).
#'
#' @family advanced
#' @inheritParams add_node
#' @param from_node Id of first node in edge
#' @param to_node Id of second node in edge
#' @param from_node_type Type of `from_node`
#' @param to_node_type Type of `to_node`
#' @param type_warning If node types are not specified the model will issue a
#'   warning and use the first node in networks type for both. To hide this
#'   warning for unipartite networks set this to `TRUE`.
#'
#' @inherit new_sbm_network return
#'
#' @export
#'
#' @examples
#'
#' # Start with network with 5 nodes all fully connected
#' net <- sim_random_network(n_nodes = 5, prob_of_edge = 1)
#'
#' # Fully connected network will have 10 edges
#' attr(net, 'n_edges')
#'
#' # Add a new edge between two non-yet-seen nodes
#' net <- net %>% add_edge('new_from_node', 'new_to_node')
#'
#' # We have two nodes added to our model
#' attr(net, 'n_nodes')
#'
#' # And one edge
#' attr(net, 'n_edges')
#'
#' # adding an edge between two nodes already in the network
#' net <- net %>% add_edge('new_from_node', 'new_to_node')
#'
#' # Another edge will be added
#' attr(net, 'n_edges')
#'
add_edge <- function(sbm,
                     from_node,
                     to_node,
                     from_node_type = NULL,
                     to_node_type = NULL,
                     show_messages = TRUE,
                     type_warning = TRUE){
  UseMethod("add_edge")
}

add_edge.default <- function(sbm,
                             from_node,
                             to_node,
                             from_node_type = NULL,
                             to_node_type = NULL,
                             show_messages = TRUE,
                             type_warning = TRUE){
  cat("add_edge generic")
}

#' @export
add_edge.sbm_network <- function(sbm,
                                 from_node,
                                 to_node,
                                 from_node_type = NULL,
                                 to_node_type = NULL,
                                 show_messages = TRUE,
                                 type_warning = TRUE){

  # Make sure these nodes exist already. If not add them
  from_node_missing <- not_in(from_node, sbm$nodes$id)
  if(from_node_missing){
    sbm <- add_node(sbm, id = from_node, type = from_node_type, show_messages = show_messages)
  }

  to_node_missing <- not_in(to_node, sbm$nodes$id)
  if(to_node_missing){
    sbm <- add_node(sbm, id = to_node, type = to_node_type, show_messages = show_messages)
  }

  # Add edge to tracked data
  sbm$edges <- dplyr::bind_rows(sbm$edges,
                                dplyr::tibble(!!attr(sbm, 'from_column') := from_node,
                                              !!attr(sbm, 'to_column') := to_node))
  # Add edge to SBM s4 class
  attr(verify_model(sbm), 'model')$add_edge(from_node, to_node)

  # Update edge counts attribute
  attr(sbm, 'n_edges') <- attr(sbm, 'n_edges') + 1

  sbm
}

