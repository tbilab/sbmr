#' Create SBM
#'
#' Build an SBM instance from dataframe of edges. Optionally a nodes dataframe
#' can be provided to give information about node type.
#'
#' @param edges Dataframe with a from and two column encoding edges between string node ids (direction does not matter).
#' @param nodes Optional dataframe that links a node `id` to its `type`, for when model is polypartite.
#' @param from_col Name of the from column for edges
#' @param to_col Name of the to column for edges
#'
#' @return Instance of SBM class.
#' @export
#'
#' @examples
create_sbm <- function(edges, nodes = NULL, from_col = "from", to_col = "to"){

  # Break edges down to unique nodes. from_edges is used to find out if a node
  # isnt present in the edges but is in the optional nodes dataframe
  model_nodes <- dplyr::tibble(
    id = unique(c(edges[[from_col]], edges[[to_col]])),
    from_edges = TRUE
  )

  # If the user didn't provide a node dataframe, assume all nodes are the same
  # type
  if(is.null(nodes)){
    model_nodes$type = "a"
  } else {
    # Otherwise join the passed and internal nodes dataframes
    model_nodes <- dplyr::full_join(model_nodes, nodes, by = "id")

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

  # Instantiate instance of sbm class
  sbm <- new(SBM)

  # Fill in all the needed nodes
  for(i in 1:nrow(model_nodes)){
    sbm$add_node(model_nodes$id[i], model_nodes$type[i], 0L)
  }

  # Connect all the nodes
  for(i in 1:nrow(edges)){
    sbm$add_connection(
      edges[[from_col]][i],
      edges[[to_col]][i]
    )
  }

  # Return model
  sbm
}
