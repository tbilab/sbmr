#' Add edge between two nodes in network
#'
#' Connects two nodes in network (at level 0) by their ids (string).
#'
#' @inheritParams add_node
#' @param from_node Id of first node in edge
#' @param to_node Id of second node in edge
#' @param from_node_type Type of `from_node`
#' @param to_node_type Type of `to_node`
#' @param type_warning If node types are not specified the model will issue a
#'   warning and use the first node in networks type for both. To hide this
#'   warning for unipartite networks set this to `TRUE`.
#'
#' @return `sbm_network` object edge added.
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
  verify_model(sbm)$model$add_edge(from_node, to_node)

  # Update edge counts attribute
  attr(sbm, 'n_edges') <- attr(sbm, 'n_edges') + 1

  sbm
}

