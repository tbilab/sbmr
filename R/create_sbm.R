#' Create SBM
#'
#' Build an SBM instance from dataframe of edges. Optionally a nodes dataframe
#' can be provided to give information about node type.
#'
#' @param edges Dataframe with a from and two column encoding edges between
#'   string node ids (direction does not matter). A list containing both a
#'   `nodes` and `edges` dataframe can be passed to avoid having to unpack a
#'   list of nodes and edges to the arguments of the same name. This is helpful
#'   for the included simulation functions such as
#'   \code{\link{sim_sbm_network}}.
#' @param nodes Optional dataframe that links a node `id` to its `type`, for when model is polypartite.
#' @param from_col Name of the from column for edges
#' @param to_col Name of the to column for edges
#' @param eps Epsilon value for ergodicity of move proposals of nodes.
#'
#' @return Instance of SBM class.
#' @export
#'
#' @examples
#' # Create an empty model with modified epsilon parameter
#' my_sbm <- create_sbm(eps = 0.4)
#'
create_sbm <- function(
  edges = NULL,
  nodes = NULL,
  from_col = from,
  to_col = to,
  eps = 0.1
){
  no_nodes <- is.null(nodes)
  no_edges <- is.null(edges)

  # If the user gave us a bundled list of nodes and edges as returned by our
  # simulation functions then extract the nodes from this
  if(!no_edges & is_list_class(edges)){

    # If the user didnt pass a seperate nodes dataframe and the list contains nodes
    # fill in the nodes object with nodes value in network list
    if(no_nodes & !is.null(edges$nodes)){
      nodes <- edges$nodes
      no_nodes <- FALSE
    }

    if(!is.null(edges$edges)){
      # Overwrite the list with the edges values of network list
      edges <- edges$edges
    }
  }

  if(!no_nodes){
    # Make sure nodes have a type column. If it's missing, just add a constant type
    have_type_col <- "type" %in% colnames(nodes)

    if(!have_type_col){
      nodes$type <- "node"
    }
  }


  # We have 4 total possiblilities for model specification
  no_data <- no_edges & no_nodes
  nodes_w_no_edges <- (!no_nodes) & no_edges
  edges_w_no_nodes <- (!no_edges) & no_nodes
  edges_and_nodes <- (!no_edges) & (!no_nodes)


  # Instantiate instance of sbm class
  sbm <- new(SBM)

  # Set all the free parameters
  sbm$EPS <- eps

  # If we have any edges should gather their unique nodes
  if(!no_edges){
    edges_to <- dplyr::pull(edges, !!rlang::enquo(to_col))
    edges_from <- dplyr::pull(edges, !!rlang::enquo(from_col))

    # Break edges down to unique nodes. from_edges is used to find out if a node
    # isnt present in the edges but is in the optional nodes dataframe
    nodes_from_edges <- dplyr::tibble(
      id = unique(c(edges_from, edges_to)),
      from_edges = TRUE
    )

  }



  if (no_data){

    # If the user just called the function empty, return the new empty SBM object.
    return(sbm)

  } else if (nodes_w_no_edges){

    # If user just passed us nodes, make model nodes dataframe out of that
    model_nodes <- nodes

  } else if(edges_w_no_nodes){
    # If the user didn't provide a node dataframe, assume all nodes are the same
    # type
    model_nodes <- dplyr::mutate(nodes_from_edges, type = "node")

  } else if(edges_and_nodes) {

    # If we have both edges and nodes, merge those dataframes
    model_nodes <- dplyr::full_join(nodes_from_edges, nodes, by = "id")

    # Check on how well the dataframes match up
    missing_from_nodes <- model_nodes$id[is.na(model_nodes$type)]
    missing_from_edges <- model_nodes$id[is.na(model_nodes$from_edges)]

    # Check to make sure all nodes are accounted for in user passed df
    if(length(missing_from_nodes) > 0){
      stop(paste("Passed nodes dataframe is missing node(s)",
                 paste(missing_from_nodes, collapse = ", "),
                 "from edges"))
    }

    if(length(missing_from_edges) > 0){
      warning(paste("Node(s)",
                    paste(missing_from_edges, collapse = ", "),
                    "are not seen in any of the edges"))
    }

  }



  # Fill in all the needed nodes
  for(i in 1:nrow(model_nodes)){
    sbm$add_node(model_nodes$id[i], model_nodes$type[i], 0L)
  }

  # Connect all the nodes if we have edges to do so
  if(!no_edges){
    for(i in 1:nrow(edges)){
      sbm$add_connection(
        edges_from[i],
        edges_to[i]
      )
    }
  }

  # Return model
  sbm
}
