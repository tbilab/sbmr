#' Visualize network stucture
#'
#' Creates a simple interactive network visualization that visually encodes node
#' membership and type (if present) for a passed network.
#'
#' @family visualizations
#'
#' @inheritParams verify_model
#' @param width Desired width for output widget.
#' @param height Desired height for output widget.
#' @param node_color_col Name of column in the `nodes` dataframe that should be
#'   encoded with color of nodes in the plot. If this value is numeric (defined
#'   as more than 10 unique values that are numbers) the network will color
#'   nodes on a linear scale from the min value to the highest value.
#' @param node_shape_col Name of column in the `nodes` dataframe that should be
#'   encoded with shape of nodes in the plot
#'
#' @return NULL
#' @export
#'
#' @examples
#'
#' # Simulate a 3 block/cluster network with 15 nodes per block
#' net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 15)
#'
#' # Visualize network with color encoding the blocks
#' visualize_network(net)
#'
#' # Visualize network with shape encoding the true blocks
#' visualize_network(net, node_shape_col = 'block')
#'
#' # Visualize network with color encoding the true blocks
#' visualize_network(net, node_color_col = 'block')
#'
visualize_network <- function(sbm,
                              node_shape_col = 'type',
                              node_color_col = "none",
                              width = "100%",
                              height = NULL){
  UseMethod("visualize_network")
}


#' @export
visualize_network.sbm_network <- function(sbm,
                                          node_shape_col = 'type',
                                          node_color_col = "none",
                                          width = "100%",
                                          height = NULL){
  nodes_for_plotting <- sbm$nodes

  if(node_color_col == "parent" | node_shape_col == "parent"){
    node_state <- attr(sbm, "state")

    if(is.null(node_state)){
      stop("Can't visualize block structure as model has not been initialized.")
    }

    nodes_for_plotting <- node_state %>%
      dplyr::filter(level == 0)
  }

  r2d3::r2d3(
    data = list(
      edges = sbm$edges,
      nodes = nodes_for_plotting
    ),
    options = list(
      color_col = node_color_col,
      shape_col = node_shape_col
    ),
    container = 'div',
    script = system.file("d3/network_viz.js", package = "sbmR"),
    width = width,
    height = height
  )
}
utils::globalVariables(c("parent"))

