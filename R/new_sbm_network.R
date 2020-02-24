#' Builds a new sbm_network object from data
#'
#' Constructs an object with the `sbm_network` class. Takes as input at a
#' minumum an edge dataframe and will also accept a nodes dataframe that can
#' contain addition information about the nodes in the network. A helper for
#' allowing bipartite structure is provided which will treat either side of
#' passed edges dataframe as two different types of nodes.
#'
#' @section Class structure:
#'
#'   The `sbm_network` class is an S3 class with two minimum components:
#'
#'   \describe{ \item{`$edges`}{Edges that the network is built out of. This is
#'   simply the passed `edges` input to the function.} \item{`$nodes`}{Nodes
#'   that the edges connect. This is either the passed `nodes` input or is
#'   created from the `edges` input automatically by listing the unique node ids
#'   seen in the edges.} }
#'
#'
#'   If the method \code{\link{mcmc_sweep}} has been run on the `sbm_network`
#'   object then a `$sweep_result` slot will be added. For more info see
#'   \code{\link{mcmc_sweep}}.
#'
#'   If either of the methods \code{\link{collapse_groups}} or
#'   \code{\link{collapse_run}} have been applied to the `sbm_network` object
#'   then the slot `$collapse_result` will be append.
#'
#'   A few attributes are contained by the class as well:
#'
#'   \describe{ \item{`n_nodes`}{Number of unique nodes in the current model.
#'   Equivalent to `nrow(sbm_network$nodes)`.} \item{`n_edges`}{Number of edges
#'   in the current model. Equivalent to `nrow(sbm_network$edges)`.}
#'   \item{`from_column`}{Raw quosure representing the `edges_from_column`
#'   argument. This is kept so bipartite network types can be inferred and no
#'   modification of the passed `edges` dataframe needs to take place.}
#'   \item{`to_column`}{Same as `from_column`} \item{`model`}{ S4 class that is
#'   exported by the C++ code used to implement all the modeling algorithms.
#'   Most of the time the user should not have to interact with this object and
#'   thus it can be ignored.} }
#'
#'
#' @family model_setup
#'
#' @param edges Dataframe with a from and two column encoding edges between
#'   string node ids (direction does not matter).
#' @param nodes Optional dataframe that links a node `id` to its `type`, for
#'   when model is polypartite.
#' @param edges_from_column Name of the from column for edges
#' @param edges_to_column Name of the to column for edges
#' @param bipartite_edges Do the passed edges reflect a bipartite struture? I.e.
#'   are nodes in from `from` column of a different type to those in the `to`
#'   column?
#' @param setup_model Should an SBM model object be added? Set to `FALSE` if
#'   network is just being visualized or described.
#' @param default_node_type What should nodes that the type is generated for be
#'   called?
#' @param edge_types A dataframe in the same format of as `edges` that contains
#'   allowed pairs (order does not matter) of possible node type combinations
#'   across edges. For instance: nodes of type polinator are allowed to connect
#'   to flower nodes but not to other pollinator nodes. If this is left
#'   undefined, it is inferred from edges.
#' @param show_warnings Do you want to be warned when minor problems are
#'   detected by function? Usefull to disable when not running in an interactive
#'   mode etc.
#' @param random_seed Integer seed to be passed to model's internal random
#'   sampling engine. Note that if the model is restored from a saved state this
#'   seed will be initialized again to the start value which will harm
#'   reproducability.
#'
#' @return An S3 object of class `sbm_network`. For details see
#'   \link{new_sbm_network} section "Class structure."
#' @export
#'
#' @examples
#'
#' # Build small object from simple edge dataframe
#' edges <- dplyr::tribble(
#'   ~a_node, ~b_node,
#'   "a1"   , "b1"   ,
#'   "a1"   , "b2"   ,
#'   "a1"   , "b3"   ,
#'   "a2"   , "b1"   ,
#'   "a2"   , "b4"   ,
#'   "a3"   , "b1"
#' )
#'
#' new_sbm_network(edges, edges_from_column = a_node, edges_to_column = b_node)
#'
#' # Builds network from nodes and edges
#' nodes <- dplyr::tribble(
#'   ~id, ~type,
#'   "a1", "node",
#'   "a2", "node",
#'   "a3", "node",
#'   "b1", "node",
#'   "b2", "node",
#'   "b3", "node",
#'   "b4", "node"
#' )
#'
#' new_sbm_network(edges = edges,
#'                 nodes = nodes,
#'                 edges_from_column = a_node,
#'                 edges_to_column = b_node)
#'
#' # Can build a bipartite network just from an edge list
#' new_sbm_network(edges, bipartite_edges = TRUE,
#'                 edges_from_column = a_node,
#'                 edges_to_column = b_node)
#'
#' # Or if adding a node dataframe as well, arbitrary polypartite structure is possible
#' edges_tripartite <- dplyr::tribble(
#'   ~from,    ~to,
#'      "a1", "b1",
#'      "a1", "b2",
#'      "a2", "b1",
#'      "b1", "c1",
#'      "b2", "c1",
#'      "b2", "c2"
#' )
#'
#' nodes_tripartite <- dplyr::tribble(
#'   ~id, ~type,
#'   "a1", "a",
#'   "a2", "a",
#'   "b1", "b",
#'   "b2", "b",
#'   "c1", "c",
#'   "c2", "c"
#' )
#'
#' new_sbm_network(edges = edges_tripartite,
#'                 nodes = nodes_tripartite)
#'
#' # the edge_types argument lets you explicitely tell the
#' # model what each type of node is allowed to connect to.
#' # Here we use it to enforce tripartite structure where nodes of
#' # the a type can connect to either b or c but b and c can only
#' # connect to a.
#'
#' edge_types <- dplyr::tribble(~from, ~to,
#'                                        "a", "b",
#'                                        "a", "c")
#'
#' new_sbm_network(edges = edges_tripartite,
#'                 nodes = nodes_tripartite,
#'                 edge_types = edge_types)
#'
new_sbm_network <- function(edges = dplyr::tibble(),
                            nodes = NULL,
                            edges_from_column = from,
                            edges_to_column = to,
                            bipartite_edges = FALSE,
                            setup_model = TRUE,
                            edge_types = NULL,
                            default_node_type = "node",
                            show_warnings = interactive(),
                            random_seed = NULL){


  # Setup some tidy eval stuff for the column names
  to_column <- rlang::enquo(edges_to_column)
  from_column <- rlang::enquo(edges_from_column)
  to_col_string <- rlang::as_name(to_column)
  from_col_string <- rlang::as_name(from_column)

  # Get an idea of what kind of data we were given to drive logic
  missing_nodes <- is.null(nodes)

  # Checks if a column exists in a dataframe
  col_exists <- function(col_quo, df) rlang::as_name(col_quo) %in% colnames(df)

  # Constructs a properly formed nodes dataframe from just edges
  build_nodes_from_edges <- function(edges){
    # Check how we should type our nodes
    to_node_type <- if (bipartite_edges) to_col_string else default_node_type
    from_node_type <- if (bipartite_edges) from_col_string else default_node_type

    # Break edges down to unique nodes and assign appropriate types
    unique_to_nodes <- edges %>%
      dplyr::distinct(!!to_column) %>%
      dplyr::transmute(id = !!to_column,
                       type = to_node_type)

    unique_from_nodes <- edges %>%
      dplyr::distinct(!!from_column) %>%
      dplyr::transmute(id = !!from_column,
                       type = from_node_type)

    # Combine and count duplicates
    all_unique <- dplyr::bind_rows(unique_to_nodes, unique_from_nodes) %>%
      dplyr::group_by(.data$id) %>%
      dplyr::summarise(n_types = length(unique(.data$type)),
                       type = dplyr::first(.data$type) ) %>%
      dplyr::ungroup()

    # Check for nodes that have multiple types
    multi_type_nodes <- all_unique$n_types > 1
    if (any(all_unique$n_types > 1)) {
      stop("Bipartite edge structure was requested but some nodes appeared in both from and two columns of supplied edges.")
    }

    # Return built nodes, removing the n_types column
    all_unique %>% dplyr::select(-.data$n_types)
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
      nodes$type <- default_node_type
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
                 from_col_string))
    }

    if(!col_exists(to_column, edges)) {
      stop(paste("Edges data does not have the specified to column:",
                 to_col_string))
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
    if (bipartite_edges & show_warnings) {
      warning("bipartite_edges setting ignored due to nodes dataframe being provided.")
    }
  }

  # Next, make sure data fits together properly and report on any unconnected nodes
  unique_edge_ids <- unique(c(dplyr::pull(edges, !!from_column),
                              dplyr::pull(edges, !!to_column)))

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
    nodes <- dplyr::filter(nodes, not_in(id, unconnected_nodes))
  }

  # Build object
  x <- structure(list(nodes = nodes,
                      edges = edges),
                 class = "sbm_network",
                 n_nodes = nrow(nodes),
                 n_edges = nrow(edges),
                 from_column = from_column,
                 to_column = to_column,
                 edge_types = edge_types,
                 random_seed = random_seed)

  # Initialize a model if requested
  if (setup_model) {
    x <- verify_model(x, show_messages = FALSE)
  }

  # Return
  x
}

utils::globalVariables(c("from", "to"))

