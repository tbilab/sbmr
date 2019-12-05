
#' SBM class
#'
#' @slot edges Dataframe of Edges
#' @slot nodes Dataframe of Nodes
#' @slot model C++ SBM object
#' @slot node_types Dataframe mapping node string types to c++ friendly integers
#' @export
SBM <- setClass("SBM",
         slots = list(
           edges = "data.frame",
           nodes = "data.frame",
           model = "Rcpp_Rcpp_SBM",
           node_types = "data.frame")
)

setGeneric("add_node", function(x, id, type) standardGeneric("add_node"))
setMethod("add_node", "SBM", function(x, id, type){
  # Check for type in string -> int type map
  loc_of_type_string <- match(type, x@node_types$string)

  # If this is the first time that this type has been seen add it to list
  if(is.na(loc_of_type_string)){
    type_int <- nrow(x@node_types) + 1
    x@node_types <- rbind(
      x@node_types,
      c("string" = type, "int" = type_int)
    )
    print("This is a newly seen type")
  } else {
    # Otherwise just grab it from the dataframe
    type_int <- x@node_types$int[loc_of_type_string]
    print("Type has already been seen")
  }
  print(x@node_types)

  x@model$add_node(id, as.integer(type_int), 0L)

  x
})


#' Create SBM Object
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
SBM <- function(edges, nodes = NULL, from_col = "from", to_col = "to"){

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
      throw(paste("Passed nodes dataframe is missing node(s)",
                  paste(missing_from_nodes, collapse = ", "),
                  "from edges"))
    }

    if(length(missing_from_edges) > 0){
      warning(paste("Node(s)",
                    paste(missing_from_edges, collapse = ", "),
                    "are not seen in any of the edges"))
    }
  }

  # Construct class
  my_sbm <- new("SBM", edges = edges, nodes = nodes)

  # Setup the nodes types tracking df
  my_sbm@node_types <- dplyr::tibble(
    string = c("a"),
    int = c(1)
  )

  for(i in 1:nrow(model_nodes)){
    my_sbm <- my_sbm %>%
      add_node(model_nodes$id[i],  model_nodes$type[i])
  }

  my_sbm
}

