#' Builds a new sbm_network object from data
#'
#' Constructs an object with the `sbm_network` class. Takes as input at a
#' minumum an edge dataframe and will also accept a nodes dataframe that can
#' contain addition information about the nodes in the network. A helper for
#' allowing bipartite structure is provided which will treat either side of
#' passed edges dataframe as two different types of nodes.
#'
#' @param edges Dataframe with a from and two column encoding edges between
#'   string node ids (direction does not matter).
#' @param nodes Optional dataframe that links a node `id` to its `type`, for
#'   when model is polypartite.
#' @param edges_from_col Name of the from column for edges
#' @param edges_to_col Name of the to column for edges
#' @param bipartite_edges Do the passed edges reflect a bipartite struture? I.e.
#'   are nodes in from `from` column of a different type to those in the `to`
#'   column?
#' @param show_warnings Do you want to be warned when minor problems are
#'   detected by function? Usefull to disable when not running in an interactive
#'   mode etc.
#'
#' @return
#' @export
#'
#' @examples
#'
#' # Build small object from simple edge dataframe
#' small_edges <- dplyr::tribble(
#'   ~a_node, ~b_node,
#'   "a1"   , "b1"   ,
#'   "a1"   , "b2"   ,
#'   "a1"   , "b3"   ,
#'   "a2"   , "b1"   ,
#'   "a2"   , "b4"   ,
#'   "a3"   , "b1"
#' )
#'
#' new_sbm_network(small_edges, edges_from_col = a_node, edges_to_col = b_node)
#'
#' # Build larger object from a simulated dataset
#' network <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 40)
#'
#' new_sbm_network(edges = network$edges, nodes = network$nodes)
#'
new_sbm_network <- function(edges = dplyr::tibble(),
                            nodes = NULL,
                            edges_from_col = from,
                            edges_to_col = to,
                            bipartite_edges = FALSE,
                            show_warnings = interactive()){
  # Setup some tidy eval stuff for the column names
  to_column <- rlang::enquo(edges_to_col)
  from_column <- rlang::enquo(edges_from_col)

  # Get an idea of what kind of data we were given to drive logic
  missing_nodes <- is.null(nodes)

  # Syntax sugar
  not_in <- function(a, b) !(a %in% b)

  # Checks if a column exists in a dataframe
  col_exists <- function(col_quo, df) rlang::as_name(col_quo) %in% colnames(df)

  # Constructs a properly formed nodes dataframe from just edges
  build_nodes_from_edges <- function(edges){
    # Check how we should type our nodes
    to_node_type <- if (bipartite_edges) "to_node" else "node"
    from_node_type <- if (bipartite_edges) "from_node" else "node"

    # Break edges down to unique nodes and assign appropriate types
    unique_two_nodes <- edges %>%
      dplyr::distinct(!!to_column) %>%
      dplyr::transmute(
        id = !!to_column,
        type = to_node_type
      )

    unique_from_nodes <- edges %>%
      dplyr::distinct(!!from_column) %>%
      dplyr::transmute(
        id = !!from_column,
        type = from_node_type
      )

    # Combine and count duplicates
    all_unique <- dplyr::bind_rows(unique_two_nodes, unique_from_nodes) %>%
      dplyr::group_by(id) %>%
      dplyr::summarise(type = dplyr::first(type),
                       n_types = length(unique(type))) %>%
      dplyr::ungroup()

    # Check for nodes that have multiple types
    multi_type_nodes <- all_unique$n_types > 1
    if (any(all_unique$n_types > 1 )) {
      stop("Bipartite edge structure was requested but some nodes appeared in both from and two columns of supplied edges.")
    }

    # Return built nodes, removing the n_types column
    all_unique %>% dplyr::select(-n_types)
  }

  # Make sure nodes dataframe is properly formatted
  validate_nodes <- function(nodes){
    missing_id <- not_in('id', colnames(nodes))
    if (missing_id){
      stop("Nodes dataframe needs an id column.")
    }

    # Make sure nodes have a type column. If it's missing, just add a constant type
    missing_type <- not_in("type", colnames(nodes))
    if(missing_type){
      nodes$type <- "node"
    }

    nodes
  }

  # Make sure edges have the from and two columns we need
  validate_edges <- function(edges){

    # Check to make sure we have at least a few entries in our edges
    if(nrow(edges) == 0){
      stop("No edges provided")
    }

    # Make sure both the from and two columns exist
    if(!col_exists(from_column, edges)) {
      stop(paste("Edges data does not have the specified from column:",
                 rlang::as_name(from_column)))
    }

    if(!col_exists(to_column, edges)) {
      stop(paste("Edges data does not have the specified to column:",
                 rlang::as_name(to_column)))
    }

    # Return edge dataframe
    edges
  }

  # First validate our edges
  edges <- validate_edges(edges)

  # First make sure we have nodes in the proper format
  if (missing_nodes){
    # Build nodes if needed
    nodes <- build_nodes_from_edges(edges)
  } else {
    # Otherwise just make sure they are properly formatted
    nodes <- validate_nodes(nodes)

    # Let user know if their bipartite edge setting was unneccesary
    if (bipartite_edges) {
      warning("bipartite_edges setting ignored due to nodes dataframe being provided.")
    }
  }

  # Next, make sure data fits together properly and report on any unconnected nodes
  unique_edge_ids <- unique(c(edges[[rlang::as_name(from_column)]],
                              edges[[rlang::as_name(to_column)]]))
  unique_node_ids <- unique(nodes$id)
  edge_nodes_not_in_nodes <- not_in(unique_edge_ids, unique_node_ids)
  nodes_not_in_edges <- not_in(unique_node_ids,  unique_edge_ids)

  if (any(edge_nodes_not_in_nodes)) {
    stop(paste("Passed nodes dataframe is missing node(s)",
               paste(unique_edge_ids[edge_nodes_not_in_nodes], collapse = ", "),
               "from edges"))
  }

  if(any(nodes_not_in_edges)){
    unconnected_nodes <- unique_node_ids[nodes_not_in_edges]

    # Let the user know what happened
    if(show_warnings){
      warning(paste("Node(s)",
                    paste(unconnected_nodes, collapse = ", "),
                    "are not seen in any of the edges and have been removed from data."))
    }

    # If any nodes were unconnected by edges, remove them
    nodes <- nodes[nodes$id != unconnected_nodes]
  }

  # Build final structure
  structure(
    list(
      nodes = nodes,
      edges = edges
    ),
    class = "sbm_network",
    n_nodes = nrow(nodes),
    n_edges = nrow(edges),
    from_column = from_column,
    to_column = to_column
  )
}
